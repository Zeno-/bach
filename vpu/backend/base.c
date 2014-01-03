#include <stdlib.h>
#include <stdint.h>
#include <time.h>

#include <SDL/SDL.h>
#include <SDL/SDL_main.h>

#include "vpu/fonts/bmfonts.h"
#include "base.h"
#include "private.h"
#include "config.h"
#include "render.h"

#define MAX_BACKENDSTRLEN 128

struct fpsctx {
    uint32_t prevtick;
    uint16_t tickInterval;
};

struct display vpu_prv;
struct display_privdata vpu_pdata_prv;
static struct fpsctx fpstimer;
static char backendstr[MAX_BACKENDSTRLEN];

static enum vpuerror initsys(void);
static enum vpuerror initscr(unsigned w, unsigned h, int fullscreen);
static void setbackendinfo(void);
static void setfixedfont(struct display *screen,
                         const struct vidfont8 *font);

static enum vpuerror inittextsys(void);
static void cleanuptextsys(void);
static void initfpstimer(int fpslimit);

/**************************************************************************
 * Public
 *************************************************************************/

enum vpuerror
vpu_init(unsigned w, unsigned h, int fullscreen)
{
    enum vpuerror err;

    if ((err = initsys()) != VPU_ERR_NONE)
        return err;
    if ((err = initscr(w, h, fullscreen)) != VPU_ERR_NONE)
        return err;

    /* If the basic VPU system is running, then it has to be cleaned up
     * at exit; other init functions after this point may fail, but the
     * basic cleanup for those before this line must be done, so add the
     * atexit() now.
     */
    atexit(vpu_cleanup);

    initfpstimer(VPU_FPSLIMIT);

    if ((err = inittextsys() != VPU_ERR_NONE))
        return err;

    setbackendinfo();

    return VPU_ERR_NONE;
}

void
vpu_cleanup(void)
{
    cleanuptextsys();
    SDL_Quit();
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
vpu_refresh(int force)
{
    if (!force && !vpu_shouldrefresh())
        return;
    vpu_refresh_tlayer();
    SDL_Flip(VID_PRV_SURFACE);
}

void
vpu_clrdisplay(void)
{
    SDL_Surface *surface = VID_PRV_SURFACE;

    // FIXME: the background colour should be modifiable
    SDL_FillRect(surface, NULL, DEFAULT_BGCOLOUR);
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

static enum vpuerror
initsys(void)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        return VPU_ERR_INITFAIL;

    return VPU_ERR_NONE;
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
    vpu_prv.txt.fgcolour = DEFAULT_TEXTFG;
    vpu_prv.txt.bgcolour = DEFAULT_TEXTBG;

    return VPU_ERR_NONE;
}

static void
setbackendinfo(void)
{

    SDL_version compiled;

    SDL_VERSION(&compiled);

    snprintf(backendstr, MAX_BACKENDSTRLEN,
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
    screen->txt.cnum = screen->txt.cols * screen->txt.rows;
    /* Centre text "window" on screen; i.e. dertemine upper-left pixel of
     * the text display
     */
    extra_xpx = (screen->w % screen->txt.cols) / 2;
    extra_ypx = (screen->h % screen->txt.rows) / 2;
    screen->txt.origin = extra_xpx + extra_ypx * screen->w;
}

static enum vpuerror
inittextsys(void)
{
    unsigned ccount;
    unsigned ok = 1;

    vpu_prv.txt.flags = VPU_TXTAUTOSCROLL | VPU_TXTLAYERVISIBLE;

    setfixedfont(&vpu_prv, &DEFAULT_CHFONT);

    ccount = vpu_prv.txt.cnum;

    ok &= ( vpu_prv.txt.mem
            = calloc(ccount, sizeof (*vpu_prv.txt.mem)) ) != NULL;
    ok &= ( vpu_prv.txt.attribs
            = calloc(ccount, sizeof *vpu_prv.txt.attribs)) != NULL;

    ok &= ( vpu_prv.txt.colours
            = malloc(ccount * sizeof *vpu_prv.txt.colours)) != NULL;

    if (ok) {
        memset(vpu_prv.txt.colours, DEFAULT_TEXTBG, vpu_prv.txt.cnum);
    }
    else {
        cleanuptextsys();
        return VPU_ERR_INITMEMFAIL;
    }

    return VPU_ERR_NONE;
}

static void
cleanuptextsys(void)
{
    free(vpu_prv.txt.mem);
    free(vpu_prv.txt.colours);
    free(vpu_prv.txt.attribs);

    vpu_prv.txt.mem = NULL;
    vpu_prv.txt.colours = NULL;
    vpu_prv.txt.attribs = NULL;
}

static void
initfpstimer(int fpslimit)
{
    fpstimer.prevtick  = SDL_GetTicks();
    fpstimer.tickInterval = 1000/fpslimit;
}
