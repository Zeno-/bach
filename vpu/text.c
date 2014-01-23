#include "text.h"

#include <string.h>

#include "vpu/video.h"
#include "vpu/fonts/bmfonts.h"
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
    pos += vpu_txtlayer->origin;
    pos += vpu_instance->w * y * fontheight + x * VPU_FIXED_FONT_WIDTH;

    return pos;
}

uint16_t
vpu_textlayerflags(void)
{
    return vpu_txtlayer->flags;
}

void
vpu_settextlayerflags(uint16_t flags)
{
    vpu_txtlayer->flags = flags;
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
    memmove(dest,
            src,
            (vpu_txtlayer->charmem_sz - vpu_txtlayer->cols) * sizeof *src);
    memset(TXTCHPOS(0, vpu_txtlayer->rows-1), 0, vpu_txtlayer->cols);

    cdest = vpu_tl_paramsmem;
    csrc  = vpu_tl_paramsmem + vpu_txtlayer->cols;
    memmove(cdest,
            csrc,
            (vpu_txtlayer->params_sz - vpu_txtlayer->cols) * sizeof *csrc);
    memset(TXTCOLORPOS(0, vpu_txtlayer->rows-1),
           vpu_txtlayer->fgcolour, vpu_txtlayer->cols);
    memset(TXTBGCOLORPOS(0, vpu_txtlayer->rows-1),
           vpu_txtlayer->bgcolour, vpu_txtlayer->cols);
    memset(TXTATTRPOS(0, vpu_txtlayer->rows-1),
           vpu_txtlayer->attrib, vpu_txtlayer->cols);
}

/**************************************************************************
 * Attributes and colours
 *************************************************************************/

uint32_t
vpu_textfg(void)
{
    return vpu_txtlayer->fgcolour;
}

void
vpu_settextfg(uint32_t newcolour)
{
    vpu_txtlayer->fgcolour = newcolour;
}

void
vpu_settextbg(uint32_t newcolour)
{
    vpu_txtlayer->bgcolour = newcolour;
}

uint8_t
vpu_textattr(void)
{
    return vpu_txtlayer->attrib;
}

void
vpu_settextattr(uint8_t attr)
{
    vpu_txtlayer->attrib = attr;
}

/**************************************************************************
 * Characters and (simple) strings
 *************************************************************************/

void
vpu_putchar(int ch)
{
    vpu_putchar_c(ch, vpu_txtlayer->fgcolour);
}

void
vpu_putchar_c(int ch, uint32_t colour)
{
    vpu_putcharat_c(ch, vpu_txtlayer->cursx, vpu_txtlayer->cursy, colour);
    vpu_cursadvance();
}

void
vpu_puts(const char *s)
{
    vpu_puts_c(s, vpu_txtlayer->fgcolour);
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
    vpu_puttab_c(vpu_txtlayer->fgcolour);
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
    vpu_putcharat_c(ch, x, y, vpu_txtlayer->fgcolour);
}

void
vpu_putcharat_c(int ch, uint8_t x, uint8_t y, uint32_t colour)
{
    *TXTCHPOS(x, y) = ch;
    *TXTCOLORPOS(x, y) = colour;
    *TXTBGCOLORPOS(x, y) = vpu_txtlayer->bgcolour;
    *TXTATTRPOS(x, y) = vpu_txtlayer->attrib;
}

/**************************************************************************
 * Cursor positioning
 *************************************************************************/

void
vpu_curssetpos(uint8_t x, uint8_t y)
{
    vpu_txtlayer->cursx = x;
    vpu_txtlayer->cursy = y;
}

void
vpu_curssetposrel(int x, int y)
{
    vpu_txtlayer->cursx += x;
    vpu_txtlayer->cursy += y;
    restraincx();
    restraincy();
}

void
vpu_curshome(void)
{
    vpu_txtlayer->cursx = 0;
}

void
vpu_cursadvance(void)
{
    vpu_txtlayer->cursx++;
    if (vpu_txtlayer->cursx >= vpu_txtlayer->cols)
        vpu_txtlayer->cursx = 0;
}

void
vpu_cursnewline(void)
{
    vpu_txtlayer->cursx = 0;
    vpu_txtlayer->cursy++;
    if  (vpu_txtlayer->cursy >= vpu_txtlayer->rows) {
        vpu_txtlayer->cursy--;
        if (vpu_txtlayer->flags & VPU_TXTAUTOSCROLL)
            vpu_scrolltexty();
    }
}

/**************************************************************************
 * Private
 *************************************************************************/

inline static void
restraincx(void)
{
    if (vpu_txtlayer->cursx < 0)
        vpu_txtlayer->cursx = 0;
    else if (vpu_txtlayer->cursx >= vpu_txtlayer->cols)
        vpu_txtlayer->cursx = vpu_txtlayer->cols - 1;
}

inline static void
restraincy(void)
{
    if (vpu_txtlayer->cursy < 0)
        vpu_txtlayer->cursy = 0;
    if (vpu_txtlayer->cursy >= vpu_txtlayer->rows)
        vpu_txtlayer->cursy = vpu_txtlayer->rows;
}
