#ifndef HAL_MAINHEADER_H
#define HAL_MAINHEADER_H

enum {
    HAL_BACKEND_SDL
};

#define HAL_INTERFACE HAL_BACKEND_SDL

/* FIXME: Each backend interface should be in its own subdirectory
 *        E.g. sdl/init_hal.h
 */

#if HAL_INTERFACE == HAL_BACKEND_SDL
#   include "init_hal.h"
#   include "kybdmouse_hal.h"
#endif

#endif /* HAL_MAINHEADER_H */
