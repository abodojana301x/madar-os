#include "kprintf.h"

void web_view_show_text(const char *text) {
    kprintf("%s\n", text ? text : "");
}
