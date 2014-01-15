#include "vpu/backend/config.h"

#ifdef VPU_BUILD_BACKENDTEST1

#include <stdlib.h>
#include <stdio.h>

#if HAVE_UNISTD_H
#   include <unistd.h>
#endif

#include "vpu/backend/base.h"
#include "vpu/backend/text.h"
#include "vpu/backend/config.h"

int main(int argc, char **argv)
{
#if !defined _WIN32
    (void)argc; /* UNUSED */
    (void)argv; /* UNUSED */
#endif

    if (vpu_init(SCREEN_PIXELS_X, SCREEN_PIXELS_Y, 0, NULL)
            != VPU_ERR_NONE) {
        fputs("Could not init VPU\n", stderr);
        exit(1);
    }
#if 1
    struct display *scr;
    int i, r, c;

    scr = vpu_getinstance();

    vpu_settextfg(vpu_rgbto32(0xa0, 0xa0, 0));
    vpu_puts(DEFAULT_PROGNAME);
    vpu_puts("\n");
    vpu_settextfg(vpu_rgbto32(0xa0, 0, 0));
    vpu_puts("Video subsystem   : Running\n");
    vpu_puts("VPU Backend       : ");
    vpu_puts(vpu_backendinfostr());
    vpu_puts("\n---------------------------------------------\n");


    vpu_settextlayerflags( vpu_textlayerflags() & ~VPU_TXTAUTOSCROLL);


    for (i = 0; i < 10000; i++) {
        vpu_settextattr(VPU_TXTATTRIB_REVERSE);
        vpu_curssetpos(0, 4);
        for (r = 4; r < scr->txt.rows; r++) {
            for (c = 0; c < scr->txt.cols; c++) {
                vpu_settextfg(vpu_rgbto32(r+c, rand()%32, r+c));
                vpu_putchar(' ');
            }
            vpu_puts("\n");
        }

        vpu_refresh(VPU_REFRESH_COMMITONLY);

        vpu_settextattr(VPU_TXTATTRIB_TRANSPARENT);
        vpu_curssetpos(30, (i/100) % (scr->txt.rows - 5) + 5);
        vpu_settextfg(vpu_rgbto32(0xFF, 0xFF, 0xFF));
        vpu_puts("This is a test");

        vpu_refresh(VPU_REFRESH_NORMAL);
    }

    vpu_refresh(VPU_REFRESH_FORCE);
    //sleep(2);
#else
    char str[30];
    uint32_t fgcolour;
    uint32_t altcolour;


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

    vpu_settextattr(VPU_TXTATTRIB_REVERSE);

    for (i = 0; i < 10000; i++) {
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
#endif

    return 0;
}
#endif
