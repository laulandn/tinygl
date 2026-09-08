
#include <stdio.h>
#include <stdlib.h>

#include <GL/agl.h>


// Define SDL_MACOSCLASSIC_TINYGL to use TinyGL, otherwise we will assume Mesa

// Define USING_SDL2 to use SDL2, USING_SDL1 for SDL1, nothing for no SDL

// NOTE: If you don't define either, there will be no display!


#ifdef USING_SDL2
#include <SDL2/SDL.h>
#endif
#ifdef USING_SDL1
#include <SDL/SDL.h>
#endif


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


extern char *osBuffer;
extern AGLContext osContext;
extern int aglTheError;

// These may or may not match actual screen...
extern int aglTheScreenDepth;
extern int aglWinSizeX,aglWinSizeY;


#ifdef USING_SDL2
static SDL_Texture *texture = NULL;
static SDL_Renderer *renderer = NULL;
#endif

// This is needed, so we'll have to get it somehow...
#ifdef USING_SDL2
extern SDL_Window *theOnlyWindow;
#endif


AGLContext aglCreateContext(AGLPixelFormat pix, AGLContext share)
{
#ifdef USING_SDL2
    if(!theOnlyWindow) {
      fprintf(stderr,"theOnlyWindow failed!\n"); fflush(stderr); aglTheError=1;
      return NULL;
    }  
    SDL_GetWindowSize(theOnlyWindow,&aglWinSizeX,&aglWinSizeY);
#endif
#ifdef SDL_MACOSCLASSIC_TINYGL
  	osContext = (AGLContext)ostgl_create_context(aglWinSizeX,aglWinSizeY,aglTheScreenDepth);
	  ostgl_make_current((ostgl_context_t *)osContext);
	  osBuffer=((ostgl_context_t *)osContext)->pixels;
#else
    int format=OSMESA_RGB;
    if(aglTheScreenDepth==32) format=OSMESA_ARGB;
    int factor=2;
    if(aglTheScreenDepth==32) factor=4;
    fprintf(stderr,"aglTheScreenDepth is %d\n",aglTheScreenDepth); fflush(stderr);
    // TODO: The context is obviously not QUITE right...
  	osContext=(AGLContext)OSMesaCreateContext(format,NULL);
  	osBuffer=(char *)malloc(aglWinSizeX*aglWinSizeY*factor);
    if(!osBuffer) {
      fprintf(stderr,"osBuffer failed!\n"); fflush(stderr); aglTheError=1;
      return NULL;
    }  
    else { fprintf(stderr,"got osBuffer\n"); fflush(stderr); } 
    GLboolean res=OSMesaMakeCurrent((OSMesaContext)osContext,osBuffer,GL_UNSIGNED_BYTE,aglWinSizeX,aglWinSizeY);
    if(!res) {
      fprintf(stderr,"OSMesaMakeCurrent failed!\n"); fflush(stderr); aglTheError=1;
      return NULL;
    }  
    else { fprintf(stderr,"got OSMesaMakeCurrent\n"); fflush(stderr); } 
#endif
    if(!osContext) {
      fprintf(stderr,"osContext failed!\n"); fflush(stderr); aglTheError=1;
      return NULL;
    }
    else { fprintf(stderr,"got osContext\n"); fflush(stderr); } 
#ifdef USING_SDL2
    renderer = SDL_CreateRenderer(theOnlyWindow, -1, SDL_RENDERER_PRESENTVSYNC);	
    if(!renderer) {
      fprintf(stderr,"SDL_CreateRenderer failed!\n"); fflush(stderr); aglTheError=1;
      return NULL;
    }  
    else { fprintf(stderr,"got renderer\n"); fflush(stderr); } 
    texture = SDL_CreateTexture(renderer, SDL_GetWindowPixelFormat(theOnlyWindow), SDL_TEXTUREACCESS_STREAMING, aglWinSizeX,aglWinSizeY);
    if(!texture) {
      fprintf(stderr,"SDL_CreateTexture failed!\n"); fflush(stderr); aglTheError=1;
      return NULL;
    }  
    else { fprintf(stderr,"got texture\n"); fflush(stderr); }
#endif
    fprintf(stderr,"aglCreateContext done\n"); fflush(stderr);
    return (AGLContext)osContext;
}


void aglSwapBuffers(AGLContext ctx)
{
#ifdef USING_SDL2
    SDL_Surface *src=NULL;
    SDL_Surface *dst=NULL;
#endif
#ifdef SDL_MACOSCLASSIC_TINYGL
    // Note: If doesn't match actual win size, we're wrong for win, right for buffer...
    aglWinSizeX=((ostgl_context_t *)ctx)->width;
    aglWinSizeY=((ostgl_context_t *)ctx)->height;
    // Same here...
    aglTheScreenDepth=((ostgl_context_t *)ctx)->depth;
#else
    // Shouldn't we get mesa stuff here?
#endif
#ifdef USING_SDL2
    // Would error checking here slow things down?
  	src = SDL_CreateRGBSurfaceWithFormatFrom(osBuffer, aglWinSizeX, aglWinSizeY, aglTheScreenDepth, (aglWinSizeX * aglTheScreenDepth / 8), SDL_PIXELFORMAT_RGB565);
  	dst = SDL_CreateRGBSurfaceWithFormatFrom(NULL, aglWinSizeX, aglWinSizeY, 0, 0, SDL_GetWindowPixelFormat(theOnlyWindow));
  	if (SDL_LockTexture(texture, NULL, &dst->pixels, &dst->pitch) == 0)
  	{
   		if (SDL_BlitSurface(src, NULL, dst, NULL) != 0)
  		{ SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "surface blit failed: %s", SDL_GetError()); }
	  	SDL_UnlockTexture(texture);
	  }
  	else
	  { SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "texture lock failed: %s", SDL_GetError()); }
	  if(dst) SDL_FreeSurface(dst);
	  if(src) SDL_FreeSurface(src);
	  SDL_RenderClear(renderer);
	  SDL_RenderCopy(renderer, texture, NULL, NULL);
	  SDL_RenderPresent(renderer);
#endif
}

