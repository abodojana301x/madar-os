#include "api_log.h"
#include <kprintf.h>

static uint32_t g_total_calls = 0;
static uint32_t g_failed_calls = 0;

void compat_api_log_reset(void)
{
    g_total_calls = 0;
    g_failed_calls = 0;
}

void compat_api_log_call(const char *name, int status)
{
    g_total_calls++;
    if (status != 0) g_failed_calls++;
    kprintf("[compat-api] %s => %s\n", name ? name : "unknown", status == 0 ? "OK" : "ERR");
}

uint32_t compat_api_log_total_calls(void)
{
    return g_total_calls;
}

uint32_t compat_api_log_failed_calls(void)
{
    return g_failed_calls;
}
