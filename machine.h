#ifndef BACH_MACHINE_H
#define BACH_MACHINE_H

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

struct machine *machine_new(void);
void machine_dispose(struct machine **M);
void machine_destroy(struct machine **M);
int machine_poweron(struct machine *M, const struct machine_config *cfg);
void machine_poweroff(struct machine *M);

#endif /* BACH_MACHINE_H */
