#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

#include "vpu/backend/base.h"
#include "vpu/backend/text.h"
#include "vpu/backend/config.h"
#include "vpu/fonts/bmfonts.h"

int main(int argc, char **argv)
{
#if !defined _WIN32
    (void)argc; /* UNUSED */
    (void)argv; /* UNUSED */
#endif

    if (vpu_init(800, 600, 0, &vidfont8x8)
            != VPU_ERR_NONE) {
        fputs("Could not init VPU\n", stderr);
        exit(1);
    }

    sleep(2);

    return 0;
}
