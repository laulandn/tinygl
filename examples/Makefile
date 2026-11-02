
#CC=m68k-apple-macos-gcc
#CXX=m68k-apple-macos-g++
#CC=powerpc-apple-macos-gcc
#CXX=powerpc-apple-macos-g++


#CFLAGS=-I../include -I.. -I$(HOME)/Retro68-build/toolchain/m68k-apple-macos/include/SDL2 -DSTBI_NO_THREAD_LOCALS
#CFLAGS += -I$(HOME)/sdl2macos9/dummy_dir/.

CFLAGS=-I../include -I.. -I/opt/local/include -I/opt/local/include/SDL2 -I../../../stb

SDLMAIN= -lSDL2main

LDFLAGS= -L.. $(SDLMAIN) -lSDL2 -L/opt/local/lib
LDFLAGS+= -lglu -lTinyGL -laglminisdl2 -lm
#LDFLAGS=-L.. -lglu -lTinyGL -L/opt/local/lib -lSDL2


%.o : %.c
#echo Compiling $<
	$(CC) -o $@ -c $< $(CFLAGS)
	

all:	bounce cube gears mech morph3d spin teapot


bounce:	bounce.o ui_sdl2.o
	$(CC) -o bounce bounce.o ui_sdl2.o $(LDFLAGS) 
#	../bin2app.sh bounce

cube:	cube.o
	$(CC) -o cube cube.o ui_sdl2.o $(LDFLAGS)
#	../bin2app.sh cube

gears:	gears.o
	$(CC) -o gears gears.o ui_sdl2.o $(LDFLAGS)
#	../bin2app.sh gears

mech:	mech.o
	$(CC) -o mech mech.o ui_sdl2.o $(LDFLAGS)
#	../bin2app.sh mech

morph3d:	morph3d.o
	$(CC) -o morph3d morph3d.o ui_sdl2.o $(LDFLAGS)
#	../bin2app.sh morph3d

spin:	spin.o
	$(CC) -o spin spin.o ui_sdl2.o $(LDFLAGS)
#	../bin2app.sh spin

teapot:	teapot.o
	$(CC) -o teapot teapot.o ui_sdl2.o $(LDFLAGS)
#	../bin2app.sh teapot


clean:
	rm -rf bounce cube gears mech morph3d spin teapot *.o *.APPL *.bin *.gdb *.dsk *.pef

