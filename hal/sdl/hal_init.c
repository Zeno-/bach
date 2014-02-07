#include "hal_init.h"

#include <SDL/SDL.h>
#include <stdlib.h>
#include "config_master.h"

static int isinitialised = 0;       // FIXME (global)

enum hal_error
hal_init(void)
{
    if (isinitialised)
        return HAL_NOERROR; /* Silently ignore */

    /* For SDL_Init(), The Event Handling, File I/O, and Threading
     * subsystems are initialized by default.
     */
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        return HAL_INIT_FAIL;

    isinitialised = 1;

#ifdef DEFAULT_PROG_TITLE
    SDL_WM_SetCaption(DEFAULT_PROG_TITLE, NULL);
#endif

    atexit(SDL_Quit);

    return HAL_NOERROR;
}

int hal_isinitalised(void)
{
    return isinitialised;
}

