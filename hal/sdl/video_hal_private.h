#ifndef VID_BACKENDPRIV_H
#define VID_BACKENDPRIV_H

#include "hal_video_refs.h"

struct display_privdata {
    SDL_Surface *vidsurface;
    uint32_t *vpixels;
};

#endif /* VID_BACKENDPRIV_H */
