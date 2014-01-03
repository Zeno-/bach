#include "render.h"

#include <stdint.h>

#include "base.h"
#include "private.h"
#include "config.h"
#include "vpu/fonts/bmfonts.h"
#include "text.h"

static void blitglyph(int ch, uint32_t *dest,
                      uint32_t fgcolour, uint32_t bgcolour, uint8_t attr);

void vpu_refresh_tlayer(void)
{
    uint32_t bgcolour;

    uint32_t *dest;
    const uint32_t *fgcp;
    uint8_t *atrp;

    uint8_t  *currchval;
    unsigned r, c, xdelta, ydelta;
    unsigned nrows, ncols;

    if (!(VPU_TL.flags & VPU_TXTLAYERVISIBLE))
        return;

    VPU_DIRECTPXWRITE_START();

    dest = VPU_PRV_PIXELS + VPU_TL.origin;

    xdelta = VPU_FIXED_FONT_WIDTH;
    ydelta = VPU_PRV_INSTANCE.w * (VPU_PRV_INSTANCE.fixedfont->height - 1);

    nrows = VPU_TL.rows;
    ncols = VPU_TL.cols;

    bgcolour = VPU_TL.bgcolour;

    currchval = VPU_TL_MEM;
    fgcp = TXTCOLORPOS(0,0);
    atrp = TXTATTRPOS(0,0);

    for (r = 0; r < nrows; r++) {
        for (c = 0; c < ncols; c++) {
            blitglyph(*currchval++, dest, *fgcp++, bgcolour, *atrp++);
            dest += xdelta;
        }
        dest += ydelta;
    }

    VPU_DIRECTPXWRITE_END();
}

static void blitglyph(int ch, uint32_t *dest,
                      uint32_t fgcolour, uint32_t bgcolour, uint8_t attr)
{
    unsigned fontheight, mask;
    const unsigned char *glyph, *src;
    unsigned char gpx;
    unsigned i;
    uint32_t *destrow;
    unsigned destdelta = VPU_PRV_INSTANCE.w;

    glyph = VPU_PRV_INSTANCE.fixedfont->pixeldata
            + ch * VPU_PRV_INSTANCE.fixedfont->height;

    fontheight = VPU_PRV_INSTANCE.fixedfont->height;

    for (mask = 1<<(VPU_FIXED_FONT_WIDTH-1); mask != 0; mask >>= 1) {
        destrow = dest;
        src = glyph;
        for (i = 0; i < fontheight; i++) {
            gpx = *src;
            if (attr & VPU_TXTATTRIB_REVERSE)
                gpx = ~gpx;
            if (gpx
                    & mask) {
                *destrow = fgcolour;
            } else if (!(VPU_TL.flags & VPU_TXTLAYEROVERLAY)) {
                *destrow = bgcolour;
            }
            destrow += destdelta;
            src++;
        }
        dest++;
    }
}
