#include "shell.h"
#include "string.h"
#include "kprintf.h"

static int contains_token(const char *s, const char *tok) {
    size_t n = strlen(tok);
    if (!s || !tok || n == 0) return 0;
    for (size_t i = 0; s[i]; i++) {
        if (strncmp(&s[i], tok, n) == 0) return 1;
    }
    return 0;
}

lang_t detect_lang(const char* input) {
    if (contains_token(input, "ls") || contains_token(input, "cd") || contains_token(input, "cat")) return LANG_LINUX;
    if (contains_token(input, "show") || contains_token(input, "create")) return LANG_ENGLISH;
    if (contains_token(input, "اعرض") || contains_token(input, "انشئ")) return LANG_ARABIC;
    return LANG_LINUX; // Default
}
