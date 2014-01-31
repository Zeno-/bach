#include "vpu/config.h"

#if VPU_BUILDTESTS == 1 && VPU_BUILDTEST_SCROLL == 1

#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

#include "vpu/video.h"
#include "vpu/text.h"
#include "vpu/config.h"
#include "test_common.h"
#include "hal/hal.h"

#define ITERATIONS 500
#define DOAUTOSCROLL 1

int main(int argc, char **argv)
{
    (void)argc; /* UNUSED */
    (void)argv; /* UNUSED */

    size_t i;
    char str[30];
    uint32_t fgcolour;
    uint32_t altcolour;

    vput_test_initall();

    fputs("Video subsystem running\n", stdout);

    /* Set/clear scroll flag is set */
#if DOAUTOSCROLL == 1
    vpu_settextlayerflags( vpu_textlayerflags() | VPU_TXTAUTOSCROLL);
#else
    vpu_settextlayerflags( vpu_textlayerflags() &  ~VPU_TXTAUTOSCROLL);
#endif

    vpu_settextattr(VPU_TXTATTRIB_REVERSE);     /* Reverse text */

    /* Colours for the test text */
    fgcolour    = vpu_rgbto32(0xa0, 0, 0);
    altcolour   = vpu_rgbto32(0x00, 0x00, 0xff);

    vpu_puts("Video subsystem   : Running\n");

    vpu_settextfg(vpu_rgbto32(0xa0, 0xa0, 0));
    vpu_puts("VPU Backend       : ");
    vpu_puts(vpu_backendinfostr());
    vpu_puts_c("\n---------------------------------------------\n",
               fgcolour);

    for (i = 0; i < ITERATIONS; i++) {
        /* Odd and even lines different colour */
        fgcolour = i & 1 ? VTXT_DEF_FGCOLOUR : altcolour;

        sprintf(str, "Counter: %zu\n", i+1);
        vpu_puts_c(str, fgcolour);
        vpu_refresh(VPU_REFRESH_NORMAL);
    }

    vpu_refresh(VPU_REFRESH_FORCE);

    sleep(2);


    struct event e;
    while(1) {
        if (evsys_poll(&e, EQ_POLL_BLOCKING))
            if (e.type & EVENT_QUIT)
                break;
    }

    fputs("Exiting\n", stdout);

    return 0;
}

#endif
