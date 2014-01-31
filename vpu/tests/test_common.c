#include "vpu/config.h"

#include <stdio.h>
#include "test_common.h"
#include "hal/hal.h"
#include "vpu/video.h"

#if VPU_BUILDTESTS

void vput_test_initall(void)
{
    if (hal_init() != HAL_NOERROR) {
        fputs("Inititialisation of HAL failed.\n", stderr);
        exit(1);
    }

    if (vpu_init(SCREEN_PIXELS_X, SCREEN_PIXELS_Y, 0, NULL)
            != VPU_ERR_NONE) {
        fputs("Could not init VPU\n", stderr);
        exit(1);
    }

}

#endif /* #if VPU_BUILDTESTS */
