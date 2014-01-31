#ifndef ZPU_HAL_EVENTS_H
#define ZPU_HAL_EVENTS_H

#include <stdint.h>

#define DEF_EVENTFLAGS (~EVENT_NONE)

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
    EVENT_MOUSEMOV  = 1 << 3
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
    } data;
};

enum {
    EVENTQUEUE_WHENFULL_IGNORE,
    EVENTQUEUE_WHENFULL_RMOLD
};

enum eventpollbehaviour {
    EQ_POLL_BLOCKING,
    EQ_POLL_NONBLOCKING
};

struct eventqueue {
    eventflags  eventflags;
    int         fullbehaviour;
    int         maxsz;
    int         front, back;
};


enum eventsyserr evsys_initeventsys(eventflags eflags);
int evsys_poll(struct event *e, enum eventpollbehaviour pollbehaviour);
void evsys_wait(void);
int evsys_hasevents(void);
int evsys_get(struct event *e);
int evsys_peek(struct event *e);


#endif /* ZPU_HAL_EVENTS_H */
