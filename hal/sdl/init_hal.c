#include "init_hal.h"

#include <SDL/SDL.h>
#include <stdlib.h>
#include "config_master.h"

static int isinitialised = 0;

enum hal_error
hal_init(void)
{
    /* For SDL_Init(), The Event Handling, File I/O, and Threading
     * subsystems are initialized by default.
     */
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        return HAL_INIT_FAIL;

    isinitialised = 1;

#ifdef DEFAULT_PROGNAME
    SDL_WM_SetCaption(DEFAULT_PROGNAME, NULL);
#endif

    atexit(SDL_Quit);

    return HAL_NOERROR;
}

int hal_isinitalised(void)
{
    return isinitialised;
}

