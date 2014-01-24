#ifndef INIT_HAL_H
#define INIT_HAL_H

enum hal_error {
    HAL_NOERROR,
    HAL_INIT_FAIL,
    HAL_INIT_VIDEO_FAIL,
    HAL_INIT_KYBDMOUSE_FAIL
};

enum hal_error  hal_init(void);
int             hal_isinitalised(void);

#endif /* INIT_HAL_H */
