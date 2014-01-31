#ifndef ZPU_COMMON_WARN_H
#define ZPU_COMMON_WARN_H

#include <stdio.h>
#define WARN(s) \
    fprintf(stderr, "Warning: %s:%d: %s\n", __FILE__, __LINE__, (s))

#endif /* ZPU_COMMON_WARN_H */
