#include "kprintf.h"
#include "../apps/shell.h"
#include "../apps/fileman.h"
#include "../apps/http_client.h"

int stress_suite_run(void) {
    char text[128];
    kprintf("Testing Layer 7 Stress Suite...\n");
    for (int i = 0; i < 5; i++) {
        if (shell_execute_line("echo stress") != 0) return -1;
    }
    if (fileman_copy("/home/guest/readme.txt", "/home/guest/stress-copy.txt") != 0) return -1;
    if (http_client_get("http://test.local/", text, sizeof(text)) < 0) return -1;
    kprintf("Stress Suite PASSED\n");
    return 0;
}
