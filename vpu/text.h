#ifndef VPU_BACKENDTEXT_H
#define VPU_BACKENDTEXT_H

#include <stdint.h>
#include "hal/hal.h"
#include "vpu/video.h"

#define TXTBUFF_TABWIDTH 4

#define TXTCHPOS(v, x, y) \
    ( \
        (v)->refs.txt_charmem \
        + (x) + (y) * (v)->refs.txtlayer->cols \
    )
#define TXTCOLORPOS(v, x, y) \
    ( \
        (v)->refs.txt_fgcolormem \
        + (x) + (y) * (v)->refs.txtlayer->cols \
    )

#define TXTBGCOLORPOS(v, x, y) \
    ( \
        (v)->refs.txt_bgcolormem \
        + (x) + (y) * (v)->refs.txtlayer->cols \
    )

#define TXTATTRPOS(v, x, y) \
    ( \
        (v)->refs.txt_attrmem \
        + (x) + (y) * (v)->refs.txtlayer->cols \
    )

uint32_t    *vpu_txtpixelorigin(VideoSys *vctx, uint8_t x, uint8_t y);

uint16_t    vpu_textlayerflags(VideoSys *vctx);
void        vpu_settextlayerflags(VideoSys *vctx, uint16_t flags);

void        vpu_txtcls(VideoSys *vctx);
void        vpu_scrolltexty(VideoSys *vctx);

uint32_t    vpu_textfg(VideoSys *vctx);
void        vpu_settextfg(VideoSys *vctx, uint32_t newcolour);
void        vpu_settextbg(VideoSys *vctx, uint32_t newcolour);
uint8_t     vpu_textattr(VideoSys *vctx);
void        vpu_settextattr(VideoSys *vctx, uint8_t attr);

void        vpu_putchar(VideoSys *vctx, int ch);
void        vpu_putchar_c(VideoSys *vctx, int ch, uint32_t colour);
void        vpu_puts(VideoSys *vctx, const char *s);
void        vpu_puts_c(VideoSys *vctx, const char *s, uint32_t colour);
void        vpu_puttab(VideoSys *vctx);
void        vpu_puttab_c(VideoSys *vctx, uint32_t colour);
void        vpu_putcharat(VideoSys *vctx, int ch, uint8_t x, uint8_t y);
void        vpu_putcharat_c(VideoSys *vctx, int ch, uint8_t x, uint8_t y, uint32_t colour);

void        vpu_curssetpos(VideoSys *vctx, uint8_t x, uint8_t y);
void        vpu_curssetposrel(VideoSys *vctx, int x, int y);
void        vpu_curshome(VideoSys *vctx);
void        vpu_cursadvance(VideoSys *vctx);
void        vpu_cursnewline(VideoSys *vctx);

#endif /* VPU_BACKENDTEXT_H */
