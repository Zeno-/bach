#include "text.h"

#include <stdlib.h>
#include "vpu/fonts/bmfonts.h"
#include "base.h"
#include "private.h"
#include "common/warn.h"

void
vpu_clrtext(void)
{
    memset(VPU_TL_MEM, 0, VPU_PRV_INSTANCE.txt.cnum);
}

uint32_t *vpu_txtpixelorigin(uint8_t x, uint8_t y)
{
    uint32_t *pos;
    unsigned fontheight;

    fontheight = VPU_PRV_INSTANCE.fixedfont->height;
    pos = VPU_PRV_PIXELS;
    pos += VPU_TL.origin;
    pos += VPU_PRV_INSTANCE.w * y * fontheight + x * VPU_FIXED_FONT_WIDTH;

    return pos;
}

void
vpu_scrolltexty(void)
{
    uint8_t *src, *dest;
    uint32_t *csrc, *cdest;

    src  = TXTCHPOS(0, 1);
    dest = TXTCHPOS(0, 0);

    memmove(dest, src, VPU_TL.cnum - VPU_TL.cols);
    memset(TXTCHPOS(0, VPU_TL.rows-1),
           0,
           VPU_TL.cols);

    /* Scroll colours */
    csrc = TXTCOLORPOS(0, 1);
    cdest = TXTCOLORPOS(0, 0);
    memmove(cdest, csrc, VPU_TL.cnum - VPU_TL.cols);
    memset(TXTCOLORPOS(0, VPU_TL.rows-1),
           VPU_TL.fgcolour,
           VPU_TL.cols);

    /* TODO: FIXME: Scroll Attribs */
}

void
vpu_putchar(int ch)
{
    vpu_putcharat(ch, VPU_TL.cursx, VPU_TL.cursy);
    vpu_cursadvance();
}

void
vpu_puts(const char *s)
{
    int ch;
    while ((ch = *s++)) {
        switch(ch) {
            case '\n':
                vpu_cursnewline();
                break;
            case '\t':
                vpu_puttab();
                break;
            default:
                vpu_putchar(ch);
                break;
        }
    }
}

void
vpu_puttab(void)
{
    int i;
    for (i = 0; i < TXTBUFF_TABWIDTH; i++) {
        vpu_putchar(' ');
    }
}

void
vpu_putcharat(int ch, uint8_t x, uint8_t y)
{
    *TXTCHPOS(x, y) = ch;
}

void
vpu_curssetpos(uint8_t x, uint8_t y)
{
    VPU_TL.cursx = x;
    VPU_TL.cursy = y;
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
