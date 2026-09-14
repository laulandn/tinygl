#ifndef __ZZZ2026AGL_H__
#define __ZZZ2026AGL_H__


/* This is a tiny fake implementation AGL, attempting to be compatible with Apple's */
/* It uses either Mesa or TinyGL's offscreen rendering */


// NOTE: You can, and should, use a real Apple agl.h
#include <GL/agl.h>


// Define SDL_MACOSCLASSIC_TINYGL to use TinyGL, otherwise we will assume Mesa

// Define USING_SDL2 to use SDL2, USING_SDL1 for SDL1, neither for no SDL

// NOTE: If you don't define either, there may be no display!


#ifdef SDL_MACOSCLASSIC_TINYGL
#include "GL/ostinygl.h"
#else
#include <GL/osmesa.h>
#endif


// This should be opaque to clients
typedef struct __AGLPixelFormatRec
{
  int fake;
} __AGLPixelFormatRec;

// This should be opaque to clients
typedef struct __AGLContextRec
{
  int fake;
} __AGLContextRec;


#ifdef SDL_MACOSCLASSIC_TINYGL
typedef ostgl_context_t * OSGLContext;
#else
typedef OSMesaContext OSGLContext;
#endif


extern struct __AGLPixelFormatRec aglThePixelFormat;
extern AGLPixelFormat aglThePixelFormatPtr;

extern char *osBuffer;
extern OSGLContext osContext;
extern int aglTheError;

extern int aglTheScreenDepth;  // TODO: This should be current depth
extern int aglWinSizeX,aglWinSizeY;  // TODO: This should be current size

// This is needed, so we'll have to get it somehow...
extern void *theOnlyLonelyWindow;


#endif
