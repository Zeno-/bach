#ifndef VPU_BACKENDTEXT_H
#define VPU_BACKENDTEXT_H

#include <stdint.h>
#include "private.h"

#define TXTBUFF_TABWIDTH 4

#define TXTCHPOS(x, y) \
    ( \
        VPU_TL_MEM \
        + (x) + (y) * VPU_TL.cols \
    )
#define TXTCOLORPOS(x, y) \
    ( \
        VPU_TL_COLORMEM \
        + (x) + (y) * VPU_TL.cols \
    )

#define TXTATTRPOS(x, y) ( \
    VPU_TL_ATTRMEM + (y) * VPU_TL.cols + (x) )

uint32_t *vpu_txtpixelorigin(uint8_t x, uint8_t y);

uint16_t vpu_textlayerflags(void);
void vpu_settextlayerflags(uint16_t flags);

void vpu_txtcls(void);
void vpu_scrolltexty(void);
void vpu_putchar(int ch);
void vpu_puts(const char *s);
void vpu_puttab(void);

void vpu_putcharat(int ch, uint8_t x, uint8_t y);

void vpu_chartoscreen(int ch, int x, int y);

void vpu_curssetpos(uint8_t x, uint8_t y);
void vpu_curshome(void);
void vpu_cursadvance(void);
void vpu_cursnewline(void);

#endif /* VPU_BACKENDTEXT_H */
