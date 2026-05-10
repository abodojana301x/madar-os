#include "string.h"

int html_min_extract_text(const char *html, char *out, unsigned long max_len) {
    unsigned long j = 0;
    int in_tag = 0;
    int pending_space = 0;
    if (!html || !out || max_len == 0) return -1;
    for (unsigned long i = 0; html[i] && j + 1 < max_len; i++) {
        char c = html[i];
        if (c == '<') { in_tag = 1; pending_space = 1; continue; }
        if (c == '>') { in_tag = 0; continue; }
        if (in_tag) continue;
        if (c == '\r' || c == '\n' || c == '\t') c = ' ';
        if (pending_space && j > 0 && out[j - 1] != ' ') out[j++] = ' ';
        pending_space = 0;
        out[j++] = c;
    }
    out[j] = '\0';
    return (int)j;
}
