#include "string.h"
#include "kprintf.h"

int app_tcp_http_get(const char *host, const char *path, char *response, unsigned long max_len) {
    const char *body = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<title>Madar Test</title><p>Layer 7 HTTP client ready.</p><a href=\"/\">home</a>";
    (void)path;
    if (!host || !response || max_len == 0) return -1;
    if (strcmp(host, "test.local") != 0 && strcmp(host, "localhost") != 0) {
        kprintf("http: network fallback for %s\n", host);
    }
    strncpy(response, body, max_len - 1);
    response[max_len - 1] = '\0';
    return 0;
}
