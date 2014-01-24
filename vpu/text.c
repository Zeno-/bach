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

    fontheight = vpurefs.instance->fixedfont->height;
    pos = vpurefs.pixelmem;
    pos += vpurefs.txtlayer->origin;
    pos += vpurefs.instance->w * y * fontheight + x * VPU_FIXED_FONT_WIDTH;

    return pos;
}

uint16_t
vpu_textlayerflags(void)
{
    return vpurefs.txtlayer->flags;
}

void
vpu_settextlayerflags(uint16_t flags)
{
    vpurefs.txtlayer->flags = flags;
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
            (vpurefs.txtlayer->charmem_sz - vpurefs.txtlayer->cols)
                * sizeof *src);
    memset(TXTCHPOS(0, vpurefs.txtlayer->rows-1),
           0,
           vpurefs.txtlayer->cols);

    cdest = vpurefs.txt_paramsmem;
    csrc  = vpurefs.txt_paramsmem + vpurefs.txtlayer->cols;
    memmove(cdest,
            csrc,
            (vpurefs.txtlayer->params_sz - vpurefs.txtlayer->cols)
                * sizeof *csrc);
    memset(TXTCOLORPOS(0, vpurefs.txtlayer->rows-1),
           vpurefs.txtlayer->fgcolour, vpurefs.txtlayer->cols);
    memset(TXTBGCOLORPOS(0, vpurefs.txtlayer->rows-1),
           vpurefs.txtlayer->bgcolour, vpurefs.txtlayer->cols);
    memset(TXTATTRPOS(0, vpurefs.txtlayer->rows-1),
           vpurefs.txtlayer->attrib, vpurefs.txtlayer->cols);
}

/**************************************************************************
 * Attributes and colours
 *************************************************************************/

uint32_t
vpu_textfg(void)
{
    return vpurefs.txtlayer->fgcolour;
}

void
vpu_settextfg(uint32_t newcolour)
{
    vpurefs.txtlayer->fgcolour = newcolour;
}

void
vpu_settextbg(uint32_t newcolour)
{
    vpurefs.txtlayer->bgcolour = newcolour;
}

uint8_t
vpu_textattr(void)
{
    return vpurefs.txtlayer->attrib;
}

void
vpu_settextattr(uint8_t attr)
{
    vpurefs.txtlayer->attrib = attr;
}

/**************************************************************************
 * Characters and (simple) strings
 *************************************************************************/

void
vpu_putchar(int ch)
{
    vpu_putchar_c(ch, vpurefs.txtlayer->fgcolour);
}

void
vpu_putchar_c(int ch, uint32_t colour)
{
    vpu_putcharat_c(ch, vpurefs.txtlayer->cursx, vpurefs.txtlayer->cursy,
                    colour);
    vpu_cursadvance();
}

void
vpu_puts(const char *s)
{
    vpu_puts_c(s, vpurefs.txtlayer->fgcolour);
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
    vpu_puttab_c(vpurefs.txtlayer->fgcolour);
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
    vpu_putcharat_c(ch, x, y, vpurefs.txtlayer->fgcolour);
}

void
vpu_putcharat_c(int ch, uint8_t x, uint8_t y, uint32_t colour)
{
    *TXTCHPOS(x, y) = ch;
    *TXTCOLORPOS(x, y) = colour;
    *TXTBGCOLORPOS(x, y) = vpurefs.txtlayer->bgcolour;
    *TXTATTRPOS(x, y) = vpurefs.txtlayer->attrib;
}

/**************************************************************************
 * Cursor positioning
 *************************************************************************/

void
vpu_curssetpos(uint8_t x, uint8_t y)
{
    vpurefs.txtlayer->cursx = x;
    vpurefs.txtlayer->cursy = y;
}

void
vpu_curssetposrel(int x, int y)
{
    vpurefs.txtlayer->cursx += x;
    vpurefs.txtlayer->cursy += y;
    restraincx();
    restraincy();
}

void
vpu_curshome(void)
{
    vpurefs.txtlayer->cursx = 0;
}

void
vpu_cursadvance(void)
{
    vpurefs.txtlayer->cursx++;
    if (vpurefs.txtlayer->cursx >= vpurefs.txtlayer->cols)
        vpurefs.txtlayer->cursx = 0;
}

void
vpu_cursnewline(void)
{
    vpurefs.txtlayer->cursx = 0;
    vpurefs.txtlayer->cursy++;
    if  (vpurefs.txtlayer->cursy >= vpurefs.txtlayer->rows) {
        vpurefs.txtlayer->cursy--;
        if (vpurefs.txtlayer->flags & VPU_TXTAUTOSCROLL)
            vpu_scrolltexty();
    }
}

/**************************************************************************
 * Private
 *************************************************************************/

inline static void
restraincx(void)
{
    if (vpurefs.txtlayer->cursx < 0)
        vpurefs.txtlayer->cursx = 0;
    else if (vpurefs.txtlayer->cursx >= vpurefs.txtlayer->cols)
        vpurefs.txtlayer->cursx = vpurefs.txtlayer->cols - 1;
}

inline static void
restraincy(void)
{
    if (vpurefs.txtlayer->cursy < 0)
        vpurefs.txtlayer->cursy = 0;
    if (vpurefs.txtlayer->cursy >= vpurefs.txtlayer->rows)
        vpurefs.txtlayer->cursy = vpurefs.txtlayer->rows;
}
