#ifndef BACH_VPU_FONTS_H
#define BACH_VPU_FONTS_H

#define VPU_FIXED_FONT_WIDTH    8

#define VPU_FONT_8x8_NAME    "8x8"
#define VPU_FONT_8x14_NAME   "8x14"
#define VPU_FONT_8x16_NAME   "8x16"

struct vidfont8 {
    unsigned height;
    const unsigned char *pixeldata;
};

extern const struct vidfont8 vidfont8x8;
extern const struct vidfont8 vidfont8x14;
extern const struct vidfont8 vidfont8x16;

const struct vidfont8 *vidfont_getnamedfont(const char *name);

const unsigned char *vidfont_getglyph(const struct vidfont8 *font, int ch);

#endif /* BACH_VPU_FONTS_H */
