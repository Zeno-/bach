:: Thanks to RangerMauve (from irc freenode) for initial version

gcc -o zenocpu.exe -I. -IJ:/SDL-1.2.15/include vpu/tests/test.c vpu/tests/genterrain.c vpu/backend/base.c vpu/backend/render.c vpu/backend/text.c vpu/fonts/bmfonts.c -lmingw32 -lSDLmain -lSDL
