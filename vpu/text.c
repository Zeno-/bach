#include "text.h"

#include <string.h>

#include "vpu/video.h"
#include "vpu/fonts/bmfonts.h"
#include "common/warn.h"

inline static void restraincx(VideoSys *vctx);
inline static void restraincy(VideoSys *vctx);

uint32_t *
vpu_txtpixelorigin(VideoSys *vctx, uint8_t x, uint8_t y)
{
    uint32_t *pos;
    unsigned fontheight;

    fontheight = vctx->disp.fixedfont->height;
    pos = vctx->refs.pixelmem;
    pos += vctx->refs.txtlayer->origin;
    pos += vctx->disp.w * y * fontheight + x * VPU_FIXED_FONT_WIDTH;

    return pos;
}

uint16_t
vpu_textlayerflags(VideoSys *vctx)
{
    return vctx->refs.txtlayer->flags;
}

void
vpu_settextlayerflags(VideoSys *vctx, uint16_t flags)
{
    vctx->refs.txtlayer->flags = flags;
}

void
vpu_txtcls(VideoSys *vctx)
{
    vpu_clrtext(vctx);
    vpu_curshome(vctx);
}

void
vpu_scrolltexty(VideoSys *vctx)
{
    uint8_t *src, *dest;
    uint32_t *csrc, *cdest;

    src  = TXTCHPOS(vctx, 0, 1);
    dest = TXTCHPOS(vctx, 0, 0);
    memmove(dest,
            src,
            (vctx->refs.txtlayer->charmem_sz - vctx->refs.txtlayer->cols)
                * sizeof *src);
    memset(TXTCHPOS(vctx, 0, vctx->refs.txtlayer->rows-1),
           0,
           vctx->refs.txtlayer->cols);

    cdest = vctx->refs.txt_paramsmem;
    csrc  = vctx->refs.txt_paramsmem + vctx->refs.txtlayer->cols;
    memmove(cdest,
            csrc,
            (vctx->refs.txtlayer->params_sz - vctx->refs.txtlayer->cols)
                * sizeof *csrc);
    memset(TXTCOLORPOS(vctx, 0, vctx->refs.txtlayer->rows-1),
           vctx->refs.txtlayer->fgcolour, vctx->refs.txtlayer->cols);
    memset(TXTBGCOLORPOS(vctx, 0, vctx->refs.txtlayer->rows-1),
           vctx->refs.txtlayer->bgcolour, vctx->refs.txtlayer->cols);
    memset(TXTATTRPOS(vctx, 0, vctx->refs.txtlayer->rows-1),
           vctx->refs.txtlayer->attrib, vctx->refs.txtlayer->cols);
}

/**************************************************************************
 * Attributes and colours
 *************************************************************************/

uint32_t
vpu_textfg(VideoSys *vctx)
{
    return vctx->refs.txtlayer->fgcolour;
}

void
vpu_settextfg(VideoSys *vctx, uint32_t newcolour)
{
    vctx->refs.txtlayer->fgcolour = newcolour;
}

void
vpu_settextbg(VideoSys *vctx, uint32_t newcolour)
{
    vctx->refs.txtlayer->bgcolour = newcolour;
}

uint8_t
vpu_textattr(VideoSys *vctx)
{
    return vctx->refs.txtlayer->attrib;
}

void
vpu_settextattr(VideoSys *vctx, uint8_t attr)
{
    vctx->refs.txtlayer->attrib = attr;
}

/**************************************************************************
 * Characters and (simple) strings
 *************************************************************************/

void
vpu_putchar(VideoSys *vctx, int ch)
{
    vpu_putchar_c(vctx, ch, vctx->refs.txtlayer->fgcolour);
}

void
vpu_putchar_c(VideoSys *vctx, int ch, uint32_t colour)
{
    vpu_putcharat_c(vctx, ch,
                    vctx->refs.txtlayer->cursx, vctx->refs.txtlayer->cursy,
                    colour);
    vpu_cursadvance(vctx);
}

void
vpu_puts(VideoSys *vctx, const char *s)
{
    vpu_puts_c(vctx, s, vctx->refs.txtlayer->fgcolour);
}

void
vpu_puts_c(VideoSys *vctx, const char *s, uint32_t colour)
{
    int ch;
    while ((ch = *s++)) {
        switch(ch) {
            case '\n':
                vpu_cursnewline(vctx);
                break;
            case '\t':
                vpu_puttab_c(vctx, colour);
                break;
            default:
                vpu_putchar_c(vctx, ch, colour);
                break;
        }
    }
}

void
vpu_puttab(VideoSys *vctx)
{
    vpu_puttab_c(vctx, vctx->refs.txtlayer->fgcolour);
}

void
vpu_puttab_c(VideoSys *vctx, uint32_t colour)
{
    int i;

    /* FIXME:   Adjust spaces printed to align with tabstops */

    for (i = 0; i < TXTBUFF_TABWIDTH; i++) {
        vpu_putchar_c(vctx, ' ', colour);
    }
}

void
vpu_putcharat(VideoSys *vctx, int ch, uint8_t x, uint8_t y)
{
    vpu_putcharat_c(vctx, ch, x, y, vctx->refs.txtlayer->fgcolour);
}

void
vpu_putcharat_c(VideoSys *vctx, int ch, uint8_t x, uint8_t y, uint32_t colour)
{
    *TXTCHPOS(vctx, x, y) = ch;
    *TXTCOLORPOS(vctx, x, y) = colour;
    *TXTBGCOLORPOS(vctx, x, y) = vctx->refs.txtlayer->bgcolour;
    *TXTATTRPOS(vctx, x, y) = vctx->refs.txtlayer->attrib;
}

/**************************************************************************
 * Cursor positioning
 *************************************************************************/

void
vpu_curssetpos(VideoSys *vctx, uint8_t x, uint8_t y)
{
    vctx->refs.txtlayer->cursx = x;
    vctx->refs.txtlayer->cursy = y;
}

void
vpu_curssetposrel(VideoSys *vctx, int x, int y)
{
    vctx->refs.txtlayer->cursx += x;
    vctx->refs.txtlayer->cursy += y;
    restraincx(vctx);
    restraincy(vctx);
}

void
vpu_curshome(VideoSys *vctx)
{
    vctx->refs.txtlayer->cursx = 0;
}

void
vpu_cursadvance(VideoSys *vctx)
{
    vctx->refs.txtlayer->cursx++;
    if (vctx->refs.txtlayer->cursx >= vctx->refs.txtlayer->cols)
        vctx->refs.txtlayer->cursx = 0;
}

void
vpu_cursnewline(VideoSys *vctx)
{
    vctx->refs.txtlayer->cursx = 0;
    vctx->refs.txtlayer->cursy++;
    if  (vctx->refs.txtlayer->cursy >= vctx->refs.txtlayer->rows) {
        vctx->refs.txtlayer->cursy--;
        if (vctx->refs.txtlayer->flags & VPU_TXTAUTOSCROLL)
            vpu_scrolltexty(vctx);
    }
}

/**************************************************************************
 * Private
 *************************************************************************/

inline static void
restraincx(VideoSys *vctx)
{
    if (vctx->refs.txtlayer->cursx < 0)
        vctx->refs.txtlayer->cursx = 0;
    else if (vctx->refs.txtlayer->cursx >= vctx->refs.txtlayer->cols)
        vctx->refs.txtlayer->cursx = vctx->refs.txtlayer->cols - 1;
}

inline static void
restraincy(VideoSys *vctx)
{
    if (vctx->refs.txtlayer->cursy < 0)
        vctx->refs.txtlayer->cursy = 0;
    if (vctx->refs.txtlayer->cursy >= vctx->refs.txtlayer->rows)
        vctx->refs.txtlayer->cursy = vctx->refs.txtlayer->rows;
}
