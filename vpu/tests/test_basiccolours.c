#include "vpu/config.h"
#include "machine.h"

#if VPU_BUILDTESTS == 1 && VPU_BUILDTEST_BASICCOLOURS == 1

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

#include "hal/hal.h"
#include "vpu/video.h"
#include "vpu/text.h"
#include "config_master.h"

int main(int argc, char **argv)
{
    (void)argc; /* UNUSED */
    (void)argv; /* UNUSED */

    struct machine *mctx;
    VideoSys *vsys;
    struct event e;

    struct machine_config cfg = {
       SCREEN_PIXELS_X, SCREEN_PIXELS_Y, 0, NULL
    };

    int i, r, c;

    mctx = machine_poweron(&cfg);
    vsys = mctx->vsys;

    vpu_settextfg(vsys, vpu_rgbto32(vsys, 0xa0, 0xa0, 0));
    vpu_puts(vsys, DEFAULT_PROGNAME);
    vpu_puts(vsys, "\n");
    vpu_settextfg(vsys, vpu_rgbto32(vsys, 0xa0, 0, 0));
    vpu_puts(vsys, "Video subsystem   : Running\n");
    vpu_puts(vsys, "VPU Backend       : ");
    vpu_puts(vsys, vpu_backendinfostr(vsys));
    vpu_puts(vsys, "\n---------------------------------------------\n");

    vpu_settextlayerflags(vsys,
                          vpu_textlayerflags(vsys) & ~VPU_TXTAUTOSCROLL);

    for (i = 0; i < 5000; i++) {
        vpu_settextattr(vsys, VPU_TXTATTRIB_REVERSE);
        vpu_curssetpos(vsys, 0, 4);
        for (r = 4; r < vsys->disp.txt.rows; r++) {
            for (c = 0; c < vsys->disp.txt.cols; c++) {
                vpu_settextfg(vsys,
                              vpu_rgbto32(vsys, r + c, rand() % 32, r + c));
                vpu_putchar(vsys, ' ');
            }
            vpu_puts(vsys, "\n");
        }

        vpu_refresh(vsys, VPU_REFRESH_COMMITONLY);

        vpu_settextattr(vsys, VPU_TXTATTRIB_TRANSPARENT);
        vpu_curssetpos(vsys, 30, (i/100) % (vsys->disp.txt.rows - 5) + 5);
        vpu_settextfg(vsys, vpu_rgbto32(vsys, 0xFF, 0xFF, 0xFF));

        vpu_puts(vsys, "This is a test");

        vpu_refresh(vsys, VPU_REFRESH_NORMAL);

        if (evsys_poll(mctx->esys, &e, EQ_POLL_NONBLOCKING))
            if (e.type & EVENT_QUIT)
                break;
    }

    vpu_puts(vsys, "\n\nFinished.");
    vpu_refresh(vsys, VPU_REFRESH_FORCE);

    while(1) {
        if (evsys_poll(mctx->esys, &e, EQ_POLL_BLOCKING))
            if (e.type & EVENT_QUIT)
                break;
    }

    machine_poweroff(mctx);
    return 0;
}
#endif
