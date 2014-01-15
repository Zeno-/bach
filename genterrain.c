#include "vpu/backend/config.h"

#if VPU_BUILD_BACKENDTEST2 == 1

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <unistd.h>

#include "vpu/backend/base.h"
#include "vpu/backend/text.h"
#include "vpu/backend/config.h"
#include "vpu/fonts/bmfonts.h"

#define OUTPUTCOLOURED      1
#define OUTPUTSMOOTHEDNOISE 1

static void show(double *a, int w, int h);
static void genterrainmap(void);

int main(int argc, char **argv)
{
#if !defined _WIN32
    (void)argc; /* UNUSED */
    (void)argv; /* UNUSED */
#endif

    if (vpu_init(1024, 768, 0, &vidfont8x8)
            != VPU_ERR_NONE) {
        fputs("Could not init VPU\n", stderr);
        exit(1);
    }

    //srand(time(NULL));

    genterrainmap();

    //sleep(2);
    //getchar();

    return 0;
}



static double *genrandarr(int w, int h)
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

static double interpolate(double x0, double x1, double alpha)
{
    return x0 * (1.0 - alpha) + alpha * x1;
}

static double *gensmoothnoise(double *a, int w, int h, double *basenoise, unsigned oct)
{
    int i, j;

    unsigned long sampleperiod;
    double samplefreq;

    sampleperiod = 1 << oct;
    samplefreq = 1.0 / sampleperiod;

    for (i = 0; i < w; i++) {
        int sample_i0 = (int)(((double)i / sampleperiod)) * sampleperiod;
        int sample_i1 = (sample_i0 + sampleperiod) % w;
        double hblend = (i - sample_i0) * samplefreq;
        for (j = 0; j < h; j++) {
            int sample_j0 = (int)(((double)j / sampleperiod)) * sampleperiod;
            int sample_j1 = (sample_j0 + sampleperiod) % h;
            double vblend = (j - sample_j0) * samplefreq;

            double top = interpolate(*(basenoise + sample_i0 + sample_j0 * w),
                                     *(basenoise + sample_i1 + sample_j0 * w),
                                     hblend);
            double bottom = interpolate(*(basenoise + sample_i0 + sample_j1 * w),
                                       *(basenoise + sample_i1 + sample_j1 * w),
                                       hblend);

            *(a + i + j * w) = interpolate(top, bottom, vblend);
        }
    }

#if OUTPUTSMOOTHEDNOISE == 1
    show(a, w, h);
    sleep(1);
#endif

    return a;
}

static double *GeneratePerlinNoise(int w, int h, double *basenoise, int octaveCount)
{

    int i, j, oct, n;
    double persistance = .7;
    double *smoothNoise;
    double *perlinNoise;

    n = w * h;

    if ( (smoothNoise = malloc(n * sizeof *smoothNoise)) == NULL) {
        perror("genrandr()");
        exit(1);
    }

    if ( (perlinNoise = calloc(n, sizeof *perlinNoise)) == NULL) {
        perror("genrandr()");
        exit(1);
    }

    double amplitude = 1.0;
    double totalAmplitude = 0.0;

    //generate smooth noise
    for (oct = octaveCount - 1; oct >= 0; oct--)
    {
        amplitude *= persistance;
        totalAmplitude += amplitude;
        gensmoothnoise(smoothNoise, w, h, basenoise, oct);
        for (i = 0; i < w; i++) {
            for (j = 0; j < h; j++) {
                *(perlinNoise + i + j * w) += *(smoothNoise + i + j * w) * amplitude;
            }
        }
    }

    //normalisation
    for (i = 0; i < w; i++) {
        for (j = 0; j < h; j++) {
            *(perlinNoise + i + j * w) /= totalAmplitude;
            if (*(perlinNoise + i + j * w) > 1)
                printf("%lf\n", *(perlinNoise + i + j * w) );
        }
    }

    free (smoothNoise);

    return perlinNoise;
}

static void show(double *a, int w, int h)
{
    int r, c, i;
    uint32_t colour;
    char ch;

    vpu_settextattr(vpu_textattr() | VPU_TXTATTRIB_REVERSE);

    i = 0;
    for (r = 0; r < h; r++) {
        for (c = 0; c < w; c++) {
#if OUTPUTCOLOURED == 1
            if (a[i] < 132/255.0) {
                if (a[i] > 125/255.0) {
                    vpu_settextbg(0x00ffffff);
                    ch =  '.';
                    colour = vpu_rgbto32(a[i]*255*0.90, a[i]*255*0.90, a[i]*255/1.5);
                } else {
                    vpu_settextbg(0x0000000);
                    ch =  ' ';
                    colour = vpu_rgbto32(0, a[i]*.25*255, a[i]*255);
                }
            } else if (a[i] < 135/255.0) {
                vpu_settextbg(0x00);
                colour = vpu_rgbto32(0, a[i]*255, 0);
                ch = ' ';
            } else {
                vpu_settextbg(0x00);
                colour = vpu_rgbto32(a[i]*0.95, a[i]*255, 0);
                ch = '^';
            }
#else
            colour = vpu_rgbto32(a[i]*255, a[i]*255, a[i]*255);
            ch = ' ';
#endif
            i++;
            vpu_putcharat_c(ch, c, r, colour);
        }
    }

    vpu_refresh(VPU_REFRESH_FORCE);
}

static void genterrainmap(void)
{
    struct display *scr;
    int w, h;

    double *basenoise, *pnoise;

    scr = vpu_getinstance();
    w = scr->txt.cols;
    h = scr->txt.rows;

    basenoise = genrandarr(w, h);
    pnoise = GeneratePerlinNoise(w, h, basenoise, 8);

    show(pnoise, w, h);
    sleep(2);

    free(basenoise);
    free(pnoise);
}

#endif /* VPU_BUILD_BACKENDTEST2 */
