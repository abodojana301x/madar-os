#ifndef COMPAT_API_LOG_H
#define COMPAT_API_LOG_H

#include <types.h>

void compat_api_log_reset(void);
void compat_api_log_call(const char *name, int status);
uint32_t compat_api_log_total_calls(void);
uint32_t compat_api_log_failed_calls(void);

#endif
