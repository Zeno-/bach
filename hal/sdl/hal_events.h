#ifndef BACH_HAL_EVENTS_H
#define BACH_HAL_EVENTS_H

#include <stdint.h>

#define DEF_EVENTFLAGS (~EVENT_NONE)

typedef struct eventsys EventSys;

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

EventSys *evsys_initeventsys(eventflags eflags,
                             enum eventsyserr *err);
void evsys_stopeventsys(EventSys *esys);
int evsys_poll(EventSys *esys, struct event *e,
               enum eventpollbehaviour pollbehaviour);
void evsys_wait(EventSys *esys);
void evsys_waitforquit(EventSys *esys);
int evsys_hasevents(EventSys *esys);
int evsys_getevent(EventSys *esys, struct event *e);
int evsys_peekevent(EventSys *esys, struct event *e);


#endif /* BACH_HAL_EVENTS_H */
