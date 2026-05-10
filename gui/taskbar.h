#ifndef GUI_TASKBAR_H
#define GUI_TASKBAR_H

#include <types.h>

void taskbar_init(void);
void taskbar_draw(void);
int taskbar_click(int x, int y, uint32_t *window_id);

#endif
