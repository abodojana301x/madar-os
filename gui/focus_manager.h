#ifndef GUI_FOCUS_MANAGER_H
#define GUI_FOCUS_MANAGER_H

#include <types.h>

/* Set focus to a specific window */
void focus_set(uint32_t id);

/* Get currently focused window ID */
uint32_t focus_get(void);

#endif
