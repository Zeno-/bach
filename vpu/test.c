#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "vpu/backend/base.h"
#include "vpu/backend/text.h"
#include "vpu/backend/config.h"

int main(void)
{
    char str[30];

    if (vpu_init(SCREEN_PIXELS_X, SCREEN_PIXELS_Y, 0) != VPU_ERR_NONE) {
        fputs("Could not init VPU\n", stderr);
        exit(1);
    }

    fputs("Video subsystem running\n", stdout);

    vpu_settextlayerflags( vpu_textlayerflags() & ~VPU_TXTAUTOSCROLL);

    size_t i;
    vpu_puts("Video subsystem   : Running\n");
    vpu_puts("VPU Backend       : ");
    vpu_puts(vpu_backendinfostr());
    vpu_puts("\n---------------------------------------------\n");
    for (i = 0; i < 50000; i++) {
        vpu_curshome();
        sprintf(str, "Counter: %zu\n", i+1);
        vpu_puts(str);
        vpu_refresh(VPU_FORCEREFRESH_FALSE);
    }

    vpu_refresh(VPU_FORCEREFRESH_TRUE);
    sleep(2);

    fputs("Exiting\n", stdout);

    return 0;
}
