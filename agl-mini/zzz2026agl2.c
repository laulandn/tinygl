
/* This is a tiny fake implementation AGL, attempting to be compatible with Apple's */
/* It uses either Mesa or TinyGL's offscreen rendering */

#include <stdio.h>
#include <stdlib.h>

#include "zzz2026agl.h"


extern int pixelSizeTheyAskedFor;
extern int depthSizeTheyAskedFor;
extern int stencilSizeTheyAskedFor;
extern int swapIntervalTheyAskedFor;


/*
// TODO: This should be moved to and used from single static pixel format instance
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
*/


GLboolean aglDescribePixelFormat(AGLPixelFormat pix, GLint attrib,
                                        GLint *value)
{
  GLboolean ret=false;
#ifdef DEBUG
fprintf(stderr,"Called my aglDescribePixelFormat...\n"); fflush(stderr);
#endif
  switch(attrib) {
    case AGL_RGBA:
      /* Of COURSE we are rgba..right? */
      fprintf(stderr,"aglDescribePixelFormat asked about AGL_RGBA, we are lying...\n"); fflush(stderr);
      *value=true;
      ret=true;
      break;
    case AGL_ACCELERATED:
      /* Of COURSE we are accelerated..right? */
      fprintf(stderr,"aglDescribePixelFormat asked about AGL_ACCELERATED, we are lying...\n"); fflush(stderr);
      *value=true;
      ret=true;
      break;
    case AGL_FULLSCREEN:
      /* No!  No fullscreen for us! */
      fprintf(stderr,"aglDescribePixelFormat asked about AGL_FULLSCREEN, we are lying...\n"); fflush(stderr);
      *value=false;
      ret=true;
      break;
    case AGL_RENDERER_ID:
      /* Um...1? */
      fprintf(stderr,"aglDescribePixelFormat asked about AGL_RENDERER_ID, we are making one up...\n"); fflush(stderr);
      *value=1;
      ret=true;
      break;
    case AGL_DEPTH_SIZE:
      /* We return what they asked for, possibly lying.  If they didn't ask, 16 */
      fprintf(stderr,"aglDescribePixelFormat asked about AGL_DEPTH_SIZE, we are lying...\n"); fflush(stderr);
      *value=depthSizeTheyAskedFor;
      ret=true;
      break;
    case AGL_PIXEL_SIZE:
      /* We return what they asked for, possibly lying.  If they didn't ask, 16 */
      fprintf(stderr,"aglDescribePixelFormat asked about AGL_PIXEL_SIZE, we are lying...\n"); fflush(stderr);
      *value=pixelSizeTheyAskedFor;
      ret=true;
      break;
    case AGL_STENCIL_SIZE:
      /* We return what they asked for, possibly lying.  If they didn't ask, 16 */
      fprintf(stderr,"aglDescribePixelFormat asked about AGL_STENCIL_SIZE, we are lying...\n"); fflush(stderr);
      *value=stencilSizeTheyAskedFor;
      ret=true;
      break;
    case AGL_DOUBLEBUFFER:
      /* Of COURSE we are double buffered...right? */
      fprintf(stderr,"aglDescribePixelFormat asked about AGL_DOUBLEBUFFER, are we lying?\n"); fflush(stderr);
      *value=true;
      ret=true;
      break;
    default:
      fprintf(stderr,"aglDescribePixelFormat %d %d ...not implemented\n",(int)pix,(int)attrib); fflush(stderr);
      break;
  }
  return ret;
}


AGLRendererInfo aglQueryRendererInfo(const AGLDevice *gdevs, GLint ndev)
{
  fprintf(stderr,"aglQueryRendererInfo...not implemented\n"); fflush(stderr);
  return (AGLRendererInfo)0;
}


void aglDestroyRendererInfo(AGLRendererInfo rend)
{
  fprintf(stderr,"aglDestroyRendererInfo...not implemented\n"); fflush(stderr);
}


AGLRendererInfo aglNextRendererInfo(AGLRendererInfo rend)
{
  fprintf(stderr,"aglNextRendererInfo...not implemented\n"); fflush(stderr);
  return (AGLRendererInfo)0;
}


GLboolean aglDescribeRenderer(AGLRendererInfo rend, GLint prop, GLint *value)
{
  switch(prop) {
    default:
      fprintf(stderr,"aglDescribeRenderer %d...not implemented\n",prop); fflush(stderr);
      break;
  }
  return false;
}


GLboolean aglConfigure(GLenum whats, GLuint value)
{
  switch(whats) {
    default:
      fprintf(stderr,"aglConfigure %d...not implemented\n",whats); fflush(stderr);
      break;
  }
  return false;
}


const GLubyte *aglErrorString(GLenum code)
{
  if (code)
    switch (code)
    {
#ifdef AGL_GL_ERROR
      case AGL_GL_ERROR:
        return (const GLubyte *)"AGL_GL_ERROR";
        break;
#endif
      case AGL_BAD_ATTRIBUTE:
        return (const GLubyte *)"AGL_BAD_ATTRIBUTE";
        break;
      case AGL_BAD_PIXELFMT:
        return (const GLubyte *)"AGL_BAD_PIXELFMT";
        break;
      case AGL_BAD_DRAWABLE:
        return (const GLubyte *)"AGL_BAD_DRAWABLE";
        break;
#ifdef AGL_INVALID_OPT
      case AGL_INVALID_OPT:
        return (const GLubyte *)"AGL_INVALID_OPT";
        break;
#endif
      case AGL_BAD_CONTEXT:
        return (const GLubyte *)"AGL_BAD_CONTEXT";
        break;
#ifdef AGL_INVALID_GDEV
      // duplicate?
/*      case AGL_INVALID_GDEV:
        return (const GLubyte *)"AGL_INVALID_GDEV";
        break;*/
#endif
#ifdef AGL_NOT_RENDERING
      case AGL_NOT_RENDERING:
        return (const GLubyte *)"AGL_NOT_RENDERING";
        break;
#endif
#ifdef AGL_DO_ALREADY
      case AGL_DO_ALREADY:
        return (const GLubyte *)"AGL_DO_ALREADY";
        break;
#endif
#ifdef AGL_ATTRIB_UNAVAIL
      case AGL_ATTRIB_UNAVAIL:
        return (const GLubyte *)"AGL_ATTRIB_UNAVAIL";
        break;
#endif
      default:
        return (const GLubyte *)"Unknown error.";
        break;
    }
  return "Who knows?!?";
}


GLboolean aglUpdateContext(AGLContext ctx)
{
  //fprintf(stderr,"aglUpdateContext...not implemented\n"); fflush(stderr);
  // We ignore...we only support a single static context...
  // ...but maybe we should do something here if win size changed?
  return true;
}


GLboolean aglSetInteger(AGLContext ctx, GLenum pname,
                               const GLint *params)
{
#ifdef DEBUG
fprintf(stderr,"Called my aglSetInteger...\n"); fflush(stderr);
#endif
  switch(pname) {
    case AGL_COLORMAP_ENTRY:
      // We don't support indexed color, so ignore
      break;
    case AGL_SWAP_INTERVAL:
      swapIntervalTheyAskedFor=(int)*params;  /* Is this right? */
      break;
    default:
      fprintf(stderr,"aglSetInteger pname=%d...not implemented\n",(int)pname); fflush(stderr);
      break;
  }
  return true;
}


GLboolean aglDisable(AGLContext ctx, GLenum pname)
{
  switch(pname) {
    default:
      fprintf(stderr,"aglDisable %d...not implemented\n",pname); fflush(stderr);
      break;
  }
  return true;
}


GLboolean aglSetOffScreen(AGLContext ctx, int w, int h, int rowbytes, void *baseaddr)
{
  fprintf(stderr,"aglSetOffScreen %d x %d rowbytes=%d...not implemented\n",w,h,rowbytes); fflush(stderr);
  // TODO: Maybe resize window here?
  // Can we use rowbytes and baseaddr?  How?
  return true;
}


GLboolean aglSetFullScreen(AGLContext ctx, int w, int h, int freq, int dev)
{
  fprintf(stderr,"aglSetFullScreen %d x %d and %d and %d...not implemented\n",w,h,freq,dev); fflush(stderr);
  // TODO: Maybe resize window here?
  return true;
}
