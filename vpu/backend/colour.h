#ifndef VPU_BACKENDCOLOUR_H
#define VPU_BACKENDCOLOUR_H

struct RGB {
    unsigned char r, g, b;
};

#define VID_RGBTO32(s) ( vid_rgbto32((s)->r, (s)->g, (s)->b) )

#endif /* VPU_BACKENDCOLOUR_H */
