#include "vpu/config.h"

#include <stdio.h>
#include "test_common.h"
#include "machine.h"

#if VPU_BUILDTESTS

struct machine *
vput_test_initall(void)
{
    struct machine *m;
    struct machine_config cfg = {
       SCREEN_PIXELS_X, SCREEN_PIXELS_Y, 0, NULL
    };

    m = machine_poweron(&cfg);

    return m;
}

#endif /* #if VPU_BUILDTESTS */
