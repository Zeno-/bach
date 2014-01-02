#include "render.h"

#include <stdint.h>

#include "base.h"
#include "private.h"
#include "config.h"
#include "vpu/fonts/bmfonts.h"
#include "text.h"

static void blitglyph(int ch, uint32_t *dest,
                      uint32_t fgcolour, uint32_t bgcolour);

void vpu_refresh_tlayer(void)
{
    uint32_t fgcolour, bgcolour;
    uint32_t *dest;
    uint8_t  *currchval, *prevchval;
    unsigned r, c, xdelta, ydelta;
    unsigned nrows, ncols;

    VPU_DIRECTPXWRITE_START();

    dest = VPU_PRV_PIXELS + VPU_TL.origin;

    xdelta = VPU_FIXED_FONT_WIDTH;
    ydelta = VPU_PRV_INSTANCE.w * (VPU_PRV_INSTANCE.fixedfont->height - 1);

    nrows = VPU_TL.rows;
    ncols = VPU_TL.cols;

    currchval = VPU_TL_MEM;

    fgcolour = VPU_TL.fgcolour;
    bgcolour = VPU_TL.bgcolour;

    for (r = 0; r < nrows; r++) {
        for (c = 0; c < ncols; c++) {
            blitglyph(*currchval, dest, fgcolour, bgcolour);
            dest += xdelta;
            currchval++;
            prevchval++;
        }
        dest += ydelta;
    }

    VPU_DIRECTPXWRITE_END();
}

static void blitglyph(int ch, uint32_t *dest,
                      uint32_t fgcolour, uint32_t bgcolour)
{
    unsigned fontheight, mask;
    const unsigned char *glyph, *src;
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
            if (*src & mask) {
                *destrow = fgcolour;
            } else {
                *destrow = bgcolour;
            }
            destrow += destdelta;
            src++;
        }
        dest++;
    }
}
