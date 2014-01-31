#ifndef VPU_BACKENDCONFIG_H
#define VPU_BACKENDCONFIG_H

#define DEFAULT_CHFONT      vidfont8x14

#define VTXT_DEF_FGCOLOUR   ( vpu_rgbto32(0xC0, 0xC0, 0xC0) )
#define VTXT_DEF_BGCOLOUR   ( vpu_rgbto32(0x00, 0x00, 0x00) )
#define VGFX_DEF_BGCOLOUR   ( vpu_rgbto32(0x00, 0x00, 0x00) )

#define VTXT_DEF_TEXTFLAGS  ( VPU_TXTAUTOSCROLL | VPU_TXTLAYERVISIBLE )

#define SCREEN_PIXELS_X     800
#define SCREEN_PIXELS_Y     600

#define VPU_FPSLIMIT        60

/* 0 = Build no tests
 * 1 = Build test as defined by VPU_BUILDTEST_*
 */
#define VPU_BUILDTESTS              1

#define VPU_BUILDTEST_BASICCOLOURS  0
#define VPU_BUILDTEST_SCROLL        0
#define VPU_BUILDTEST_TERRAIN       1

#endif /* VPU_BACKENDCONFIG_H */
