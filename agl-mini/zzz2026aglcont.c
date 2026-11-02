
/* This is a tiny fake implementation AGL, attempting to be compatible with Apple's */
/* It uses either Mesa or TinyGL's offscreen rendering */

#include <stdio.h>
#include <stdlib.h>

#include "zzz2026agl.h"


// This should be kept in sync with macosgl.c in SDL2, as much as possible...


#define USING_NO_SDL 1


#ifdef USING_SDL2
#include <SDL2/SDL.h>
#undef USING_NO_SDL
#endif
#ifdef USING_SDL1
#include <SDL/SDL.h>
#undef USING_NO_SDL
#endif


#ifdef USING_SDL2
static SDL_Texture *texture = NULL;
static SDL_Renderer *renderer = NULL;
// ...or...
static SDL_Surface *wsurface;
// ...or...
// TODO: An offscreen pixmap...
#endif


#ifdef USING_SDL2
extern void sdl2Convert_16bpp_to_1bpp(SDL_Surface* src16, SDL_Surface* dest1);
#endif


// TODO: SDL1 
AGLContext aglCreateContext(AGLPixelFormat pix, AGLContext share)
{
    fprintf(stderr,"aglCreateContext....\n"); fflush(stderr);
#ifdef USING_NO_SDL
    // GLUT should have already opened a window, but if not, we'll get our own
    if(!theOnlyLonelyWindow) {
      // TODO: Open a new window just for AGL...
    }
#endif
    if(!theOnlyLonelyWindow) {
      fprintf(stderr,"theOnlyLonelyWindow failed!\n"); fflush(stderr); 
      aglTheError=AGL_BAD_CONTEXT;
      return NULL;
    }  
#ifdef USING_SDL1
    // TODO Get actual current display depth
#endif
#ifdef USING_SDL2
    SDL_GetWindowSize(theOnlyLonelyWindow,&aglWinSizeX,&aglWinSizeY);
    aglTheScreenDepth=SDL_BITSPERPIXEL(SDL_GetWindowPixelFormat(theOnlyLonelyWindow));
#endif
    fprintf(stderr,"aglTheScreenDepth is %d\n",aglTheScreenDepth); fflush(stderr);
    fprintf(stderr,"aglTheRenderDepth is %d\n",aglTheRenderDepth); fflush(stderr);
#ifdef SDL_MACOSCLASSIC_TINYGL
    int tDepth=aglTheRenderDepth;
    if(aglTheRenderDepth==32) {
      // NOTE: This doesn't work in aglSwapBuffers...can't fool SDL...
      fprintf(stderr,"NOTE: Requested 32 bit, switching to 16!\n"); fflush(stderr);
      tDepth=16;
    }
  	osContext = ostgl_create_context(aglWinSizeX,aglWinSizeY,tDepth);
	  ostgl_make_current(osContext);
	  osBuffer=osContext->pixels;
#else
    // If not TinyGL offscreen, then we must be using Mesa
    int format=OSMESA_BGR;
    if(aglTheRenderDepth==32) format=OSMESA_ARGB;  // Not sure this is right...
    int factor=2;
    if(aglTheRenderDepth==32) factor=4;
    // TODO: The context is obviously not QUITE right...
  	osContext=OSMesaCreateContext(format,NULL);
  	osBuffer=(char *)malloc(aglWinSizeX*aglWinSizeY*factor);
    if(!osBuffer) {
      fprintf(stderr,"osBuffer failed!\n"); fflush(stderr); 
      aglTheError=AGL_BAD_CONTEXT;
      return NULL;
    }  
    GLboolean res=OSMesaMakeCurrent((OSMesaContext)osContext,osBuffer,GL_UNSIGNED_BYTE,aglWinSizeX,aglWinSizeY);
    if(!res) {
      fprintf(stderr,"OSMesaMakeCurrent failed!\n"); fflush(stderr); 
      aglTheError=AGL_BAD_CONTEXT;
      return NULL;
    }  
    else { fprintf(stderr,"got OSMesaMakeCurrent\n"); fflush(stderr); } 
#endif
    if(!osBuffer) {
      fprintf(stderr,"osBuffer failed!\n"); fflush(stderr);
      aglTheError=AGL_BAD_CONTEXT;
      return NULL;
    }
    else { fprintf(stderr,"got osBuffer\n"); fflush(stderr); } 
    if(!osContext) {
      fprintf(stderr,"osContext failed!\n"); fflush(stderr);
      aglTheError=AGL_BAD_CONTEXT;
      return NULL;
    }
    else { fprintf(stderr,"got osContext\n"); fflush(stderr); } 
#ifdef USING_SDL1
    // TODO Set up what we'll need
#endif
#ifdef USING_SDL2
    if(aglTheScreenDepth>8) {
      renderer = SDL_CreateRenderer(theOnlyLonelyWindow, -1, SDL_RENDERER_PRESENTVSYNC);	
      if(!renderer) {
        fprintf(stderr,"SDL_CreateRenderer failed!\n"); fflush(stderr);
        aglTheError=AGL_BAD_CONTEXT;
        return NULL;
      }  
      else { fprintf(stderr,"got renderer\n"); fflush(stderr); } 
      texture = SDL_CreateTexture(renderer, SDL_GetWindowPixelFormat(theOnlyLonelyWindow), SDL_TEXTUREACCESS_STREAMING, aglWinSizeX,aglWinSizeY);
      if(!texture) {
        fprintf(stderr,"SDL_CreateTexture failed!\n"); fflush(stderr);
        aglTheError=AGL_BAD_CONTEXT;
        return NULL;
      }  
      else { fprintf(stderr,"got texture\n"); fflush(stderr); }
    }
    else {
      wsurface=SDL_GetWindowSurface(theOnlyLonelyWindow);
      if(!wsurface) {
        fprintf(stderr,"SDL_GetWindowSurface failed!\n"); fflush(stderr);
            // Panic!
            SDL_Quit();
            exit(5);
      }  
    }
#endif
#ifdef USING_NO_SDL
    // TODO: Set up offscreen pixmap based on OpenGL offscreen...
#endif
    fprintf(stderr,"aglCreateContext done\n"); fflush(stderr);
    return (AGLContext)osContext;
}


#ifdef USING_SDL2
void aglSwapBuffersSDL2Surface(AGLContext ctx)
{
#ifdef SDL_MACOSCLASSIC_TINYGL
    int tFormat=SDL_PIXELFORMAT_RGB565;
    if(aglTheRenderDepth==32) tFormat=SDL_PIXELFORMAT_ARGB8888;
    // Note: If doesn't match actual win size, we're wrong for win, right for buffer...
    aglWinSizeX=((ostgl_context_t *)ctx)->width;  // probably not needed...
    aglWinSizeY=((ostgl_context_t *)ctx)->height;  // probably not needed...
    aglTheRenderDepth=((ostgl_context_t *)ctx)->depth;  // probably not needed...
    SDL_Surface *src = SDL_CreateRGBSurfaceWithFormatFrom(((ostgl_context_t *)ctx)->pixels, aglWinSizeX, aglWinSizeY, aglTheRenderDepth, aglWinSizeX * (aglTheRenderDepth / 8), tFormat);
    if(aglTheScreenDepth==1) {
      sdl2Convert_16bpp_to_1bpp(src,wsurface);
    }
    else {
	    SDL_BlitSurface(src, NULL, wsurface, NULL);
	  }
	  SDL_UpdateWindowSurface(theOnlyLonelyWindow);
	  SDL_FreeSurface(src);
#endif
}
#endif


#ifdef USING_SDL2
void aglSwapBuffersSDL2Texture(AGLContext ctx)
{
    SDL_Surface *src=NULL;
    SDL_Surface *dst=NULL;
    int tFormat=SDL_PIXELFORMAT_RGB565;
    if(aglTheRenderDepth==32) tFormat=SDL_PIXELFORMAT_ARGB8888;
    //fprintf(stderr,"aglSwapBuffers FYI window is %d by %d\n",aglWinSizeX,aglWinSizeY);
    int tDepth=aglTheRenderDepth;
#ifdef SDL_MACOSCLASSIC_TINYGL
    // Note: If doesn't match actual win size, we're wrong for win, right for buffer...
    aglWinSizeX=((ostgl_context_t *)ctx)->width;  // probably not needed...
    aglWinSizeY=((ostgl_context_t *)ctx)->height;  // probably not needed...
    aglTheRenderDepth=((ostgl_context_t *)ctx)->depth;  // probably not needed...
#else
    // Shouldn't we get mesa stuff here?
#endif
    // Would error checking here slow things down too much?
  	src = SDL_CreateRGBSurfaceWithFormatFrom(osBuffer, aglWinSizeX, aglWinSizeY, aglTheRenderDepth, (aglWinSizeX * aglTheRenderDepth / 8), tFormat);
  	dst = SDL_CreateRGBSurfaceWithFormatFrom(NULL, aglWinSizeX, aglWinSizeY, 0, 0, SDL_GetWindowPixelFormat(theOnlyLonelyWindow));
  	if (SDL_LockTexture(texture, NULL, &dst->pixels, &dst->pitch) == 0)	{
   		if (SDL_BlitSurface(src, NULL, dst, NULL) != 0) { 
  		  SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "surface blit failed: %s", SDL_GetError()); 
          // Panic!
          SDL_Quit();
          exit(5);
  		}
	  	SDL_UnlockTexture(texture);
	  }
  	else { 
  	  SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "texture lock failed: %s", SDL_GetError());
      // Panic!
      SDL_Quit();
      exit(5);
  	}
	  if(dst) SDL_FreeSurface(dst);
	  if(src) SDL_FreeSurface(src);
	  SDL_RenderClear(renderer);
	  SDL_RenderCopy(renderer, texture, NULL, NULL);
	  SDL_RenderPresent(renderer);
}
#endif


void aglSwapBuffers(AGLContext ctx)
{
#ifdef USING_SDL1
  // TODO
#endif
#ifdef USING_SDL2
  if(aglTheScreenDepth>8) aglSwapBuffersSDL2Texture(ctx);
  else aglSwapBuffersSDL2Surface(ctx);
#endif
#ifdef USING_NO_SDL
    // TODO: If we have color quickdraw, blit from offscreen pixmap to window
#endif
}

