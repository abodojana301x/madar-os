#ifndef GUI_INPUT_POLLER_H
#define GUI_INPUT_POLLER_H

#include <types.h>

/* Initialize input poller (connects keyboard/mouse drivers to event loop) */
void input_poller_init(void);

/* Poll keyboard and mouse for events and inject into event loop */
void input_poller_poll(void);

#endif
