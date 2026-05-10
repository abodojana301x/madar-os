#ifndef BOOT_DIAG_H
#define BOOT_DIAG_H

#include <types.h>

void boot_diag_register_stage(const char *name);
void boot_diag_report_stage(int stage_id, int success);
void boot_diag_fail(const char *msg);
int boot_diag_has_failed(void);
void boot_diag_screen(void);

#endif
