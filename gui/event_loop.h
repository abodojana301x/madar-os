#ifndef GUI_EVENT_LOOP_H
#define GUI_EVENT_LOOP_H

#include <types.h>

typedef enum {
    GUI_EVENT_NONE = 0,
    GUI_EVENT_MOUSE_MOVE,
    GUI_EVENT_MOUSE_DOWN,
    GUI_EVENT_MOUSE_UP,
    GUI_EVENT_MOUSE_CLICK,
    GUI_EVENT_MOUSE_RIGHT_DOWN,
    GUI_EVENT_MOUSE_RIGHT_UP,
    GUI_EVENT_KEY
} gui_event_type_t;

typedef struct {
    gui_event_type_t type;
    int x;
    int y;
    uint32_t key;
    uint8_t ascii;
    uint8_t modifiers;
    uint32_t mod;
} gui_event_t;

void event_loop_init(void);
int event_loop_step(void);
void event_loop_inject(const gui_event_t *ev);
uint32_t event_loop_latency_ms(void);

#endif
