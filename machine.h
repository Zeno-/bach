#ifndef ZPU_MACHINE_H
#define ZPU_MACHINE_H

#include "hal/hal.h"
#include "vpu/video.h"

struct machine {
    EventSys *esys;
    VideoSys *vsys;
};

struct machine_config {
    unsigned video_pix_w;
    unsigned video_pix_h;
    int fullscreen;
    const struct vidfont8 *font;
};

struct machine *machine_poweron(const struct machine_config *cfg);
void machine_poweroff(struct machine *mctx);

#endif
