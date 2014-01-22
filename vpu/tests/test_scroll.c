#include "vpu/backend/config.h"

#if VPU_BUILDTESTS == 1 && VPU_BUILDTEST_SCROLL == 1

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

#include "vpu/backend/base.h"
#include "vpu/backend/text.h"
#include "vpu/backend/config.h"

int main(int argc, char **argv)
{
    (void)argc; /* UNUSED */
    (void)argv; /* UNUSED */

    size_t i;

    if (vpu_init(SCREEN_PIXELS_X, SCREEN_PIXELS_Y, 0, NULL)
            != VPU_ERR_NONE) {
        fputs("Could not init VPU\n", stderr);
        exit(1);
    }
    char str[30];
    uint32_t fgcolour;
    uint32_t altcolour;


    fputs("Video subsystem running\n", stdout);

    fgcolour    = vpu_rgbto32(0xa0, 0, 0);
    altcolour   = vpu_rgbto32(0x00, 0x00, 0xff);

    /* Ensure text auto scroll flag is set */
    vpu_settextlayerflags( vpu_textlayerflags() | VPU_TXTAUTOSCROLL);

    vpu_puts("Video subsystem   : Running\n");
    vpu_settextfg(vpu_rgbto32(0xa0, 0xa0, 0));
    vpu_puts("VPU Backend       : ");
    vpu_puts(vpu_backendinfostr());
    vpu_puts_c("\n---------------------------------------------\n", fgcolour);

    vpu_settextattr(VPU_TXTATTRIB_REVERSE);

    for (i = 0; i < 50000; i++) {
        fgcolour = i & 1 ? VTXT_DEF_FGCOLOUR : altcolour;

        //vpu_curshome();
        sprintf(str, "Counter: %zu\n", i+1);
        vpu_puts_c(str, fgcolour);
        vpu_refresh(VPU_REFRESH_NORMAL);
        //sleep(1);
    }

    vpu_refresh(VPU_REFRESH_FORCE);
    sleep(2);

    fputs("Exiting\n", stdout);

    return 0;
}

#endif
