#ifndef BACH_CONFIG_MASTER_H
#define BACH_CONFIG_MASTER_H

#define DEFAULT_PROG_TITLE  \
    "BACH - BAsic ArCHitecture Emulator"
#define DEFAULT_PROG_NAME \
    "bach"

enum {
    HAL_BACKEND_SDL
};

#define HAL_INTERFACE   HAL_BACKEND_SDL
#define DEFAULT_RUNMODE RUNMODE_LUA

#endif /* BACH_CONFIG_MASTER_H */
