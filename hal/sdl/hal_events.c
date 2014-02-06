#include "hal_events.h"

#include <SDL/SDL.h>
#include <stdlib.h>

#include "common/macros.h"
#include "common/warn.h"

struct eventsys {
    struct eventqueue equeue;
    struct event *events;
};

#define MAXQSZ  1024

static enum eventsyserr initqueue(EventSys *esys, eventflags eventflags);
inline static void cleanup(EventSys *esys);

inline static int   eq_peek(EventSys *esys, struct event *e);
inline static void  eq_add(EventSys *esys, const struct event *e);
inline static void  eq_rem(EventSys *esys);
inline static int   eq_isfull(const EventSys *esys);
inline static int   eq_isempty(const EventSys *esys);
static int          eq_processbackend(EventSys *esys);

/**************************************************************************
 * Public
 *************************************************************************/

EventSys *
evsys_initeventsys(eventflags eventflags, enum eventsyserr *err)
{
    enum eventsyserr err_temp;
    EventSys *esys;

    if ((esys = calloc(1, sizeof *esys)) == NULL) {
        if (err)
            *err = ESYSERR_MALLOC;
        return NULL;
    }

    if ((err_temp = initqueue(esys, eventflags)) != ESYSERR_NONE) {
        if (err)
            *err = err_temp;
        return NULL;
    }

    return esys;
}

void evsys_stopeventsys(EventSys *esys)
{
    if (esys)
        cleanup(esys);
}

int evsys_poll(EventSys *esys, struct event *e,
               enum eventpollbehaviour pollbehaviour)
{
    if (evsys_hasevents(esys))
        return evsys_getevent(esys, e);

    if (pollbehaviour == EQ_POLL_NONBLOCKING && !SDL_PollEvent(NULL))
        return 0;
    else if (pollbehaviour == EQ_POLL_BLOCKING)
        evsys_wait(esys);

    if (eq_processbackend(esys))
        return evsys_getevent(esys, e);

    return 0;
}

void evsys_wait(EventSys *esys)
{
    (void)esys;   /* UNUSED */
    SDL_WaitEvent(NULL);
}

void evsys_waitforquit(EventSys *esys)
{
    struct event e;

    while (1) {
        if (evsys_poll(esys, &e, EQ_POLL_BLOCKING))
            if (e.type & EVENT_QUIT)
                break;
    }

    eq_add(esys, &e);    /* push back onto the queue */
}

int evsys_hasevents(EventSys *esys)
{
    return !eq_isempty(esys);
}

int evsys_getevent(EventSys *esys, struct event *e)
{
    int r;

    if ((r = eq_peek(esys, e)))
        eq_rem(esys);
    return r;
}

int evsys_peekevent(EventSys *esys, struct event *e)
{
    return eq_peek(esys, e);
}

/**************************************************************************
 * Private
 *************************************************************************/

static enum eventsyserr
initqueue(EventSys *esys, eventflags eventflags)
{
    if ((esys->events = malloc(MAXQSZ * sizeof *esys->events)) == NULL)
        return ESYSERR_MALLOC;

    esys->equeue.eventflags   = eventflags;
    esys->equeue.maxsz        = MAXQSZ;
    esys->equeue.back         = 0;
    esys->equeue.front        = 0;

    esys->equeue.fullbehaviour = EVENTQUEUE_WHENFULL_RMOLD;

    return ESYSERR_NONE;
}

inline static void
cleanup(EventSys *esys)
{
    if (esys->events) {
        free(esys->events);
    }
    free(esys);
}

inline static int
eq_peek(EventSys *esys, struct event *e)
{
    int headpos;

    if (eq_isempty(esys)) {
        WARN("Attempt to get/peek empty event queue.");
        return 0;
    }

    /* The 'head' element is one behind the current front
     * offset/index.
     */
    if (esys->equeue.front == 0)
        headpos = esys->equeue.maxsz - 1;
    else
        headpos = esys->equeue.front - 1;

    memcpy(e, &esys->events[headpos], sizeof *e);

    return 1;
}

inline static void
eq_add(EventSys *esys, const struct event *e)
{
    if (eq_isfull(esys)) {
        if (esys->equeue.fullbehaviour == EVENTQUEUE_WHENFULL_RMOLD)
            eq_rem(esys);
        else
            return;     /* Ignore new event */
    }
    memcpy(&esys->events[esys->equeue.front], e, sizeof *e);
    esys->equeue.front = (esys->equeue.front + 1) % esys->equeue.maxsz;
}

inline static void
eq_rem(EventSys *esys)
{
    if (eq_isempty(esys)) {
        WARN("Attempt to remove from empty event queue.");
        return;
    }

    esys->equeue.back = (esys->equeue.back + 1) % esys->equeue.maxsz;
}

inline static int
eq_isfull(const EventSys *esys)
{
    return (esys->equeue.front + 1) % esys->equeue.maxsz
            == esys->equeue.back;
}

inline static int
eq_isempty(const EventSys *esys)
{
    return esys->equeue.front == esys->equeue.back;
}

static int
eq_processbackend(EventSys *esys)
{
    eventflags flagmask = esys->equeue.eventflags;
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
                eq_add(esys, &e);
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
