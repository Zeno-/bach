#include "hal_events.h"

#include <SDL/SDL.h>
#include <stdlib.h>

#include "common/macros.h"

static const int maxqsize   = 1024;
static const int isinit     = 0;

static struct eventqueue equeue;
static struct event *events;

static enum eventsyserr initqueue(eventflags eventflags);
static void cleanup(void);

inline static void ev_add(const struct event *e);
inline static void ev_rem(void);
inline static int  ev_isfull(void);

enum eventsyserr
evsys_initeventsys(eventflags eventflags)
{
    enum eventsyserr err;

    if (isinit) {
        fputs("Event system already initialised. Ignoring.\n", stderr);
        return ESYSERR_NONE;
    }

    if ((err = initqueue(eventflags)) != ESYSERR_NONE)
        return err;

    atexit(cleanup);

    return ESYSERR_NONE;
}

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
ev_add(const struct event *e)
{
    if (ev_isfull())
        ev_rem();
    //events[equeue.front] = FIXME: Copy event into queue
    equeue.front = (equeue.front + 1) % (equeue.maxsz);
}

inline static void
ev_rem(void)
{
    if (equeue.front == equeue.back) {
        printf("Empty queue\n");
        return;
    }

    equeue.back = (equeue.back + 1) % (equeue.maxsz);
}

inline static int
ev_isfull(void)
{
    return (equeue.front + 1) % (equeue.maxsz) == equeue.back;
}

static void
cleanup(void)
{
    if (events) {
        free(events);
    }
}

#if 0
eventmask pollevent(void)
{
    eventmask event = 0;
    SDL_Event sdlevent;

    while (SDL_PollEvent(&sdlevent)) {
        switch (sdlevent.type) {
        case SDL_QUIT:
            event |= EVENT_QUIT;
            break;
        case SDL_KEYDOWN:
            event |= EVENT_KEYDOWN;
            break;
        case SDL_KEYUP:
            event |= EVENT_KEYUP;
            break;
        default:
            break;
        }
    }
    return event;
}
#endif
