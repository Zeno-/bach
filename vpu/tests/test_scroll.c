#include "vpu/config.h"

#if VPU_BUILDTESTS == 1 && VPU_BUILDTEST_SCROLL == 1

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

#include "vpu/video.h"
#include "vpu/text.h"
#include "vpu/config.h"
#include "test_common.h"
#include "machine.h"

#define ITERATIONS 500
#define DOAUTOSCROLL 1

int main(int argc, char **argv)
{
    (void)argc; /* UNUSED */
    (void)argv; /* UNUSED */

    struct machine *mctx;
    VideoSys *vsys;

    size_t i;
    char str[30];
    uint32_t fgcolour;
    uint32_t altcolour;

    mctx = vput_test_initall();
    vsys = mctx->vsys;

    /* Set/clear scroll flag is set */
#if DOAUTOSCROLL == 1
    vpu_settextlayerflags(vsys,
                          vpu_textlayerflags(vsys) | VPU_TXTAUTOSCROLL);
#else
    vpu_settextlayerflags(vsys,
                          vpu_textlayerflags(vsys) &  ~VPU_TXTAUTOSCROLL);
#endif

    vpu_settextattr(vsys, VPU_TXTATTRIB_REVERSE);     /* Reverse text */

    /* Colours for the test text */
    fgcolour    = vpu_rgbto32(vsys, 0xa0, 0, 0);
    altcolour   = vpu_rgbto32(vsys, 0x00, 0x00, 0xff);

    vpu_puts(vsys, "Video subsystem   : Running\n");

    vpu_settextfg(vsys, vpu_rgbto32(vsys, 0xa0, 0xa0, 0));
    vpu_puts(vsys, "VPU Backend       : ");
    vpu_puts(vsys, vpu_backendinfostr(vsys));
    vpu_puts_c(vsys, "\n---------------------------------------------\n",
               fgcolour);

    for (i = 0; i < ITERATIONS; i++) {
        /* Odd and even lines different colour */
        fgcolour = i & 1 ? VTXT_DEF_FGCOLOUR(vsys) : altcolour;

        sprintf(str, "Counter: %zu\n", i+1);
        vpu_puts_c(vsys, str, fgcolour);
        vpu_refresh(vsys, VPU_REFRESH_NORMAL);
    }

    vpu_refresh(vsys, VPU_REFRESH_FORCE);

#if TEST_WAITFORQUIT
    struct event e;
    while(1) {
        if (evsys_poll(mctx->esys, &e, EQ_POLL_BLOCKING))
            if (e.type & EVENT_QUIT)
                break;
    }
#else
    sleep(2);
#endif

    fputs("Exiting\n", stdout);

    machine_poweroff(mctx);
    return 0;
}

#endif
