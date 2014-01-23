#ifndef VIDEO_HAL_REFS_H
#define VIDEO_HAL_REFS_H

#include <stdint.h>

extern struct display *vpu_instance;
extern struct txtlayer *vpu_txtlayer;

extern uint32_t *vpu_pixelmem;
extern uint8_t  *vpu_tl_charemem;
extern uint32_t *vpu_tl_paramsmem;
extern uint32_t *vpu_tl_fgcolormem;
extern uint32_t *vpu_tl_bgcolormem;
extern uint32_t *vpu_tl_attrmem;

#endif /* VIDEO_HAL_REFS_H */
