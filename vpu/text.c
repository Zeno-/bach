#include "text.h"

#include <stdlib.h>
#include "vpu/fonts/bmfonts.h"
#include "hal/video_hal.h"
#include "hal/video_hal_private.h"
#include "common/warn.h"

inline static void restraincx(void);
inline static void restraincy(void);

uint32_t *
vpu_txtpixelorigin(uint8_t x, uint8_t y)
{
    uint32_t *pos;
    unsigned fontheight;

    fontheight = vpu_instance->fixedfont->height;
    pos = vpu_pixelmem;
    pos += vpu_tl->origin;
    pos += vpu_instance->w * y * fontheight + x * VPU_FIXED_FONT_WIDTH;

    return pos;
}

uint16_t
vpu_textlayerflags(void)
{
    return vpu_tl->flags;
}

void
vpu_settextlayerflags(uint16_t flags)
{
    vpu_tl->flags = flags;
}

void
vpu_txtcls(void)
{
    vpu_clrtext();
    vpu_curshome();
}

void
vpu_scrolltexty(void)
{
    uint8_t *src, *dest;
    uint32_t *csrc, *cdest;

    src  = TXTCHPOS(0, 1);
    dest = TXTCHPOS(0, 0);
    memmove(dest, src, (vpu_tl->charmem_sz - vpu_tl->cols) * sizeof *src);
    memset(TXTCHPOS(0, vpu_tl->rows-1), 0, vpu_tl->cols);

    cdest = vpu_tl_paramsmem;
    csrc  = vpu_tl_paramsmem + vpu_tl->cols;
    memmove(cdest, csrc, (vpu_tl->params_sz - vpu_tl->cols) * sizeof *csrc);
    memset(TXTCOLORPOS(0, vpu_tl->rows-1), vpu_tl->fgcolour, vpu_tl->cols);
    memset(TXTBGCOLORPOS(0, vpu_tl->rows-1), vpu_tl->bgcolour, vpu_tl->cols);
    memset(TXTATTRPOS(0, vpu_tl->rows-1), vpu_tl->attrib, vpu_tl->cols);
}

/**************************************************************************
 * Attributes and colours
 *************************************************************************/

uint32_t
vpu_textfg(void)
{
    return vpu_tl->fgcolour;
}

void
vpu_settextfg(uint32_t newcolour)
{
    vpu_tl->fgcolour = newcolour;
}

void
vpu_settextbg(uint32_t newcolour)
{
    vpu_tl->bgcolour = newcolour;
}

uint8_t
vpu_textattr(void)
{
    return vpu_tl->attrib;
}

void
vpu_settextattr(uint8_t attr)
{
    vpu_tl->attrib = attr;
}

/**************************************************************************
 * Characters and (simple) strings
 *************************************************************************/

void
vpu_putchar(int ch)
{
    vpu_putchar_c(ch, vpu_tl->fgcolour);
}

void
vpu_putchar_c(int ch, uint32_t colour)
{
    vpu_putcharat_c(ch, vpu_tl->cursx, vpu_tl->cursy, colour);
    vpu_cursadvance();
}

void
vpu_puts(const char *s)
{
    vpu_puts_c(s, vpu_tl->fgcolour);
}

void
vpu_puts_c(const char *s, uint32_t colour)
{
    int ch;
    while ((ch = *s++)) {
        switch(ch) {
            case '\n':
                vpu_cursnewline();
                break;
            case '\t':
                vpu_puttab_c(colour);
                break;
            default:
                vpu_putchar_c(ch, colour);
                break;
        }
    }
}

void
vpu_puttab(void)
{
    vpu_puttab_c(vpu_tl->fgcolour);
}

void
vpu_puttab_c(uint32_t colour)
{
    int i;
    for (i = 0; i < TXTBUFF_TABWIDTH; i++) {
        vpu_putchar_c(' ', colour);
    }
}

void
vpu_putcharat(int ch, uint8_t x, uint8_t y)
{
    vpu_putcharat_c(ch, x, y, vpu_tl->fgcolour);
}

void
vpu_putcharat_c(int ch, uint8_t x, uint8_t y, uint32_t colour)
{
    *TXTCHPOS(x, y) = ch;
    *TXTCOLORPOS(x, y) = colour;
    *TXTBGCOLORPOS(x, y) = vpu_tl->bgcolour;
    *TXTATTRPOS(x, y) = vpu_tl->attrib;
}

/**************************************************************************
 * Cursor positioning
 *************************************************************************/

void
vpu_curssetpos(uint8_t x, uint8_t y)
{
    vpu_tl->cursx = x;
    vpu_tl->cursy = y;
}

void
vpu_curssetposrel(int x, int y)
{
    vpu_tl->cursx += x;
    vpu_tl->cursy += y;
    restraincx();
    restraincy();
}

void
vpu_curshome(void)
{
    vpu_tl->cursx = 0;
}

void
vpu_cursadvance(void)
{
    vpu_tl->cursx++;
    if (vpu_tl->cursx >= vpu_tl->cols)
        vpu_tl->cursx = 0;
}

void
vpu_cursnewline(void)
{
    vpu_tl->cursx = 0;
    vpu_tl->cursy++;
    if  (vpu_tl->cursy >= vpu_tl->rows) {
        vpu_tl->cursy--;
        if (vpu_tl->flags & VPU_TXTAUTOSCROLL)
            vpu_scrolltexty();
    }
}

/**************************************************************************
 * Private
 *************************************************************************/

inline static void
restraincx(void)
{
    if (vpu_tl->cursx < 0)
        vpu_tl->cursx = 0;
    else if (vpu_tl->cursx >= vpu_tl->cols)
        vpu_tl->cursx = vpu_tl->cols - 1;
}

inline static void
restraincy(void)
{
    if (vpu_tl->cursy < 0)
        vpu_tl->cursy = 0;
    if (vpu_tl->cursy >= vpu_tl->rows)
        vpu_tl->cursy = vpu_tl->rows;
}
