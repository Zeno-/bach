#ifndef VPU_BACKENDCONFIG_H
#define VPU_BACKENDCONFIG_H

#define DEFAULT_CHFONT      vidfont8x14

#define DEFAULT_TEXTFG      ( vpu_rgbto32(0xC0, 0xC0, 0xC0) )
#define DEFAULT_TEXTBG      ( vpu_rgbto32(0x00, 0x00, 0x00) )
#define DEFAULT_BGCOLOUR    ( vpu_rgbto32(0x00, 0x00, 0x00) )

#define SCREEN_PIXELS_X     800
#define SCREEN_PIXELS_Y     600

#define VPU_FPSLIMIT        60

#endif /* VPU_BACKENDCONFIG_H */
