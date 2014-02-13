TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    main.c \
    machine.c \
    vpu/fonts/bmfonts.c \
    vpu/render.c \
    vpu/text.c \
    vpu/tests/test_basiccolours.c \
    vpu/tests/test_scroll.c \
    vpu/tests/test_genterrain.c \
    hal/sdl/hal_init.c \
    hal/sdl/hal_timer.c \
    hal/sdl/video.c \
    hal/sdl/hal_events.c \
    lua/lua_iface.c


HEADERS += \
    machine.h \
    config_master.h \
    common/warn.h \
    common/macros.h \
    vpu/fonts/bmfonts.h \
    vpu/backend/render.h \
    vpu/backend/video.h \
    vpu/colour.h \
    vpu/config.h \
    vpu/render.h \
    vpu/text.h \
    vpu/video.h \
    vpu/vpu_tests.h \
    hal/sdl/hal.h \
    hal/sdl/hal_init.h \
    hal/sdl/hal_kybdmouse.h \
    hal/sdl/hasl_timer.h \
    hal/sdl/hal_video_refs.h \
    hal/sdl/hal_events.h \
    lua/lua_iface.h \
    lua/lua_config.h \
    cpu/cpu_config.h \
    cpu/opcodes.h

!win32 {
    LIBS += -lSDL -llua
}

# Obviously not great! If somebody on Windows wants to set things up
# so that absolute paths are not used then this should be fixed.
#
win32 {
    INCLUDEPATH += J:/SDL-1.2.15/include
    LIBS += -LJ:/SDL-1.2.15/lib -lmingw32 -lSDLmain -lSDL -llua
}

OTHER_FILES += \
    hal/README
