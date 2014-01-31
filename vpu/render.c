#include "render.h"

#include <stdint.h>

#include "config.h"
#include "vpu/video.h"
#include "vpu/fonts/bmfonts.h"
#include "text.h"

static void blitglyph(VideoSys *vctx,
                      int ch, uint32_t *dest,
                      uint32_t fgcolour, uint32_t bgcolour, uint32_t attr);

void vpu_refresh_tlayer(VideoSys *vctx)
{
    uint32_t *dest;
    const uint8_t  *currchval;
    const uint32_t *fgcp, *bgcp, *atrp;
    unsigned r, c, xdelta, ydelta;
    unsigned nrows, ncols;

    if (!(vctx->refs.txtlayer->flags & VPU_TXTLAYERVISIBLE))
        return;

    vpu_direct_write_start(vctx);

    dest = vctx->refs.pixelmem + vctx->refs.txtlayer->origin;

    xdelta = VPU_FIXED_FONT_WIDTH;
    ydelta = vctx->disp.w * (vctx->disp.fixedfont->height - 1);

    nrows = vctx->refs.txtlayer->rows;
    ncols = vctx->refs.txtlayer->cols;

    currchval = vctx->refs.txt_charmem;
    fgcp = TXTCOLORPOS(vctx, 0, 0);
    bgcp = TXTBGCOLORPOS(vctx, 0, 0);
    atrp = TXTATTRPOS(vctx, 0, 0);

    for (r = 0; r < nrows; r++) {
        for (c = 0; c < ncols; c++) {
            blitglyph(vctx, *currchval++, dest, *fgcp++, *bgcp++, *atrp++);
            dest += xdelta;
        }
        dest += ydelta;
    }

    vpu_direct_write_end(vctx);
}

static void blitglyph(VideoSys *vctx, int ch, uint32_t *dest,
                      uint32_t fgcolour, uint32_t bgcolour, uint32_t attr)
{
    unsigned fontheight, mask;
    const unsigned char *glyph;
    unsigned char gpx;
    unsigned i;
    uint32_t *destrow;
    unsigned destdelta;

    glyph = vctx->disp.fixedfont->pixeldata
            + ch * vctx->disp.fixedfont->height;

    fontheight = vctx->disp.fixedfont->height;

    destdelta = vctx->refs.txtlayer->cols * VPU_FIXED_FONT_WIDTH;
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
