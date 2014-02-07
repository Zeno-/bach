#include "machine.h"

#include <stdlib.h>
#include <stdio.h>

struct machine *
machine_new(void)
{
    struct machine *M;

    if ((M = calloc(1, sizeof *M)) == NULL)
        return NULL;        // FIXME: report error
    return M;
}

void
machine_dispose(struct machine **M)
{
    free(*M);
    *M = NULL;
}

void
machine_destroy(struct machine **M)
{
    machine_poweroff(*M);
    machine_dispose(M);
}

int
machine_poweron(struct machine *M, const struct machine_config *cfg)
{
    enum eventsyserr e_err;
    enum vpuerror v_err;

    /* FIXME: Instead of reporting an error, is it better to cleanup the
     * current machine and start anew?
     */
    if (M->esys || M->vsys) {
                    /* FIXME: report error */
        return 0;   /* Machine already on */
    }

    if (hal_init() != HAL_NOERROR) {
        fputs("Could not init HAL. Aborting.\n", stderr);
        machine_destroy(&M);
        exit(1);
    }

    if ((M->esys = evsys_initeventsys(DEF_EVENTFLAGS, &e_err)) == NULL) {
        fputs("Could not init event subsystem. Aborting.\n", stderr);
        machine_destroy(&M);
        exit(1);
    }

    if ((M->vsys = vpu_init(cfg->video_pix_w,
                 cfg->video_pix_h,
                 cfg->fullscreen,
                 cfg->font,
                 &v_err)) == NULL) {
        machine_destroy(&M);
        fputs("Could not init VPU. Aborting.\n", stderr);
        exit(1);
    }

    return 1;
}

void
machine_poweroff(struct machine *M)
{
    if (!M)
        return;

    if (M->esys) {
        evsys_stopeventsys(M->esys);
        M->esys = NULL;
    }
    if (M->vsys) {
        vpu_cleanup(M->vsys);
        M->vsys = NULL;
    }
}
