CFLAGS        = /O /D__MSC__ /DFX /D__DWIN32__ /DWIN32 \
                /I..\include \
                /I$(MSVC)\include

OBJS	      = glu.obj \
		mipmap.obj \
		nurbs.obj \
		nurbscrv.obj \
		nurbssrf.obj \
		nurbsutl.obj \
		polytest.obj \
		project.obj \
		quadric.obj \
		tess.obj \
		tesselat.obj

PROGRAM       = ..\lib\fxGLU.lib

all:		$(PROGRAM)

$(PROGRAM):     $(OBJS) $(LIBS)
		lib  /verbose /OUT:$(PROGRAM) $(OBJS)

