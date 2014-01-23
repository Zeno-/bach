#ifndef VPU_BACKENDTEXT_H
#define VPU_BACKENDTEXT_H

#include <stdint.h>
#include "hal/video_hal_refs.h"

#define TXTBUFF_TABWIDTH 4

#define TXTCHPOS(x, y) \
    ( \
        vpu_tl_charemem \
        + (x) + (y) * vpu_txtlayer->cols \
    )
#define TXTCOLORPOS(x, y) \
    ( \
        vpu_tl_fgcolormem \
        + (x) + (y) * vpu_txtlayer->cols \
    )

#define TXTBGCOLORPOS(x, y) \
    ( \
        vpu_tl_bgcolormem \
        + (x) + (y) * vpu_txtlayer->cols \
    )

#define TXTATTRPOS(x, y) \
    ( \
        vpu_tl_attrmem \
        + (x) + (y) * vpu_txtlayer->cols \
    )

uint32_t    *vpu_txtpixelorigin(uint8_t x, uint8_t y);

uint16_t    vpu_textlayerflags(void);
void        vpu_settextlayerflags(uint16_t flags);

void        vpu_txtcls(void);
void        vpu_scrolltexty(void);

uint32_t    vpu_textfg(void);
void        vpu_settextfg(uint32_t newcolour);
void        vpu_settextbg(uint32_t newcolour);
uint8_t     vpu_textattr(void);
void        vpu_settextattr(uint8_t attr);

void        vpu_putchar(int ch);
void        vpu_putchar_c(int ch, uint32_t colour);
void        vpu_puts(const char *s);
void        vpu_puts_c(const char *s, uint32_t colour);
void        vpu_puttab(void);
void        vpu_puttab_c(uint32_t colour);
void        vpu_putcharat(int ch, uint8_t x, uint8_t y);
void        vpu_putcharat_c(int ch, uint8_t x, uint8_t y, uint32_t colour);

void        vpu_curssetpos(uint8_t x, uint8_t y);
void        vpu_curssetposrel(int x, int y);
void        vpu_curshome(void);
void        vpu_cursadvance(void);
void        vpu_cursnewline(void);

#endif /* VPU_BACKENDTEXT_H */
