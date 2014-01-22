#ifndef VID_BACKENDPRIV_H
#define VID_BACKENDPRIV_H

#include <SDL/SDL.h>

struct display_privdata {
    SDL_Surface *vidsurface;
    uint32_t *vpixels;
};

extern struct display vpu_prv;
extern struct display_privdata vpu_pdata_prv;

#define VPU_PRV_INSTANCE    (vpu_prv)
#define VPU_PRV_PIXELS      (vpu_pdata_prv.vpixels)
#define VID_PRV_SURFACE     (vpu_pdata_prv.vidsurface)
#define VPU_TL              (vpu_prv.txt)
#define VPU_TL_MEM          (vpu_prv.txt.charmem)
#define VPU_TL_PARAMSMEM    (vpu_prv.txt.params)
#define VPU_TL_FGCOLORMEM   (vpu_prv.txt.params)
#define VPU_TL_BGCOLORMEM   (vpu_prv.txt.params + vpu_prv.txt.cnum)
#define VPU_TL_ATTRMEM      (vpu_prv.txt.params + vpu_prv.txt.cnum * 2)

#define VPU_DIRECTPXWRITE_START()  (SDL_LockSurface(VID_PRV_SURFACE))
#define VPU_DIRECTPXWRITE_END()    (SDL_UnlockSurface(VID_PRV_SURFACE))

#endif /* VID_BACKENDPRIV_H */
