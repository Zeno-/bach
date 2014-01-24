#ifndef HAL_MAINHEADER_H
#define HAL_MAINHEADER_H

#include "config_master.h"

#if HAL_INTERFACE == HAL_BACKEND_SDL
#   include "sdl/init_hal.h"
#   include "sdl/kybdmouse_hal.h"
#   include "sdl/video_hal_refs.h"
#endif

#endif /* HAL_MAINHEADER_H */
