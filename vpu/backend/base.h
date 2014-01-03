#ifndef VPU_BACKEND_H
#define VPU_BACKEND_H

#include <stdint.h>

enum vpuerror {
    VPU_ERR_NONE,
    VPU_ERR_INITFAIL,
    VPU_ERR_VMODEFAIL,
    VPU_ERR_INITMEMFAIL
};

enum vpu_txtlayerflags {
    VPU_TXTAUTOSCROLL   = (1 << 0),
    VPU_TXTLAYERVISIBLE = (1 << 1)
};

enum vpu_txtlayerattribs {
    VPU_TXTATTRIB_REVERSE       = (1 << 0),
    VPU_TXTATTRIB_TRANSPARENT   = (1 << 1)
};

enum vpu_refreshaction {
    VPU_REFRESH_NORMAL  = 0,
    VPU_REFRESH_FORCE   = 1,
    VPU_REFRESH_COMMITONLY     = 2
};

struct txtlayer {
    uint16_t    flags;          /* c.f. enum vpu_txtlayerflags */
    int         rows, cols,
                cnum;           /* Total characters (row*cols) */

    uint8_t     *mem;           /* Character memory */

    uint32_t    *colours;
    uint8_t     *attribs;

    uint32_t    fgcolour;       /* Default colour used for font fg */
    uint32_t    bgcolour;       /* Default colour used for font bg */
    uint8_t     attrib;         /* Default attributes */

    int         cursx, cursy;   /* Cursor position */

    uint32_t    origin;         /* Pixel offset for top-left cell(0,0) */
};

struct display {
    const struct vidfont8
                *fixedfont;     /* Rendering font */

    unsigned w, h;              /* Dimensions of the pixel display */

    struct txtlayer txt;

    void *pdata;                /* Private */
};

enum vpuerror vpu_init(unsigned w, unsigned h, int fullscreen);
void vpu_cleanup(void);

const char * vpu_backendinfostr(void);
struct display *vpu_getinstance(void);

/* Returns 0 if interval since prev refresh is less than fps
 * interval. i.e. to limit framerate there is no need to refresh
 * the screen if this functions returns 0.
 */
int vpu_shouldrefresh(void);

void vpu_refresh(enum vpu_refreshaction action);
void vpu_clrdisplay(void);

uint32_t vpu_rgbto32(unsigned char r, unsigned char g, unsigned char b);

#endif /* VPU_BACKEND_H */
