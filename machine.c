#include "machine.h"

#include <stdlib.h>
#include <stdio.h>
#include "vpu/video.h"

inline static void cleanup(struct machine *mctx);

struct machine *
machine_poweron(const struct machine_config *cfg)
{
    enum eventsyserr err;
    struct machine *m;

    if ((m = calloc(1, sizeof *m)) == NULL)
        return NULL;        // FIXME: report error

    if (hal_init() != HAL_NOERROR) {
        fputs("Could not init HAL. Aborting.\n", stderr);
        cleanup(m);
        exit(1);
    }

    if ((m->esys = evsys_initeventsys(DEF_EVENTFLAGS, &err)) == NULL) {
        fputs("Could not init event subsystem. Aborting.\n", stderr);
        cleanup(m);
        exit(1);
    }

    if (vpu_init(cfg->video_pix_w,
                 cfg->video_pix_h,
                 cfg->fullscreen,
                 cfg->font) != VPU_ERR_NONE) {
        cleanup(m);
        fputs("Could not init VPU. Aborting.\n", stderr);
        exit(1);
    }

    return m;
}

void
machine_poweroff(struct machine *mctx)
{
    cleanup(mctx);
}


inline static void
cleanup(struct machine *mctx)
{
    evsys_stopeventsys(mctx->esys);
    mctx->esys = NULL;
    free(mctx);
}
