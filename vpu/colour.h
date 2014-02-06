#ifndef BACH_VPU_BACKENDCOLOUR_H
#define BACH_VPU_BACKENDCOLOUR_H

struct RGB {
    unsigned char r, g, b;
};

#define VID_RGBTO32(s) ( vid_rgbto32((s)->r, (s)->g, (s)->b) )

#endif /* BACH_VPU_BACKENDCOLOUR_H */
