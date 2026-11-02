
#CC=m68k-apple-macos-gcc
#CXX=m68k-apple-macos-g++
#CC=powerpc-apple-macos-gcc
#CXX=powerpc-apple-macos-g++

CFLAGS= -I../../include
LDFLAGS=-L../.. -L/opt/local/lib -lglut -lTinyOpenGLUtility -lTinyOpenGLLibrary -lm

OBJS=$(subst .c,.o,$(wildcard *.c)) $(subst .cpp,.o,$(wildcard *.cpp))

.cpp.o:
	$(CXX) $(CFLAGS) -c -o $@ $<

.c.o:
	$(CXX) $(CFLAGS) -c -o $@ $<

all: $(OBJS)
	$(CXX) -o glutmech $(OBJS) $(LDFLAGS)
#	../bin2app.sh glutmech
#	$(CC) -o glutmech $(OBJS) $(LDFLAGS)
#	$(HOME)/sdl2macos9/xcoff2app.sh glutmech

clean:
	rm -f *.o *.bin *.pef *.APPL *.dsk *.gdb glutmech
