include config.mk

all:
#	( for f in $(DIRS); do ( cd $$f ; make -f Makefile all ) || exit 1 ; done )
	rm -f *.a
	make  -C source -f Makefile
	make  -C agl-mini -f Makefile clean
	make  -C agl-mini -f Makefilesdl2
	cp agl-mini/*.a .
	$(AR) rcv libTinyOpenGLLibrarySDL2.a source/*.o agl-mini/*.o
	$(RANLIB) libTinyOpenGLLibrarySDL2.a
	make  -C agl-mini -f Makefile clean
	make  -C agl-mini -f Makefile
	$(AR) rcv libTinyOpenGLLibrary.a source/*.o agl-mini/*.o
	$(RANLIB) libTinyOpenGLLibrary.a
	make  -i -C agl-soft -f Makefile
	make  -i -C GLUT_apple -f Makefile
	make  -C src-glu -f Makefile
	cp */*.a .
	$(AR) rcv libTinyOpenGLUtility.a src-glu/*.o
	$(RANLIB) libTinyOpenGLUtility.a
	make  -C examples -f Makefile
	make  -i -C MesaExamples -f Makefile

install:
	make -f Makefile all
	# NOTE: Do NOT copy the include files!
	cp libglu.a libglut.a libTinyOpenGLLibrary.a libTinyOpenGLLibrarySDL2.a libTinyOpenGLUtility.a $(HOME)/Retro68-build/toolchain/powerpc-apple-macos/lib/.
	
clean:
	rm -f *~ lib/libTinyGL.a include/GL/*~ TAGS
#	( for f in $(DIRS); do ( cd $$f ; make -f Makefile clean ; ) done )
	make  -C source -f Makefile clean
	make  -C examples -f Makefile clean
	make  -C MesaExamples -f Makefile clean
	make  -C agl-mini -f Makefile clean
	make  -C agl-soft -f Makefile clean
	make  -C GLUT_apple -f Makefile clean
	make  -C src-glu -f Makefile clean
	rm -rf *.a */*.a

install:
#	( for f in $(DIRS); do ( cd $$f ; make -f Makefile install ; ) done )


tar:
#	( cd .. ; tar zcvf /tmp/TinyGL-0.4.1.tar.gz TinyGL --exclude .svn --exclude TAGS )
