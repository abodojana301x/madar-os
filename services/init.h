/*
 * Layer: 4 (System Services) - Init Process
 * File: init.h
 * Purpose: PID 1 initialization logic
 */

#ifndef INIT_H
#define INIT_H

#include <types.h>

#define INIT_CFG_PATH       "/etc/init.cfg"
#define INIT_CHECK_INTERVAL 5000  /* 5 seconds in ms */

void init_start(void);
void init_main_loop(void);

#endif

