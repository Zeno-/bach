#ifndef BACH_VPU_BACKENDCONFIG_H
#define BACH_VPU_BACKENDCONFIG_H

#include "vpu_tests.h"

#define DEFAULT_CHFONT      vidfont8x14

#define VTXT_DEF_FGCOLOUR(v)   ( vpu_rgbto32((v), 0xC0, 0xC0, 0xC0) )
#define VTXT_DEF_BGCOLOUR(v)   ( vpu_rgbto32((v), 0x00, 0x00, 0x00) )
#define VGFX_DEF_BGCOLOUR(v)   ( vpu_rgbto32((v), 0x00, 0x00, 0x00) )

#define VTXT_DEF_TEXTFLAGS  ( VPU_TXTAUTOSCROLL | VPU_TXTLAYERVISIBLE )

#define SCREEN_PIXELS_X     800
#define SCREEN_PIXELS_Y     600

#define VPU_FPSLIMIT        60

#endif /* BACH_VPU_BACKENDCONFIG_H */
