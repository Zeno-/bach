#ifndef BACH_COMMON_WARN_H
#define BACH_COMMON_WARN_H

#include <stdio.h>
#define WARN(s) \
    fprintf(stderr, "Warning: %s:%d: %s\n", __FILE__, __LINE__, (s))

#endif /* BACH_COMMON_WARN_H */
