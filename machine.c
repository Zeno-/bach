#include "machine.h"

#include <stdlib.h>
#include <stdio.h>

inline static void cleanup(struct machine **M);

struct machine *
machine_new(void)
{
    struct machine *M;

    if ((M = calloc(1, sizeof *M)) == NULL)
        return NULL;        // FIXME: report error
    return M;
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
        cleanup(&M);
        exit(1);
    }

    if ((M->esys = evsys_initeventsys(DEF_EVENTFLAGS, &e_err)) == NULL) {
        fputs("Could not init event subsystem. Aborting.\n", stderr);
        cleanup(&M);
        exit(1);
    }

    if ((M->vsys = vpu_init(cfg->video_pix_w,
                 cfg->video_pix_h,
                 cfg->fullscreen,
                 cfg->font,
                 &v_err)) == NULL) {
        cleanup(&M);
        fputs("Could not init VPU. Aborting.\n", stderr);
        exit(1);
    }

    return 1;
}

void
machine_poweroff(struct machine **M)
{
    cleanup(M);
}


inline static void
cleanup(struct machine **M)
{
    if (!*M)
        return;

    if ((*M)->esys) {
        evsys_stopeventsys((*M)->esys);
        (*M)->esys = NULL;
    }
    if ((*M)->vsys) {
        vpu_cleanup((*M)->vsys);
        (*M)->vsys = NULL;
    }

    free(*M);
    *M = NULL;
}
