#include "vpu/config.h"
#include "machine.h"

#if VPU_BUILDTESTS == 1 && VPU_BUILDTEST_TERRAIN == 1

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <unistd.h>

#include "vpu/video.h"
#include "vpu/text.h"
#include "vpu/config.h"
#include "vpu/fonts/bmfonts.h"

#define OUTPUTCOLOURED      1
#define OUTPUTSMOOTHEDNOISE 0

/* Round 'n' down to 'd' */
#define INTCLAMPTO(n,d) ( ((int)((double)(n) / (d))) * (d) )

static void show(VideoSys *vctx, double *a, int w, int h);
static void genterrainmap(VideoSys *vctx, int w, int h);
static double *genperlinnoise(VideoSys *vctx, int w, int h, int octaves);
static double *genrandarr(int w, int h);
static double *gensmoothnoise(VideoSys *vctx,
                              double *a, int w, int h,
                              double *basenoise, unsigned octave);
static void show(VideoSys *vctx, double *a, int w, int h);
static double interpolate(double x0, double x1, double alpha);

void vputest_genterrain(void)
{
    struct machine *mctx;
    struct event e;
    int i;

    if (hal_init() != HAL_NOERROR) {
       fputs("Inititialisation of HAL failed.\n", stderr);
       exit(1);
    }

    struct machine_config cfg = {
       1024, 768, 0, &vidfont8x8
    };
    mctx = machine_poweron(&cfg);

    srand(time(NULL));

    for (i = 0; i < 10; i++) {
        genterrainmap(mctx->vsys,
                      mctx->vsys->refs.txtlayer->cols,
                      mctx->vsys->refs.txtlayer->rows);
        sleep(1);

        if (evsys_poll(mctx->esys, &e, EQ_POLL_NONBLOCKING))
            if (e.type & EVENT_QUIT)
                break;

    }
    //getchar();
    machine_poweroff(mctx);
}


static void
genterrainmap(VideoSys *vctx, int w, int h)
{
    double *pnoise;

    pnoise = genperlinnoise(vctx, w, h, 7);
    show(vctx, pnoise, w, h);
    free(pnoise);
}

static double *
genperlinnoise(VideoSys *vctx, int w, int h, int octaves)
{
    const double persistance    = 0.7;

    int i, j, oct;
    int arrsize;
    double amplitude            = 1.0;
    double totalAmplitude       = 0.0;

    double *bnoise, *snoise, *pnoise;

    arrsize = w * h;

    bnoise = genrandarr(w, h);  /* "base" noise */

    if ( (snoise = malloc(arrsize * sizeof *snoise)) == NULL) {
        perror("malloc() failed for smoothnoise");
        exit(1);
    }

    if ( (pnoise = calloc(arrsize, sizeof *pnoise)) == NULL) {
        perror("malloc() failed for perlinnoise");
        exit(1);
    }

    /* Combine smooth noise */
    for (oct = octaves - 1; oct >= 0; oct--)
    {
        gensmoothnoise(vctx, snoise, w, h, bnoise, oct);
        for (i = 0; i < w; i++) {
            for (j = 0; j < h; j++) {
                *(pnoise + i + j * w) +=
                        *(snoise + i + j * w) * amplitude;
            }
        }
        totalAmplitude += amplitude;
        amplitude *= persistance;
    }

    /* TODO: Incorporate normalization into the above loop */

    /* Normalise */
    for (i = 0; i < w; i++) {
        for (j = 0; j < h; j++) {
            *(pnoise + i + j * w) /= totalAmplitude;
            /* if (*(perlinNoise + i + j * w) > 1)
                printf("%lf\n", *(perlinNoise + i + j * w) ); */
        }
    }

    free(snoise);
    free(bnoise);

    return pnoise;
}

static double *
genrandarr(int w, int h)
{
    double *ra;
    int i, n;

    n = w * h;
    if ( (ra = malloc(n * sizeof *ra)) == NULL) {
        perror("genrandr()");
        exit(1);
    }

    for (i = 0; i < n; i++)
        ra[i] = (double)rand() / RAND_MAX;

    return ra;
}

static double *
gensmoothnoise(VideoSys *vctx, double *a, int w, int h,
               double *basenoise, unsigned octave)
{
    int i, j;

    unsigned long period;
    double freq;
    int sample_i0, sample_i1, sample_j0, sample_j1;
    double hblend, vblend, top, bottom;

    period = 1 << octave;
    freq = 1.0 / period;

    for (i = 0; i < w; i++) {
        sample_i0 = INTCLAMPTO(i, period);
        sample_i1 = (sample_i0 + period) % w;
        hblend = (i - sample_i0) * freq;
        for (j = 0; j < h; j++) {
            sample_j0 = INTCLAMPTO(j, period);
            sample_j1 = (sample_j0 + period) % h;
            vblend = (j - sample_j0) * freq;

            top = interpolate(*(basenoise + sample_i0 + sample_j0 * w),
                              *(basenoise + sample_i1 + sample_j0 * w),
                              hblend);
            bottom = interpolate(*(basenoise + sample_i0 + sample_j1 * w),
                                 *(basenoise + sample_i1 + sample_j1 * w),
                                 hblend);

            *(a + i + j * w) = interpolate(top, bottom, vblend);
        }
    }

#if OUTPUTSMOOTHEDNOISE == 1
    show(vctx, a, w, h);
    sleep(1);
#else
    (void)vctx;
#endif

    return a;
}

static void
show(VideoSys *vctx, double *a, int w, int h)
{
    int r, c, i;
    uint32_t colour;
    char ch;

    vpu_settextattr(vctx, vpu_textattr(vctx) | VPU_TXTATTRIB_REVERSE);

    i = 0;
    for (r = 0; r < h; r++) {
        for (c = 0; c < w; c++) {
#if OUTPUTCOLOURED == 1
            if (a[i] < 132 / 255.0) {
                if (a[i] > 125 / 255.0) {
                    vpu_settextbg(vctx, 0x00606060);
                    ch =  '.';
                    colour = vpu_rgbto32(vctx,
                                         a[i] * 255 * 0.95,
                                         a[i] * 255,
                                         a[i] * 255 * 0.20);
                } else {
                    vpu_settextbg(vctx, 0x00000000);
                    ch =  ' ';
                    colour = vpu_rgbto32(vctx,
                                         0,
                                         a[i] * 255 * 0.25,
                                         a[i] * 255);
                }
            } else if (a[i] < 135 / 255.0) {
                vpu_settextbg(vctx, 0x00000000);
                colour = vpu_rgbto32(vctx,
                                     0,
                                     a[i] * 255,
                                     0);
                ch = '\'';
            } else {
                vpu_settextbg(vctx, 0x00000000);
                colour = vpu_rgbto32(vctx,
                                     0,
                                     a[i] * 255,
                                     0);
                ch = '"';
            }
#else
            colour = vpu_rgbto32(vctx, a[i] * 255, a[i] * 255, a[i] * 255);
            ch = ' ';
#endif
            i++;
            vpu_putcharat_c(vctx, ch, c, r, colour);
        }
    }

    vpu_refresh(vctx, VPU_REFRESH_FORCE);
}


inline static double
interpolate(double x0, double x1, double alpha)
{
    return x0 * (1.0 - alpha) + alpha * x1;
}

#endif /* VPU_BUILD_BACKENDTEST2 */
