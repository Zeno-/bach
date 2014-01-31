#include "machine.h"

#include <stdlib.h>
#include <stdio.h>

inline static void cleanup(struct machine *mctx);

struct machine *
machine_poweron(const struct machine_config *cfg)
{
    enum eventsyserr e_err;
    enum vpuerror v_err;

    struct machine *m;

    if ((m = calloc(1, sizeof *m)) == NULL)
        return NULL;        // FIXME: report error

    if (hal_init() != HAL_NOERROR) {
        fputs("Could not init HAL. Aborting.\n", stderr);
        cleanup(m);
        exit(1);
    }

    if ((m->esys = evsys_initeventsys(DEF_EVENTFLAGS, &e_err)) == NULL) {
        fputs("Could not init event subsystem. Aborting.\n", stderr);
        cleanup(m);
        exit(1);
    }

    if ((m->vsys = vpu_init(cfg->video_pix_w,
                 cfg->video_pix_h,
                 cfg->fullscreen,
                 cfg->font,
                 &v_err)) == NULL) {
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
    if (mctx->esys) {
        evsys_stopeventsys(mctx->esys);
        mctx->esys = NULL;
    }
    if (mctx->vsys) {
        vpu_cleanup(mctx->vsys);
        mctx->vsys = NULL;
    }

    free(mctx);
}
