#ifndef HAL_MAINHEADER_H
#define HAL_MAINHEADER_H

#include "config_master.h"

#if HAL_INTERFACE == HAL_BACKEND_SDL
#   include "sdl/hal_init.h"
#   include "sdl/hal_kybdmouse.h"
#   include "sdl/hal_video_refs.h"
#   include "sdl/hal_timer.h"
#   include "sdl/hal_events.h"
#endif

#endif /* HAL_MAINHEADER_H */
