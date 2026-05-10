/*
 * Layer: 7 (User Applications) - HTTP/1.1 text client
 * File: http_client.c
 * Purpose: Safe text-only HTTP client facade over the Layer 4 TCP service.
 */

#include "http_client.h"
#include "kprintf.h"
#include "string.h"

int html_min_extract_text(const char *html, char *out, unsigned long max_len);
int app_tcp_http_get(const char *host, const char *path, char *response, unsigned long max_len);

static int parse_url(const char *url, char *host, unsigned long host_len, char *path, unsigned long path_len) {
    const char *p = url;
    if (!url || !host || !path) return -1;
    if (strncmp(p, "http://", 7) == 0) p += 7;
    unsigned long i = 0;
    while (*p && *p != '/' && i + 1 < host_len) host[i++] = *p++;
    host[i] = '\0';
    if (host[0] == '\0') return -1;
    strncpy(path, *p ? p : "/", path_len - 1);
    path[path_len - 1] = '\0';
    return 0;
}

int http_client_get(const char *url, char *out, unsigned long max_len) {
    char host[96];
    char path[128];
    char response[512];
    if (!out || max_len == 0 || parse_url(url, host, sizeof(host), path, sizeof(path)) != 0) return -1;
    if (app_tcp_http_get(host, path, response, sizeof(response)) != 0) return -1;
    return html_min_extract_text(response, out, max_len);
}

int http_client_selftest(void) {
    char text[256];
    kprintf("Testing HTTP Client...\n");
    if (http_client_get("http://test.local/index.html", text, sizeof(text)) < 0) return -1;
    if (!strstr(text, "Madar Test")) return -1;
    kprintf("HTTP Client PASSED\n");
    return 0;
}
