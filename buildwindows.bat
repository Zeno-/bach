:: Thanks to RangerMauve (from irc freenode) for compiling on 
:: Windows, testing that it works on that platform and  giving
:: me hints

gcc -o zenocpu.exe -I. vpu/test.c vpu/backend/base.c vpu/backend/render.c vpu/backend/text.c vpu/fonts/bmfonts.c -lmingw32 -lSDLmain -lSDL
