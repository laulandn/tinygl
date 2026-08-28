/* These are functions that were new for later versions of AGL */
/* this is a massive hack */


/* For now... */
#ifndef DEBUG
#define DEBUG
#endif


#include <stdio.h>

#include <gl.h>


int glActiveTextureARB(void)
{
  fprintf(stderr,"glActiveTextureARB...not implemented\n"); fflush(stderr);
  return 0;
}


int glClientActiveTextureARB(void)
{
  fprintf(stderr,"glClientActiveTextureARB...not implemented\n"); fflush(stderr);
  return 0;
}


int glLockArraysEXT(void)
{
  fprintf(stderr,"glLockArraysEXT...not implemented\n"); fflush(stderr);
  return 0;
}


void glBlendFuncSeparate(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha)
{
  fprintf(stderr,"glBlendFuncSeparate...not implemented\n"); fflush(stderr);
}


void glBlendEquation(GLenum mode)
{
  fprintf(stderr,"glBlendEquation...not implemented\n"); fflush(stderr);
}


void glGetIntegerv( GLenum pname, GLint *params )
{
  /* TODO: Handle ones old Mesa can't or won't */
  //fprintf(stderr,"FYI glGetIntegerv %d 0x%x\n",pname,pname); fflush(stderr);
  glGetIntegerv_old(pname,params);
  //fprintf(stderr,"FYI glGetIntegerv_old said %d 0x%x\n",*params,*params); fflush(stderr);
  switch(pname) {
    case 3410:
      fprintf(stderr,"FYI glGetIntegerv is lying and we are saying SDL_GL_RED_SIZE 5\n"); fflush(stderr);
      fprintf(stderr,"FYI glGetIntegerv_old said %d 0x%x\n",*params,*params); fflush(stderr);
      *params=5;
      break;
    case 3411:
      fprintf(stderr,"FYI glGetIntegerv is lying and we are saying SDL_GL_BLUE_SIZE 5\n"); fflush(stderr);
      fprintf(stderr,"FYI glGetIntegerv_old said %d 0x%x\n",*params,*params); fflush(stderr);
      *params=5;
      break;
    case 3412:
      fprintf(stderr,"FYI glGetIntegerv is lying and we are saying SDL_GL_GREEN_SIZE 5\n"); fflush(stderr);
      fprintf(stderr,"FYI glGetIntegerv_old said %d 0x%x\n",*params,*params); fflush(stderr);
      *params=5;
      break;
    case 3414:
      fprintf(stderr,"FYI glGetIntegerv is lying and we are saying SDL_GL_DEPTH 16\n"); fflush(stderr);
      fprintf(stderr,"FYI glGetIntegerv_old said %d 0x%x\n",*params,*params); fflush(stderr);
      *params=16;
      break;
    default:
      /* Assume old Mesa answered correctly... */
      break;
  }
}


const GLubyte * glGetString( GLenum name )
{
  /* TODO: Handle ones old Mesa can't or won't */
  char *ret=NULL;
  /*fprintf(stderr,"FYI glGetString %d 0x%x\n",name,name); fflush(stderr);*/
  ret=(char *)glGetString_old(name);
  /*fprintf(stderr,"FYI glGetString_old said %s\n",ret); fflush(stderr);*/
  switch(name) {
    case GL_VENDOR:
      fprintf(stderr,"FYI glGetString is lying and we are saying GL_VENDOR Mesa\n"); fflush(stderr);
      fprintf(stderr,"FYI glGetString_old said %s\n",ret); fflush(stderr);
      ret="Mesa";
      break;
    case GL_RENDERER:
      fprintf(stderr,"FYI glGetString is lying and we are saying GL_RENDERER Generic\n"); fflush(stderr);
      fprintf(stderr,"FYI glGetString_old said %s\n",ret); fflush(stderr);
      ret="Generic";
      break;
    case GL_VERSION:
      fprintf(stderr,"FYI glGetString is lying and we are saying  GL_VERSION 1.1\n"); fflush(stderr);
      fprintf(stderr,"FYI glGetString_old said %s\n",ret); fflush(stderr);
      ret="1.1";
      break;
    case GL_EXTENSIONS:
      fprintf(stderr,"FYI glGetString is lying and we are saying GL_EXTENSIONS unknown\n"); fflush(stderr);
      fprintf(stderr,"FYI glGetString_old said %s\n",ret); fflush(stderr);
      ret="Unknown";
      break;
    default:
      /* Assume old Mesa answered correctly... */
      break;
  }
  return (const GLubyte *)ret;
}
