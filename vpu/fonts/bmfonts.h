#ifndef VPU_BMFONTS_H
#define VPU_BMFONTS_H

#define VPU_FIXED_FONT_WIDTH    8

struct vidfont8 {
    unsigned height;
    const unsigned char *pixeldata;
};

extern const struct vidfont8 vidfont8x8;
extern const struct vidfont8 vidfont8x14;
extern const struct vidfont8 vidfont8x16;

const unsigned char *vidfont_getglyph(const struct vidfont8 *font, int ch);

#endif /* VPU_BMFONTS_H */
