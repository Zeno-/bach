#ifndef BACH_VPU_BACKENDCONFIG_H
#define BACH_VPU_BACKENDCONFIG_H

#define DEFAULT_CHFONT      vidfont8x14

#define VTXT_DEF_FGCOLOUR(v)   ( vpu_rgbto32((v), 0xC0, 0xC0, 0xC0) )
#define VTXT_DEF_BGCOLOUR(v)   ( vpu_rgbto32((v), 0x00, 0x00, 0x00) )
#define VGFX_DEF_BGCOLOUR(v)   ( vpu_rgbto32((v), 0x00, 0x00, 0x00) )

#define VTXT_DEF_TEXTFLAGS  ( VPU_TXTAUTOSCROLL | VPU_TXTLAYERVISIBLE )

#define SCREEN_PIXELS_X     800
#define SCREEN_PIXELS_Y     600

#define VPU_FPSLIMIT        60

/* 0 = Build no tests
 * 1 = Build test as defined by VPU_BUILDTEST_*
 */
#define VPU_BUILDTESTS              1
#define VPU_RUNTESTS                (VPU_BUILDTESTS)

#define VPU_BUILDTEST_BASICCOLOURS  0
#define VPU_BUILDTEST_SCROLL        0
#define VPU_BUILDTEST_TERRAIN       1

#define VPU_RUNTEST_BASICCOLOURS    (VPU_BUILDTEST_BASICCOLOURS)
#define VPU_RUNTEST_SCROLL          (VPU_BUILDTEST_SCROLL)
#define VPU_RUNTEST_TERRAIN         (VPU_BUILDTEST_TERRAIN)

#endif /* BACH_VPU_BACKENDCONFIG_H */
