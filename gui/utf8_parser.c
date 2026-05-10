#include <types.h>

int utf8_decode_next(const char *s, uint32_t *cp, uint32_t *advance)
{
    if (!s || !cp || !advance) return -1;
    uint8_t c = (uint8_t)s[0];
    if (c == 0) return -1;
    if ((c & 0x80) == 0) {
        *cp = c;
        *advance = 1;
        return 0;
    }
    *cp = '?';
    *advance = 1;
    return 0;
}
