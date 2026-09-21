
/* This is a tiny fake implementation AGL, attempting to be compatible with Apple's */
/* It uses either Mesa or TinyGL's offscreen rendering */

#include <stdio.h>
#include <stdlib.h>

#include "zzz2026agl.h"


// This should be kept in sync with Mesa 2.4.1's AGL additions, as much as possible...


struct __AGLPixelFormatRec aglThePixelFormat;
AGLPixelFormat aglThePixelFormatPtr=(AGLPixelFormat)&aglThePixelFormat;

char *osBuffer=NULL;
OSGLContext osContext=NULL;
int aglTheError=0;

// NOTE: TinyGL doesn't seem to like 32 bit pixels...
int aglTheScreenDepth=16;  // TODO: This should be current depth
int aglWinSizeX=640,aglWinSizeY=480;  // TODO: This should be current size


#ifndef USING_SDL1
#ifndef USING_SDL2
void *theOnlyLonelyWindow=NULL;
#endif
#endif


int pixelSizeTheyAskedFor=16;
int depthSizeTheyAskedFor=16;
int stencilSizeTheyAskedFor=16;  /* Is this sane? */
int swapIntervalTheyAskedFor=16;  /* Is this sane? */


GLenum aglGetError()
{
  return aglTheError;
}


struct __AGLPixelFormatRec *aglChoosePixelFormat(GDevice ** const *gdevs, GLint ndev, const GLint *v)
{
  fprintf(stderr,"fake aglChoosePixelFormat!\n"); fflush(stderr);
  return (struct __AGLPixelFormatRec *)aglThePixelFormatPtr;
}


void aglDestroyPixelFormat(AGLPixelFormat pix)
{
  // We ignore...we only support a single static pixel format...
}


GLboolean aglSetDrawable(AGLContext ctx, AGLDrawable draw)
{
  // If using SDL we ignore...we either already have SDL's drawable, or none...
#ifndef USING_SDL1
#ifndef USING_SDL2
  if(!draw) {
    fprintf(stderr,"draw was NULL!\n"); fflush(stderr); return false;
  }
  theOnlyLonelyWindow=(void *)draw;
  fprintf(stderr,"FYI theOnlyLonelyWindow is now %x\n",theOnlyLonelyWindow); fflush(stderr);
#endif
#endif
  return true;
}


GLboolean aglSetCurrentContext(AGLContext ctx)
{
  // We ignore...we only support a single static context...
  return true;
}


GLboolean aglDestroyContext(AGLContext ctx)
{
#ifdef SDL_MACOSCLASSIC_TINYGL
  ostgl_delete_context((ostgl_context_t *)ctx);
#else
  // TODO: Destroy mesa os context here
#endif
  return true;
}


AGLContext aglGetCurrentContext(void)
{
  // We ignore...we only support a single static context...
  return NULL;
}

