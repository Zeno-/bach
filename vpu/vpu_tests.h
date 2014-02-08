#ifndef BACH_VPUTESTS_HEADER
#define BACH_VPUTESTS_HEADER

/* 0 = Build no tests
 * 1 = Build test as defined by VPU_BUILDTEST_*
 */
#define VPU_BUILDTESTS              1
#define VPU_RUNTESTS                (VPU_BUILDTESTS)

#define VPU_BUILDTEST_BASICCOLOURS  1
#define VPU_BUILDTEST_SCROLL        0
#define VPU_BUILDTEST_TERRAIN       1

#define VPU_RUNTEST_BASICCOLOURS    (VPU_BUILDTEST_BASICCOLOURS)
#define VPU_RUNTEST_SCROLL          (VPU_BUILDTEST_SCROLL)
#define VPU_RUNTEST_TERRAIN         (VPU_BUILDTEST_TERRAIN)


void vputest_genterrain(void);
void vputest_basiccolours(void);

#endif
