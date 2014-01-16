TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    vpu/backend/base.c \
    vpu/backend/text.c \
    vpu/fonts/bmfonts.c \
    vpu/backend/render.c \
    vpu/tests/genterrain.c \
    vpu/tests/test.c \

HEADERS += \
    common/warn.h \
    vpu/backend/base.h \
    vpu/backend/colour.h \
    vpu/backend/config.h \
    vpu/backend/private.h \
    vpu/backend/text.h \
    vpu/fonts/bmfonts.h \
    vpu/backend/render.h

LIBS += -lSDL
