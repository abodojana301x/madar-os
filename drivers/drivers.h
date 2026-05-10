/*
 * Layer: 3 (Drivers & Hardware Abstraction)
 * File: drivers.h
 * Purpose: Unified drivers interface
 * Status: New for Layer 3
 */

#ifndef DRIVERS_H
#define DRIVERS_H

/* Include all drivers */
#include "pci.h"
#include "fb.h"
#include "kbd.h"
#include "mouse.h"
#include "ata.h"
#include "rtl8139.h"
#include "irq_manager.h"

/* Layer 3 initialization */
void drivers_init(void);
void drivers_test(void);

#endif /* DRIVERS_H */
