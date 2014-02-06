#ifndef BACH_VPU_FONTS_H
#define BACH_VPU_FONTS_H

#define VPU_FIXED_FONT_WIDTH    8

struct vidfont8 {
    unsigned height;
    const unsigned char *pixeldata;
};

extern const struct vidfont8 vidfont8x8;
extern const struct vidfont8 vidfont8x14;
extern const struct vidfont8 vidfont8x16;

const unsigned char *vidfont_getglyph(const struct vidfont8 *font, int ch);

#endif /* BACH_VPU_FONTS_H */
