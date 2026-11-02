LIBNAME=aglmini

#CC=m68k-apple-macos-gcc
#CXX=m68k-apple-macos-g++
#AR=m68k-apple-macos-ar
#RANLIB=m68k-apple-macos-ranlib
#CC=powerpc-apple-macos-gcc
#CXX=powerpc-apple-macos-g++
#AR=powerpc-apple-macos-ar
RANLIB=ranlib


CFLAGS= -I. -I../include -I../include/GL

CFLAGS+= -DSDL_MACOSCLASSIC_TINYGL=1
#CFLAGS+= -DUSING_SDL2
CFLAGS+=-DNO_COLOR_QUICKDRAW=1


OBJS=$(subst .c,.o,$(wildcard *.c))


.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<


all: $(OBJS)
	rm -f *.a
	$(AR) rcv lib$(LIBNAME).a $(OBJS)
	$(RANLIB) lib$(LIBNAME).a	

clean:
	rm -f *.o */*.o *.a