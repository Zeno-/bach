#include "text.h"

#include <stdlib.h>
#include "vpu/fonts/bmfonts.h"
#include "base.h"
#include "private.h"
#include "common/warn.h"

inline static void restraincx(void);
inline static void restraincy(void);

void
vpu_clrtext(void)
{
    memset(VPU_TL_MEM, 0, VPU_PRV_INSTANCE.txt.cnum);
}

uint32_t *
vpu_txtpixelorigin(uint8_t x, uint8_t y)
{
    uint32_t *pos;
    unsigned fontheight;

    fontheight = VPU_PRV_INSTANCE.fixedfont->height;
    pos = VPU_PRV_PIXELS;
    pos += VPU_TL.origin;
    pos += VPU_PRV_INSTANCE.w * y * fontheight + x * VPU_FIXED_FONT_WIDTH;

    return pos;
}

uint16_t
vpu_textlayerflags(void)
{
    return VPU_TL.flags;
}

void
vpu_settextlayerflags(uint16_t flags)
{
    VPU_TL.flags = flags;
}

void
vpu_txtcls(void)
{
    memset(VPU_TL.mem, 0, VPU_TL.cnum);
    memset(VPU_TL.colours, 0, VPU_TL.cnum);
    memset(VPU_TL.attribs, 0, VPU_TL.cnum);
}

void
vpu_scrolltexty(void)
{
    uint8_t *src, *dest;
    uint32_t *csrc, *cdest;

    src  = TXTCHPOS(0, 1);
    dest = TXTCHPOS(0, 0);
    memmove(dest, src, (VPU_TL.cnum - VPU_TL.cols) * sizeof *src);
    memset(TXTCHPOS(0, VPU_TL.rows-1),
           0,
           VPU_TL.cols);

    /* Scroll colours */
    csrc = TXTCOLORPOS(0, 1);
    cdest = TXTCOLORPOS(0, 0);
    memmove(cdest, csrc, (VPU_TL.cnum - VPU_TL.cols) * sizeof *csrc);
    memset(TXTCOLORPOS(0, VPU_TL.rows-1),
           VPU_TL.fgcolour,
           VPU_TL.cols);

    /* Scroll background colours */
    csrc = TXTBGCOLORPOS(0, 1);
    cdest = TXTBGCOLORPOS(0, 0);
    memmove(cdest, csrc, (VPU_TL.cnum - VPU_TL.cols) * sizeof *csrc);
    memset(TXTBGCOLORPOS(0, VPU_TL.rows-1),
           VPU_TL.bgcolour,
           VPU_TL.cols);

    /* Scroll Attribs */
    src  = TXTATTRPOS(0, 1);
    dest = TXTATTRPOS(0, 0);
    memmove(dest, src, (VPU_TL.cnum - VPU_TL.cols) * sizeof *src);
    memset(TXTATTRPOS(0, VPU_TL.rows-1),
           0,
           VPU_TL.cols);
}

/**************************************************************************
 * Attributes and colours
 *************************************************************************/

uint32_t
vpu_textfg(void)
{
    return VPU_TL.fgcolour;
}

void
vpu_settextfg(uint32_t newcolour)
{
    VPU_TL.fgcolour = newcolour;
}

void
vpu_settextbg(uint32_t newcolour)
{
    VPU_TL.bgcolour = newcolour;
}

uint8_t
vpu_textattr(void)
{
    return VPU_TL.attrib;
}

void
vpu_settextattr(uint8_t attr)
{
    VPU_TL.attrib = attr;
}

/**************************************************************************
 * Characters and (simple) strings
 *************************************************************************/

void
vpu_putchar(int ch)
{
    vpu_putchar_c(ch, VPU_TL.fgcolour);
}

void
vpu_putchar_c(int ch, uint32_t colour)
{
    vpu_putcharat_c(ch, VPU_TL.cursx, VPU_TL.cursy, colour);
    vpu_cursadvance();
}

void
vpu_puts(const char *s)
{
    vpu_puts_c(s, VPU_TL.fgcolour);
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
    vpu_puttab_c(VPU_TL.fgcolour);
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
    vpu_putcharat_c(ch, x, y, VPU_TL.fgcolour);
}

void
vpu_putcharat_c(int ch, uint8_t x, uint8_t y, uint32_t colour)
{
    *TXTCHPOS(x, y) = ch;
    *TXTCOLORPOS(x, y) = colour;
    *TXTBGCOLORPOS(x, y) = VPU_TL.bgcolour;
    *TXTATTRPOS(x, y) = VPU_TL.attrib;
}

/**************************************************************************
 * Cursor positioning
 *************************************************************************/

void
vpu_curssetpos(uint8_t x, uint8_t y)
{
    VPU_TL.cursx = x;
    VPU_TL.cursy = y;
}

void
vpu_curssetposrel(int x, int y)
{
    VPU_TL.cursx += x;
    VPU_TL.cursy += y;
    restraincx();
    restraincy();
}

void
vpu_curshome(void)
{
    VPU_TL.cursx = 0;
}

void
vpu_cursadvance(void)
{
    VPU_TL.cursx++;
    if (VPU_TL.cursx >= VPU_TL.cols)
        VPU_TL.cursx = 0;
}

void
vpu_cursnewline(void)
{
    VPU_TL.cursx = 0;
    VPU_TL.cursy++;
    if  (VPU_TL.cursy >= VPU_TL.rows) {
        VPU_TL.cursy--;
        if (VPU_TL.flags & VPU_TXTAUTOSCROLL)
            vpu_scrolltexty();
    }
}

/**************************************************************************
 * Private
 *************************************************************************/

inline static void
restraincx(void)
{
    if (VPU_TL.cursx < 0)
        VPU_TL.cursx = 0;
    else if (VPU_TL.cursx >= VPU_TL.cols)
        VPU_TL.cursx = VPU_TL.cols - 1;
}

inline static void
restraincy(void)
{
    if (VPU_TL.cursy < 0)
        VPU_TL.cursy = 0;
    if (VPU_TL.cursy >= VPU_TL.rows)
        VPU_TL.cursy = VPU_TL.rows;
}
