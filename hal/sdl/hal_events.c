#include "hal_events.h"

#include <SDL/SDL.h>
#include <stdlib.h>

#include "common/macros.h"
#include "common/warn.h"

static const int maxqsize   = 1024;
static const int isinit     = 0;

static struct eventqueue equeue;
static struct event *events;

static enum eventsyserr initqueue(eventflags eventflags);
static void cleanup(void);

inline static void eq_add(const struct event *e);
inline static void eq_rem(void);
inline static int  eq_isfull(void);
inline static int  eq_isempty(void);
static int         eq_processbackend(void);

/**************************************************************************
 * Public
 *************************************************************************/

enum eventsyserr
evsys_initeventsys(eventflags eventflags)
{
    enum eventsyserr err;

    if (isinit) {
        WARN("Event system already initialised. Ignoring.");
        return ESYSERR_NONE;
    }

    if ((err = initqueue(eventflags)) != ESYSERR_NONE)
        return err;

    atexit(cleanup);

    return ESYSERR_NONE;
}

int evsys_poll(struct event *e, enum eventpollbehaviour pollbehaviour)
{
    if (evsys_hasevents())
        return evsys_get(e);

    if (pollbehaviour == EQ_POLL_NONBLOCKING && !SDL_PollEvent(NULL))
        return 0;
    else if (pollbehaviour == EQ_POLL_BLOCKING)
        evsys_wait();

    if (eq_processbackend())
        return evsys_get(e);

    return 0;
}

void evsys_wait(void)
{
    SDL_WaitEvent(NULL);
}

int evsys_hasevents(void)
{
    return !eq_isempty();
}

int evsys_get(struct event *e)
{
    int headpos;

    if (eq_isempty()) {
        WARN("Attempt to get event from empty queue.");
        return 0;
    }

    if (equeue.front == 0)
        headpos = equeue.maxsz - 1;
    else
        headpos = equeue.front - 1;

    memcpy(e, &events[headpos], sizeof *e);
    eq_rem();

    return 1;
}

int evsys_peek(struct event *e)
{
    if (eq_isempty()) {
        WARN("Attempt to peek empty event queue.");
        return 0;
    }
    memcpy(e, &events[equeue.front], sizeof *e);
    return 1;
}


/**************************************************************************
 * Private
 *************************************************************************/

static enum eventsyserr
initqueue(eventflags eventflags)
{
    if ((events = malloc(maxqsize * sizeof *events)) == NULL)
        return ESYSERR_MALLOC;

    equeue.eventflags   = eventflags;
    equeue.maxsz        = maxqsize;
    equeue.back         = 0;
    equeue.front        = 0;

    equeue.fullbehaviour = EVENTQUEUE_WHENFULL_RMOLD;

    return ESYSERR_NONE;
}

inline static void
eq_add(const struct event *e)
{
    if (eq_isfull()) {
        if (equeue.fullbehaviour == EVENTQUEUE_WHENFULL_RMOLD)
            eq_rem();
        else
            return;     /* Ignore new event */
    }
    memcpy(&events[equeue.front], e, sizeof *e);
    equeue.front = (equeue.front + 1) % equeue.maxsz;
}

inline static void
eq_rem(void)
{
    if (equeue.front == equeue.back) {
        WARN("Attempt to remove from empty event queue.");
        return;
    }

    equeue.back = (equeue.back + 1) % equeue.maxsz;
}

inline static int
eq_isfull(void)
{
    return (equeue.front + 1) % equeue.maxsz == equeue.back;
}

inline static int
eq_isempty(void)
{
    return equeue.front == equeue.back;
}

static void
cleanup(void)
{
    if (events) {
        free(events);
    }
}


static int
eq_processbackend(void)
{
    eventflags flagmask = equeue.eventflags;
    eventflags eventtype = 0;
    struct event e;
    SDL_Event sdlevent;

    memset(&e, 0, sizeof e);

    while (SDL_PollEvent(&sdlevent)) {
        switch (sdlevent.type) {
        case SDL_QUIT:
            if (flagmask & EVENT_QUIT) {
                eventtype |= EVENT_QUIT;
                e.type = EVENT_QUIT;
                eq_add(&e);
            }
            break;
        case SDL_KEYDOWN:
            break;
        case SDL_KEYUP:
            break;
        case SDL_MOUSEMOTION:
            break;
        default:
            break;
        }
    }
    return eventtype != EVENT_NONE;
}

