#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include <SDL/SDL.h>

#include "hal/hal.h"
#include "video_hal_private.h"
#include "vpu/fonts/bmfonts.h"
#include "vpu/video.h"
#include "vpu/config.h"
#include "vpu/render.h"

#define VID_PRV_SURFACE(v)  ((v)->pdata->vidsurface)

static void initexports(VideoSys *vctx);
static enum vpuerror initscr(VideoSys *vctx, unsigned w, unsigned h,
                             int fullscreen);
static void setbackendinfo(VideoSys *vctx);
static void setfixedfont(VideoSys *vctx, const struct vidfont8 *font);

static enum vpuerror inittextsys(VideoSys *vctx,
                                 const struct vidfont8 *font);
static void cleanuptextsys(VideoSys *vctx);
static void initfpstimer(VideoSys *vctx, int fpslimit);

/**************************************************************************
 * Public
 *************************************************************************/

VideoSys *vpu_init(unsigned w, unsigned h, int fullscreen,
                   const struct vidfont8 *font,
                   enum vpuerror *err)
{
    enum vpuerror err_temp;
    VideoSys *vctx;

    if (!hal_isinitalised()) {
        fputs("Error: Hardware Abstraction Layer not initialised.\n",
              stderr);
        if (err)
            *err = VPU_ERR_INITFAIL;
        return NULL;
    }

    if ((vctx = calloc (1, sizeof *vctx)) == NULL) {
        if (err)
            *err = VPU_ERR_INITMEMFAIL;
        return NULL;
    }

    if ((err_temp = initscr(vctx, w, h, fullscreen)) != VPU_ERR_NONE) {
        if (err)
            *err = err_temp;
        return NULL;
    }

    initfpstimer(vctx, VPU_FPSLIMIT);

    if ((err_temp = inittextsys(vctx, font) != VPU_ERR_NONE)) {
        vpu_cleanup(vctx);
        return NULL;
    }

    initexports(vctx);
    vpu_clrtext(vctx);

    setbackendinfo(vctx);

    return vctx;
}

void
vpu_cleanup(VideoSys *vctx)
{
    if (!vctx)
        return;

    free (vctx->pdata);
    vctx->pdata = NULL;

    cleanuptextsys(vctx);

    free (vctx);
}

const char *
vpu_backendinfostr(VideoSys *vctx)
{
    return vctx->backendstr;
}

int
vpu_shouldrefresh(VideoSys *vctx)
{
    uint32_t currtick, expected;
    struct fpstimer *t;

    t = &vctx->fpsctx;

    /* Remember that everything is unsigned; i.e. don't use subtraction
     * unless certain that it won't cause an underflow
     */
    currtick = SDL_GetTicks();
    expected = t->prevtick + t->tickInterval;

    if (currtick < expected)
        return 0;

    t->prevtick = SDL_GetTicks();
    return 1;
}


void
vpu_refresh(VideoSys *vctx, enum vpu_refreshaction action)
{
    if (!vpu_shouldrefresh(vctx)
            && action != VPU_REFRESH_FORCE
            && action != VPU_REFRESH_COMMITONLY)
        return;
    vpu_refresh_tlayer(vctx);
    if (action != VPU_REFRESH_COMMITONLY)
        SDL_Flip(VID_PRV_SURFACE(vctx));
}

void
vpu_clrdisplay(VideoSys *vctx)
{
    SDL_Surface *surface = VID_PRV_SURFACE(vctx);

    // FIXME: the background colour should be modifiable
    SDL_FillRect(surface, NULL, VGFX_DEF_BGCOLOUR(vctx));
}

void
vpu_clrtext(VideoSys *vctx)
{
    memset(vctx->refs.txt_charmem, 0, vctx->refs.txtlayer->charmem_sz);
    memset(vctx->refs.txt_fgcolormem,
           vctx->refs.txtlayer->fgcolour,
           sizeof vctx->refs.txtlayer->fgcolour * vctx->refs.txtlayer->cnum);
    memset(vctx->refs.txt_bgcolormem,
           vctx->refs.txtlayer->bgcolour,
           sizeof vctx->refs.txtlayer->bgcolour * vctx->refs.txtlayer->cnum);
    memset(vctx->refs.txt_attrmem,
           vctx->refs.txtlayer->attrib,
           sizeof vctx->refs.txtlayer->attrib * vctx->refs.txtlayer->cnum);
}

void vpu_direct_write_start(VideoSys *vctx)
{
    SDL_LockSurface(VID_PRV_SURFACE(vctx));
}

void vpu_direct_write_end(VideoSys *vctx)
{
  SDL_UnlockSurface(VID_PRV_SURFACE(vctx));
}

uint32_t
vpu_rgbto32(VideoSys *vctx,
            unsigned char r, unsigned char g, unsigned char b)
{
    SDL_Surface *surface = VID_PRV_SURFACE(vctx);
    return SDL_MapRGB(surface->format, r, g, b);
}

/**************************************************************************
 * Private
 *************************************************************************/

static void
initexports(VideoSys *vctx)
{
    vctx->refs.txtlayer          = &vctx->disp.txt;

    vctx->refs.pixelmem          = vctx->pdata->vidsurface->pixels;
    vctx->refs.txt_charmem       = vctx->disp.txt.charmem;
    vctx->refs.txt_paramsmem     = vctx->disp.txt.params;
    vctx->refs.txt_fgcolormem    = vctx->disp.txt.params;
    vctx->refs.txt_bgcolormem    = vctx->disp.txt.params
                                    + vctx->disp.txt.cnum;
    vctx->refs.txt_attrmem       = vctx->disp.txt.params
                                    + vctx->disp.txt.cnum * 2;
}

static enum vpuerror
initscr(VideoSys *vctx, unsigned w, unsigned h, int fullscreen)
{
    SDL_Surface* surface;
    unsigned flags;
    struct display_privdata *pdata;

    /* Make sure width and height are multiples of 8 */
    w = (w >> 3) << 3;
    h = (h >> 3) << 3;

    flags = SDL_HWSURFACE | SDL_DOUBLEBUF | (fullscreen && SDL_FULLSCREEN);
    if ((surface = SDL_SetVideoMode(w, h, 32, flags)) == NULL)
        return VPU_ERR_VMODEFAIL;

    if((pdata = malloc(sizeof *pdata)) == NULL)
        return VPU_ERR_INITMEMFAIL;

    pdata->vidsurface = surface;
    vctx->pdata = pdata;

    vctx->disp.w = w;
    vctx->disp.h = h;
    vctx->disp.txt.fgcolour = VTXT_DEF_FGCOLOUR(vctx);
    vctx->disp.txt.bgcolour = VTXT_DEF_BGCOLOUR(vctx);
    vctx->disp.txt.attrib   = 0;

    return VPU_ERR_NONE;
}

static void
setbackendinfo(VideoSys *vctx)
{

    SDL_version compiled;

    SDL_VERSION(&compiled);

    snprintf(vctx->backendstr, VID_MAX_VERSIONINFO_LEN,
             "Backend using SDL %u.%u.%u",
             compiled.major, compiled.minor, compiled.patch);
}

static void
setfixedfont(VideoSys *vctx, const struct vidfont8 *font)
{
    uint32_t extra_xpx, extra_ypx;

    vctx->disp.fixedfont = font;
    vctx->disp.txt.cols = vctx->disp.w / VPU_FIXED_FONT_WIDTH;
    vctx->disp.txt.rows = vctx->disp.h / vctx->disp.fixedfont->height;
    vctx->disp.txt.charmem_sz = vctx->disp.txt.cols * vctx->disp.txt.rows;
    vctx->disp.txt.cnum = vctx->disp.txt.charmem_sz;
    vctx->disp.txt.params_sz = vctx->disp.txt.cnum * 3;

    /* Centre text "window" on screen; i.e. dertemine upper-left pixel of
     * the text display
     */
    extra_xpx = (vctx->disp.w % vctx->disp.txt.cols) / 2;
    extra_ypx = (vctx->disp.h % vctx->disp.txt.rows) / 2;
    vctx->disp.txt.origin = extra_xpx + extra_ypx * vctx->disp.w;
}

static enum vpuerror
inittextsys(VideoSys *vctx, const struct vidfont8 *font)
{
    size_t ccount, pmemreq;
    unsigned ok;

    vctx->disp.txt.flags = VTXT_DEF_TEXTFLAGS;

    setfixedfont(vctx, font != NULL ? font : &DEFAULT_CHFONT);

    ccount  = vctx->disp.txt.charmem_sz;
    pmemreq = vctx->disp.txt.params_sz * sizeof *vctx->disp.txt.params;

    /*****************************************************
     *
     *              PARAMS MEM LAYOUT
     *
     *      Each segment text rows * text cols in size
     *
     *      +---------------------------------------+
     *      +           Foreground colours          +   32 bit
     *      +---------------------------------------+
     *      +           Background colours          +   32 bit
     *      +---------------------------------------+
     *      +           Reserved & Attribs          +   32 bit[1]
     *      +---------------------------------------+
     *
     *  [1] Reserved & Attribs,
     *          Upper 24-bits:  reserved
     *          Lower 8-bits:   attribute flags
     */

    ok  = ( vctx->disp.txt.charmem
            = malloc(ccount * sizeof *vctx->disp.txt.charmem) ) != NULL;
    ok &= ( vctx->disp.txt.params = malloc(pmemreq) ) != NULL;

    if (!ok) {
        cleanuptextsys(vctx);
        return VPU_ERR_INITMEMFAIL;
    }

    return VPU_ERR_NONE;
}

static void
cleanuptextsys(VideoSys *vctx)
{
    if (!vctx)
        return;

    free(vctx->disp.txt.params);
    free(vctx->disp.txt.charmem);

    vctx->disp.txt.params    = NULL;
    vctx->disp.txt.charmem   = NULL;
}

static void
initfpstimer(VideoSys *vctx, int fpslimit)
{
    vctx->fpsctx.prevtick  = SDL_GetTicks();
    vctx->fpsctx.tickInterval = 1000/fpslimit;
}
