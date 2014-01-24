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

#define MAX_VERSIONINFO_LEN 128

#define VID_PRV_SURFACE     (vpu_pdata_prv.vidsurface)

struct fpsctx {
    uint32_t prevtick;
    uint16_t tickInterval;
};

struct display vpu_prv;
struct display_privdata vpu_pdata_prv;

static struct fpsctx fpstimer;
static char backendstr[MAX_VERSIONINFO_LEN];

static void initexports(void);
static enum vpuerror initscr(unsigned w, unsigned h, int fullscreen);
static void setbackendinfo(void);
static void setfixedfont(struct display *screen,
                         const struct vidfont8 *font);

static enum vpuerror inittextsys(const struct vidfont8 *font);
static void cleanuptextsys(void);
static void initfpstimer(int fpslimit);

/**************************************************************************
 * Public exported refs/pointers
 *************************************************************************/

static struct vpu_refs vpurefs_p;
const struct vpu_refs *vpurefs = &vpurefs_p;

/**************************************************************************
 * Public
 *************************************************************************/

enum vpuerror
vpu_init(unsigned w, unsigned h, int fullscreen,
         const struct vidfont8 *font)
{
    enum vpuerror err;

    if (!hal_isinitalised()) {
        fputs("Error: Hardware Abstraction Layer not initialised.\n",
              stderr);
        return VPU_ERR_INITFAIL;
    }

    if ((err = initscr(w, h, fullscreen)) != VPU_ERR_NONE)
        return err;

    /* If the basic VPU system is running, then it has to be cleaned up
     * at exit; other init functions after this point may fail, but the
     * basic cleanup for those before this line must be done, so add the
     * atexit() now.
     */
    atexit(vpu_cleanup);

    initfpstimer(VPU_FPSLIMIT);

    if ((err = inittextsys(font) != VPU_ERR_NONE))
        return err;

    initexports();
    vpu_clrtext();

    setbackendinfo();

    return VPU_ERR_NONE;
}

void
vpu_cleanup(void)
{
    cleanuptextsys();
}

const char *
vpu_backendinfostr(void)
{
    return backendstr;
}

struct display
*vpu_getinstance(void)
{
    return &vpu_prv;
}

int
vpu_shouldrefresh(void)
{
    uint32_t currtick, expected;
    struct fpsctx *t;

    t = &fpstimer;

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
vpu_refresh(enum vpu_refreshaction action)
{
    if (!vpu_shouldrefresh()
            && action != VPU_REFRESH_FORCE
            && action != VPU_REFRESH_COMMITONLY)
        return;
    vpu_refresh_tlayer();
    if (action != VPU_REFRESH_COMMITONLY)
        SDL_Flip(VID_PRV_SURFACE);
}

void
vpu_clrdisplay(void)
{
    SDL_Surface *surface = VID_PRV_SURFACE;

    // FIXME: the background colour should be modifiable
    SDL_FillRect(surface, NULL, VGFX_DEF_BGCOLOUR);
}

void
vpu_clrtext(void)
{
    memset(vpurefs->txt_charmem, 0, vpurefs->txtlayer->charmem_sz);
    memset(vpurefs->txt_fgcolormem,
           vpurefs->txtlayer->fgcolour,
           sizeof vpurefs->txtlayer->fgcolour * vpurefs->txtlayer->cnum);
    memset(vpurefs->txt_bgcolormem,
           vpurefs->txtlayer->bgcolour,
           sizeof vpurefs->txtlayer->bgcolour * vpurefs->txtlayer->cnum);
    memset(vpurefs->txt_attrmem,
           vpurefs->txtlayer->attrib,
           sizeof vpurefs->txtlayer->attrib * vpurefs->txtlayer->cnum);
}

void vpu_direct_write_start(void)
{
    SDL_LockSurface(VID_PRV_SURFACE);
}

void vpu_direct_write_end(void)
{
  SDL_UnlockSurface(VID_PRV_SURFACE);
}

uint32_t
vpu_rgbto32(unsigned char r, unsigned char g, unsigned char b)
{
    SDL_Surface *surface = VID_PRV_SURFACE;
    return SDL_MapRGB(surface->format, r, g, b);
}

/**************************************************************************
 * Private
 *************************************************************************/

static void
initexports(void)
{
    vpurefs_p.instance          = &vpu_prv;
    vpurefs_p.txtlayer          = &vpu_prv.txt;

    vpurefs_p.pixelmem          = vpu_pdata_prv.vpixels;
    vpurefs_p.txt_charmem       = vpu_prv.txt.charmem;
    vpurefs_p.txt_paramsmem     = vpu_prv.txt.params;
    vpurefs_p.txt_fgcolormem    = vpu_prv.txt.params;
    vpurefs_p.txt_bgcolormem    = vpu_prv.txt.params + vpu_prv.txt.cnum;
    vpurefs_p.txt_attrmem       = vpu_prv.txt.params + vpu_prv.txt.cnum * 2;
}

static enum vpuerror
initscr(unsigned w, unsigned h, int fullscreen)
{
    SDL_Surface* surface;
    unsigned flags;

    /* Make sure width and height are multiples of 8 */
    w = (w >> 3) << 3;
    h = (h >> 3) << 3;

    flags = SDL_HWSURFACE | SDL_DOUBLEBUF | (fullscreen && SDL_FULLSCREEN);
    if ((surface = SDL_SetVideoMode(w, h, 32, flags)) == NULL)
        return VPU_ERR_VMODEFAIL;

    vpu_prv.pdata = &vpu_pdata_prv;
    vpu_pdata_prv.vidsurface = surface;
    vpu_pdata_prv.vpixels = surface->pixels;

    vpu_prv.w = w;
    vpu_prv.h = h;
    vpu_prv.txt.fgcolour = VTXT_DEF_FGCOLOUR;
    vpu_prv.txt.bgcolour = VTXT_DEF_BGCOLOUR;
    vpu_prv.txt.attrib   = 0;

    return VPU_ERR_NONE;
}

static void
setbackendinfo(void)
{

    SDL_version compiled;

    SDL_VERSION(&compiled);

    snprintf(backendstr, MAX_VERSIONINFO_LEN,
             "Backend using SDL %u.%u.%u",
             compiled.major, compiled.minor, compiled.patch);
}

static void
setfixedfont(struct display *screen, const struct vidfont8 *font)
{
    uint32_t extra_xpx, extra_ypx;

    screen->fixedfont = font;
    screen->txt.cols = screen->w / VPU_FIXED_FONT_WIDTH;
    screen->txt.rows = screen->h / screen->fixedfont->height;
    screen->txt.charmem_sz = screen->txt.cols * screen->txt.rows;
    screen->txt.cnum = screen->txt.charmem_sz;
    screen->txt.params_sz = screen->txt.cnum * 3;

    /* Centre text "window" on screen; i.e. dertemine upper-left pixel of
     * the text display
     */
    extra_xpx = (screen->w % screen->txt.cols) / 2;
    extra_ypx = (screen->h % screen->txt.rows) / 2;
    screen->txt.origin = extra_xpx + extra_ypx * screen->w;
}

static enum vpuerror
inittextsys(const struct vidfont8 *font)
{
    size_t ccount, pmemreq;
    unsigned ok;

    vpu_prv.txt.flags = VTXT_DEF_TEXTFLAGS;

    setfixedfont(&vpu_prv, font != NULL ? font : &DEFAULT_CHFONT);

    ccount  = vpu_prv.txt.charmem_sz;
    pmemreq = vpu_prv.txt.params_sz * sizeof *vpu_prv.txt.params;

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

    ok  = ( vpu_prv.txt.charmem
            = malloc(ccount * sizeof *vpu_prv.txt.charmem) ) != NULL;
    ok &= ( vpu_prv.txt.params = malloc(pmemreq) ) != NULL;

    if (!ok) {
        cleanuptextsys();
        return VPU_ERR_INITMEMFAIL;
    }

    return VPU_ERR_NONE;
}

static void
cleanuptextsys(void)
{
    free(vpu_prv.txt.params);
    free(vpu_prv.txt.charmem);

    vpu_prv.txt.params    = NULL;
    vpu_prv.txt.charmem   = NULL;
}

static void
initfpstimer(int fpslimit)
{
    fpstimer.prevtick  = SDL_GetTicks();
    fpstimer.tickInterval = 1000/fpslimit;
}
