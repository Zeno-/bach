#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "vpu/backend/base.h"
#include "vpu/backend/text.h"
#include "vpu/backend/config.h"

int main(void)
{
    char str[30];
    uint32_t fgcolour;
    uint32_t altcolour;

    if (vpu_init(SCREEN_PIXELS_X, SCREEN_PIXELS_Y, 0) != VPU_ERR_NONE) {
        fputs("Could not init VPU\n", stderr);
        exit(1);
    }

    fputs("Video subsystem running\n", stdout);

    fgcolour    = vpu_rgbto32(0xa0, 0, 0);
    altcolour   = vpu_rgbto32(0x00, 0x00, 0xff);
    vpu_settextlayerflags( vpu_textlayerflags() & ~VPU_TXTAUTOSCROLL);

    size_t i;
    vpu_puts("Video subsystem   : Running\n");
    vpu_settextfg(vpu_rgbto32(0xa0, 0xa0, 0));
    vpu_puts("VPU Backend       : ");
    vpu_puts(vpu_backendinfostr());
    vpu_puts_c("\n---------------------------------------------\n", fgcolour);
    for (i = 0; i < 100000; i++) {
        fgcolour = i & 1 ? DEFAULT_TEXTFG : altcolour;

        //vpu_curshome();
        sprintf(str, "Counter: %zu\n", i+1);
        vpu_puts_c(str, fgcolour);
        vpu_refresh(VPU_FORCEREFRESH_FALSE);
        //sleep(1);
    }

    vpu_refresh(VPU_FORCEREFRESH_TRUE);
    sleep(2);

    fputs("Exiting\n", stdout);

    return 0;
}
