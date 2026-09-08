
#include <stdio.h>
#include <stdlib.h>

#include <GL/agl.h>


// Define SDL_MACOSCLASSIC_TINYGL to use TinyGL, otherwise we will assume Mesa


#ifdef SDL_MACOSCLASSIC_TINYGL
#include "GL/ostinygl.h"
#else
#include <GL/osmesa.h>
#endif


typedef struct __AGLPixelFormatRec
{
  int fake;
} __AGLPixelFormatRec;


typedef struct __AGLContextRec
{
  int fake;
} __AGLContextRec;


struct __AGLPixelFormatRec aglThePixelFormat;
AGLPixelFormat aglThePixelFormatPtr=(AGLPixelFormat)&aglThePixelFormat;


char *osBuffer=NULL;
AGLContext osContext=NULL;
int aglTheError=0;

int aglTheScreenDepth=32;  // TODO: This should be current depth
int aglWinSizeX=640,aglWinSizeY=480;  // TODO: This should be current size


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
  // We ignore...we either already have SDL's drawable, or none...
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


GLboolean aglUpdateContext(AGLContext ctx)
{
  // We ignore...we only support a single static context...
  // ...but maybe we should do something here if win size changed?
  return true;
}


GLboolean aglSetInteger(AGLContext ctx, GLenum pname,const GLint *params)
{
  fprintf(stderr,"fake aglSetInteger %d to %d!\n",pname,*params); fflush(stderr);
  return true;
}


const GLubyte * aglErrorString(GLenum code)
{
  if(aglTheError==1) return (GLubyte *)"Problem getting something!";
  else return (GLubyte *)"Who knows";
}


// TODO: This should be moved to and used from static pixel format
GLboolean aglDescribePixelFormat(struct __AGLPixelFormatRec *fmt,GLint what,GLint *value)
{
  switch(what) {
    case AGL_RGBA:
      *value=1;
      return true;
      break;
    case AGL_ACCELERATED:
      *value=true;
      return true;
      break;
    case AGL_RENDERER_ID:
      *value=1;
      return true;
      break;
    case AGL_DEPTH_SIZE:
      *value=20;
      return true;
      break;
    case AGL_STENCIL_SIZE:
      *value=20;
      return true;
      break;
    case AGL_DOUBLEBUFFER:
      *value=true;
      return true;
      break;
    case AGL_PIXEL_SIZE:
      *value=aglTheScreenDepth;
      return true;
      break;
    default:
      fprintf(stderr,"aglDescribePixelFormat...what=%d implemented\n",what); fflush(stderr);
      return false;
      break;
  }
  return false;
}

