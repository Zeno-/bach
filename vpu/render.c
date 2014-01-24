#include "render.h"

#include <stdint.h>

#include "config.h"
#include "vpu/video.h"
#include "vpu/fonts/bmfonts.h"
#include "text.h"

static void blitglyph(int ch, uint32_t *dest,
                      uint32_t fgcolour, uint32_t bgcolour, uint32_t attr);

void vpu_refresh_tlayer(void)
{
    uint32_t *dest;
    const uint8_t  *currchval;
    const uint32_t *fgcp, *bgcp, *atrp;
    unsigned r, c, xdelta, ydelta;
    unsigned nrows, ncols;

    if (!(vpurefs->txtlayer->flags & VPU_TXTLAYERVISIBLE))
        return;

    vpu_direct_write_start();

    dest = vpurefs->pixelmem + vpurefs->txtlayer->origin;

    xdelta = VPU_FIXED_FONT_WIDTH;
    ydelta = vpurefs->instance->w * (vpurefs->instance->fixedfont->height - 1);

    nrows = vpurefs->txtlayer->rows;
    ncols = vpurefs->txtlayer->cols;

    currchval = vpurefs->txt_charmem;
    fgcp = TXTCOLORPOS(0,0);
    bgcp = TXTBGCOLORPOS(0,0);
    atrp = TXTATTRPOS(0,0);

    for (r = 0; r < nrows; r++) {
        for (c = 0; c < ncols; c++) {
            blitglyph(*currchval++, dest, *fgcp++, *bgcp++, *atrp++);
            dest += xdelta;
        }
        dest += ydelta;
    }

    vpu_direct_write_end();
}

static void blitglyph(int ch, uint32_t *dest,
                      uint32_t fgcolour, uint32_t bgcolour, uint32_t attr)
{
    unsigned fontheight, mask;
    const unsigned char *glyph;
    unsigned char gpx;
    unsigned i;
    uint32_t *destrow;
    unsigned destdelta;

    glyph = vpurefs->instance->fixedfont->pixeldata
            + ch * vpurefs->instance->fixedfont->height;

    fontheight = vpurefs->instance->fixedfont->height;

    destdelta = vpurefs->txtlayer->cols * VPU_FIXED_FONT_WIDTH;
    destrow = dest;

    for (i = 0; i < fontheight; i++) {
        gpx = *glyph;
        dest = destrow;
        if (attr & VPU_TXTATTRIB_REVERSE)
            gpx = ~gpx;
        for (mask = 1<<(VPU_FIXED_FONT_WIDTH-1); mask != 0; mask >>= 1) {
            if (gpx & mask)
                *dest = fgcolour;
            else if (!(attr & VPU_TXTATTRIB_TRANSPARENT))
                *dest = bgcolour;
            dest++;
        }
        destrow += destdelta;
        glyph++;
    }
}
