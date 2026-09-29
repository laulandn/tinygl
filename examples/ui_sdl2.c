
/* headers */
#include <stdio.h>
#include <stdlib.h>

#include "SDL.h"

#include <GL/gl.h>
#include <GL/ostinygl.h>

#include "ui.h"

/* constants */
#define WIDTH 800
#define HEIGHT 600
int aBPP;  // display bpp
int BPP=16;  // render bpp
//int BPP=8;  // render bpp
int FORMAT=SDL_PIXELFORMAT_RGB565;
//int FORMAT=SDL_PIXELFORMAT_INDEX8;

/* globals */
ostgl_context_t *ctx;
int running;
SDL_Window *window;

static SDL_Renderer *renderer;
static SDL_Texture *texture;
// ...or...
static SDL_Surface *wsurface;


void createRendererTexture(void)
{
	renderer = SDL_CreateRenderer(window, -1, /*SDL_RENDERER_SOFTWARE*/SDL_RENDERER_PRESENTVSYNC);
    if(!renderer) {
      fprintf(stderr,"SDL_CreateRenderer failed!\n"); fflush(stderr);
            // Panic!
            SDL_Quit();
            exit(5);
    }  

fprintf(stderr,"FYI SDL_GetWindowPixelFormat=%x\n",SDL_GetWindowPixelFormat(window)); fflush(stderr);

	texture = SDL_CreateTexture(renderer, SDL_GetWindowPixelFormat(window), SDL_TEXTUREACCESS_STREAMING, ctx->width, ctx->height);
    if(!texture) {
      fprintf(stderr,"SDL_CreateTexture failed!\n"); fflush(stderr);
            // Panic!
            SDL_Quit();
            exit(5);
    }  
}


void createSurface(void)
{
  wsurface=SDL_GetWindowSurface(window);
    if(!wsurface) {
      fprintf(stderr,"SDL_GetWindowSurface failed!\n"); fflush(stderr);
            // Panic!
            SDL_Quit();
            exit(5);
    }  
}


/* open window, handle inputs */
int ui_loop(int argc, char **argv, const char *name)
{
	/* variables */
	int w, h;

	/* suppress warnings */
	(void)argc;
	(void)argv;

	/* get width, height from commandline */
	if (argc >= 3)
	{
		if (!(w = atoi(argv[1]))) w = WIDTH;
		if (!(h = atoi(argv[2]))) h = HEIGHT;
	}
	else
	{
		w = WIDTH;
		h = HEIGHT;
	}

    /* Enable standard application logging */
    SDL_LogSetPriority(SDL_LOG_CATEGORY_APPLICATION, SDL_LOG_PRIORITY_INFO);
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);

	/* init sdl */
	SDL_Init(SDL_INIT_VIDEO);

    // Clamp to actual display
    SDL_DisplayMode dm;
    // Or use SDL_GetDesktopDisplayMode
    if (SDL_GetCurrentDisplayMode(0, &dm) == 0) {
      if(w>dm.w) w=dm.w-8;
      if(h>dm.h) h=dm.h-32;  // Room for menubar/titlebar?
    }

	/* create sdl context */
	//window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, ctx->width, ctx->height, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
	window = SDL_CreateWindow(name, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN);
    if(!window) {
      fprintf(stderr,"SDL_CreateWindow failed!\n"); fflush(stderr);
            // Panic!
            SDL_Quit();
            exit(5);
    }  

    aBPP=SDL_BITSPERPIXEL(SDL_GetWindowPixelFormat(window));
    //FORMAT=SDL_GetWindowPixelFormat(window);
    //BPP=SDL_BITSPERPIXEL(FORMAT);
fprintf(stderr,"FYI FORMAT=%x BPP=%d aBPP=%d\n",FORMAT,BPP,aBPP); fflush(stderr);
    
	/* alloc */
	ctx = ostgl_create_context(w, h, BPP);
	ostgl_make_current(ctx);
fprintf(stderr,"FYI ctx is %d x %d x %d\n",ctx->width, ctx->height,ctx->depth); fflush(stderr);

	/* call user functions */
	init();
	reshape(ctx->width, ctx->height);
	
	if(aBPP>8) createRendererTexture();
	else createSurface();

	SDL_ShowWindow(window);

	/* main loop */
	running = SDL_TRUE;
	while (running == SDL_TRUE)
	{
		/* variables */
		SDL_Event event;

		/* event poll loop */
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				/* quit */
				case SDL_QUIT:
					running = SDL_FALSE;
					break;

				/* window resize event */
				case SDL_WINDOWEVENT:
				{
					switch (event.window.event)
					{
						case SDL_WINDOWEVENT_SIZE_CHANGED:
						{
						    fprintf(stderr,"FYI SDL_WINDOWEVENT_SIZE_CHANGED...\n"); fflush(stderr);
							/* resize gl */
							ostgl_resize(ctx, event.window.data1, event.window.data2);
							reshape(event.window.data1, event.window.data2);

							if(aBPP>8)
							{
							  /* destroy texture and make new one */
							  SDL_DestroyTexture(texture);
							  texture = SDL_CreateTexture(renderer, SDL_GetWindowPixelFormat(window),
								SDL_TEXTUREACCESS_STREAMING, ctx->width, ctx->height);
							}
							else {
							  // TODO redo wsurface
							}
							break;
						}
					}
					break;
				}
				
				/* key event */
				case SDL_KEYDOWN:
				{
					switch (event.key.keysym.scancode)
					{
						case SDL_SCANCODE_UP: key(KEY_UP); break;
						case SDL_SCANCODE_DOWN: key(KEY_DOWN); break;
						case SDL_SCANCODE_LEFT: key(KEY_LEFT); break;
						case SDL_SCANCODE_RIGHT: key(KEY_RIGHT); break;
						case SDL_SCANCODE_ESCAPE: key(KEY_ESCAPE); break;
						default: key(event.key.keysym.sym); break;
					}
					break;
				}
			}
		}

		idle();
	}

	/* shutdown */
	ostgl_delete_context(ctx);
	SDL_Quit();

	/* return success */
	return 0;
}


/* swap buffers */
void swap_buffersTexture(void)
{
	SDL_Surface *src = SDL_CreateRGBSurfaceWithFormatFrom(ctx->pixels, ctx->width, ctx->height, ctx->depth, ctx->width * (ctx->depth / 8), FORMAT);
	SDL_Surface *dst = SDL_CreateRGBSurfaceWithFormatFrom(NULL, ctx->width, ctx->height, 0, 0, SDL_GetWindowPixelFormat(window));

	if (SDL_LockTexture(texture, NULL, &dst->pixels, &dst->pitch) == 0)
	{
		if (SDL_BlitSurface(src, NULL, dst, NULL) != 0)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "surface blit failed: %s", SDL_GetError());
            // Panic!
            SDL_Quit();
            exit(5);
		}

		SDL_UnlockTexture(texture);
	}
	else
	{
	  SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "texture lock failed: %s", SDL_GetError());
      // Panic!
      SDL_Quit();
      exit(5);
	}

	SDL_FreeSurface(dst);
	SDL_FreeSurface(src);

	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, NULL, NULL);
	SDL_RenderPresent(renderer);
}


void ui_convert_16bpp_to_1bpp(SDL_Surface* src16, SDL_Surface* dest1) {
    // Safety check to ensure formats match expectations
    if (!src16 || !dest1) return;

    // Lock surfaces to directly access their raw memory buffers
    SDL_LockSurface(src16);
    SDL_LockSurface(dest1);

    int width = src16->w;
    int height = src16->h;

    for (int y = 0; y < height; y++) {
        // Find the start of the current row for both surfaces
        Uint16* src_row = (Uint16*)((Uint8*)src16->pixels + y * src16->pitch);
        Uint8* dest_row = (Uint8*)dest1->pixels + y * dest1->pitch;

        for (int x = 0; x < width; x++) {
            Uint16 pixel = src_row[x];

            // 1. Extract RGB channels (Assuming RGB565 layout)
            Uint8 r = ((pixel >> 11) & 0x1F) << 3;
            Uint8 g = ((pixel >> 5)  & 0x3F) << 2;
            Uint8 b = (pixel         & 0x1F) << 3;

            // 2. Calculate grayscale luminance (Standard ITU-R BT.601 weights)
            Uint8 luminance = (r * 77 + g * 150 + b * 29) >> 8;

            // 3. Determine if the pixel is black (0) or white (1)
            int bit = (luminance > 128) ? 1 : 0;

            // 4. Pack the bit into the 1-bit byte array (MSB alignment)
            int byte_index = x / 8;
            int bit_shift = 7 - (x % 8);

            if (x % 8 == 0) {
                // Clear out the target byte when we first reach it
                dest_row[byte_index] = 0; 
            }

            if (bit) {
                dest_row[byte_index] |= (1 << bit_shift);
            }
        }
    }

    // Unlock surfaces to safely allow drawing or internal operations
    SDL_UnlockSurface(dest1);
    SDL_UnlockSurface(src16);
}


/* swap buffers */
void swap_buffersSurface(void)
{
	SDL_Surface *src = SDL_CreateRGBSurfaceWithFormatFrom(ctx->pixels, ctx->width, ctx->height, ctx->depth, ctx->width * (ctx->depth / 8), FORMAT);

    if(aBPP==1) {
      ui_convert_16bpp_to_1bpp(src,wsurface);
    }
    else {
	  SDL_BlitSurface(src, NULL, wsurface, NULL);
	}
	
	SDL_UpdateWindowSurface(window);

	SDL_FreeSurface(src);
}


/* swap buffers */
void swap_buffers(void)
{
  if(aBPP>8) swap_buffersTexture();
  else swap_buffersSurface();
}


