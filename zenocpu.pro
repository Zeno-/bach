TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    timer/timer.c \
    hal/video_hal.c \
    vpu/fonts/bmfonts.c \
    vpu/render.c \
    vpu/text.c \
    vpu/video.c \
    vpu/tests/test_basiccolours.c \
    vpu/tests/test_scroll.c \
    vpu/tests/test_genterrain.c

HEADERS += \
    timer/timer.h \
    hal/video_hal.h \
    hal/video_hal_private.h \
    vpu/fonts/bmfonts.h \
    vpu/backend/render.h \
    vpu/backend/video.h \
    vpu/colour.h \
    vpu/config.h \
    vpu/render.h \
    vpu/text.h \
    vpu/video.h \
    hal/video_hal_refs.h

LIBS += -lSDL

# Obviously not great! If somebody on Windows wants to set things up
# so that absolute paths are not used then this should be fixed.
#
win32 {
    INCLUDEPATH += J:/SDL-1.2.15/include
    LIBS += -LJ:/SDL-1.2.15/lib -lmingw32 -lSDLmain -lSDL
}

OTHER_FILES += \
    hal/README
