#ifndef ZPU_HAL_EVENTS_H
#define ZPU_HAL_EVENTS_H

#include <stdint.h>

enum eventsyserr {
    ESYSERR_NONE,
    ESYSERR_MALLOC
};

enum eventbits
{
    EVENT_NONE      = 0,
    EVENT_QUIT      = 1 << 0,
    EVENT_KEYDOWN   = 1 << 1,
    EVENT_KEYUP     = 1 << 2,
    EVENT_MOUSE     = 1 << 3
};

typedef uint16_t eventflags;

struct kybdevent {
    int     scancode;
    int     ascii;
    uint8_t modkeys;
};

struct mouseevent {
    int     x, y;
    uint8_t buttonstate;
};

struct event {
    eventflags type;
    union {
        struct kybdevent    kybd;
        struct mouseevent   mouse;
        unsigned char       quit;
    };
};

enum {
    EVENTQUEUE_WHENFULL_IGNORE,
    EVENTQUEUE_WHENFULL_RMOLD
};

struct eventqueue {
    eventflags  eventflags;
    int         fullbehaviour;
    int         maxsz;
    int         front, back;
};


enum eventsyserr evsys_initeventsys(eventflags eflags);
void evsys_wait(void);
struct event *evsys_hasevents(void);
struct event *evsys_get(void);
struct event *evsys_peek(void);
struct event *evsys_del(void);


#endif /* ZPU_HAL_EVENTS_H */
