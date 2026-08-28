#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <TGL/gl.h>
#include "zbuffer.h"

#include "3dmath.h"
typedef unsigned char uchar;

#include <SDL.h>

#ifndef M_PI
#define M_PI 3.14159265
#endif

GLuint tex = 0;
double time_passed = 0.0;
int winSizeX = 640;
int winSizeY = 480;

void draw()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();

    // 繞著重心自轉
    glRotatef(time_passed * 2.0, 1.0, 0.0, 0.0);
    glRotatef(time_passed * 3.0, 0.0, 1.0, 0.0);

    glBegin(GL_TRIANGLES);

    // 重心校正後的新頂點座標：
    // V0 = (-0.5, -0.25, -0.375)
    // V1 = ( 0.5, -0.25, -0.375)
    // V2 = ( 0.0, -0.25,  0.625)
    // V3 = ( 0.0,  0.75,  0.125) -> 頂點

    // --- 面 1：底面 ---
    glColor3f(0.2, 0.2, 1.0);  // 藍色
    glVertex3f(-0.5, -0.25, -0.375);
    glVertex3f( 0.5, -0.25, -0.375);
    glVertex3f( 0.0, -0.25,  0.625);

    // --- 面 2：前方側面 ---
    glColor3f(1.0, 0.2, 0.2);  // 紅色
    glVertex3f(-0.5, -0.25, -0.375);
    glVertex3f( 0.5, -0.25, -0.375);
    glVertex3f( 0.0,  0.75,  0.125);

    // --- 面 3：左後方側面 ---
    glColor3f(0.2, 1.0, 0.2);  // 綠色
    glVertex3f(-0.5, -0.25, -0.375);
    glVertex3f( 0.0, -0.25,  0.625);
    glVertex3f( 0.0,  0.75,  0.125);

    // --- 面 4：右後方側面 ---
    glColor3f(1.0, 1.0, 0.2);  // 黃色
    glVertex3f( 0.5, -0.25, -0.375);
    glVertex3f( 0.0, -0.25,  0.625);
    glVertex3f( 0.0,  0.75,  0.125);

    glEnd();
    glPopMatrix();
}

void initScene()
{
    // glLightfv( GL_LIGHT0, GL_AMBIENT, white);
    // glLightfv( GL_LIGHT0, GL_SPECULAR, white);
    // glEnable( GL_CULL_FACE );
    glDisable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    // glEnable( GL_LIGHT0 );
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glTextSize(GL_TEXT_SIZE24x24);
    /*
    {
        int sw = 0, sh = 0, sc = 0; //sc goes unused.
        uchar* source_data = stbi_load("assets/texture.png", &sw, &sh, &sc, 3);
        if(source_data){
            tex = loadRGBTexture(source_data, sw, sh);
            free(source_data);
        } else {
            printf("\nCan't load texture!\n");
        }
    }*/
    glEnable(GL_NORMALIZE);
}

int main(int argc, char **argv)
{
    // initialize SDL video:

    unsigned int fps = 0;
    unsigned int maxFrames = 0;
    char needsRGBAFix = 0;
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            if (!strcmp(argv[i], "-w") && i + 1 < argc) {
                winSizeX = atoi(argv[++i]);
            } else if (!strcmp(argv[i], "-h") && i + 1 < argc) {
                winSizeY = atoi(argv[++i]);
            } else if (!strcmp(argv[i], "-fps") && i + 1 < argc) {
                fps = strtoul(argv[++i], 0, 10);
            } else if (!strcmp(argv[i], "-frames") && i + 1 < argc) {
                maxFrames = strtoul(argv[++i], 0, 10);
            }
        }
    }
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        fprintf(stderr, "ERROR: cannot initialize SDL video.\n");
        return 1;
    }
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Surface *screen = NULL;
    if ((window = SDL_CreateWindow(argv[0], SDL_WINDOWPOS_UNDEFINED,
                                   SDL_WINDOWPOS_UNDEFINED, winSizeX, winSizeY,
                                   0)) == 0) {
        fprintf(stderr, "ERROR: Video mode set failed.\n");
        return 1;
    }

    if ((renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE)) ==
        0) {
        fprintf(stderr, "ERROR: cannot create SDL renderer.\n");
        return 1;
    }

    if ((screen = SDL_GetWindowSurface(window)) == 0) {
        fprintf(stderr, "ERROR: cannot get window surface.\n");
        return 1;
    }
    printf("\nRMASK IS %u", screen->format->Rmask);
    printf("\nGMASK IS %u", screen->format->Gmask);
    printf("\nBMASK IS %u", screen->format->Bmask);
    printf("\nAMASK IS %u", screen->format->Amask);
#if TGL_FEATURE_RENDER_BITS == 32
    if (screen->format->Rmask != 0x00FF0000 ||
        screen->format->Gmask != 0x0000FF00 ||
        screen->format->Bmask != 0x000000FF) {
        needsRGBAFix = 1;
        printf(
            "\nYour screen is using an RGBA output different than this "
            "library expects.");
        printf(
            "\nYou should consider using the 16 bit version for optimal "
            "performance");
    }
#endif
    printf("\nRSHIFT IS %u", screen->format->Rshift);
    printf("\nGSHIFT IS %u", screen->format->Gshift);
    printf("\nBSHIFT IS %u", screen->format->Bshift);
    printf("\nASHIFT IS %u\n", screen->format->Ashift);
    fflush(stdout);
    SDL_ShowCursor(SDL_DISABLE);

    // initialize TinyGL:
    // unsigned int pitch;
    SDL_Texture *texture = NULL;
    switch (screen->format->BitsPerPixel) {
    case 8:
        fprintf(stderr, "ERROR: Palettes are currently not supported.\n");
        fprintf(stderr, "\nUnsupported by maintainer!!!");
        return 1;
    case 16:

        fprintf(stderr, "\nUnsupported by maintainer!!!");
        return 1;
        break;
    case 24:

        fprintf(stderr, "\nUnsupported by maintainer!!!");
        return 1;
        break;
    case 32:

        texture =
            SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA32,
                              SDL_TEXTUREACCESS_STREAMING, winSizeX, winSizeY);
        break;
    default:
        return 1;
        break;
    }
    ZBuffer *frameBuffer = NULL;
    if (TGL_FEATURE_RENDER_BITS == 32)
        frameBuffer = ZB_open(winSizeX, winSizeY, ZB_MODE_RGBA, 0);
    else
        frameBuffer = ZB_open(winSizeX, winSizeY, ZB_MODE_5R6G5B, 0);
    glInit(frameBuffer);

    // initialize GL:
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glViewport(0, 0, winSizeX, winSizeY);
    glEnable(GL_DEPTH_TEST);
    // GLfloat  h = (GLfloat) winSizeY / (GLfloat) winSizeX;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // glFrustum( -1.0, 1.0, -h, h, 5.0, 60.0 );
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    // glTranslatef( 0.0, 0.0, -45.0 );

    initScene();

    // variables for timing:
    unsigned int frames = 0;
    unsigned int framesSinceFps = 0;
    unsigned int tNow = SDL_GetTicks();
    unsigned int tLastFps = tNow;
    unsigned int tStart = tNow;

    // main loop:
    int isRunning = 1;
    while (isRunning) {
         if (maxFrames > 0 && frames >= maxFrames) break;
        ++frames;
        ++framesSinceFps;
        tNow = SDL_GetTicks();
        time_passed += 0.01666666;
        // do event handling:
        SDL_Event evt;
        while (SDL_PollEvent(&evt))
            switch (evt.type) {
            case SDL_KEYDOWN:
                switch (evt.key.keysym.sym) {
                case SDLK_ESCAPE:
                case SDLK_q:
                    isRunning = 0;
                default:
                    break;
                }
                break;
            case SDL_QUIT:
                isRunning = 0;
                break;
            }

        // draw scene:
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        draw();
        //glDrawText((unsigned char *) "Hello World!\nFrom TinyGL", 0, 0,
        //           0x00FFFFFF);
        // swap buffers:
        if (SDL_MUSTLOCK(screen) && (SDL_LockSurface(screen) < 0)) {
            fprintf(stderr, "SDL ERROR: Can't lock screen: %s\n",
                    SDL_GetError());
            return 1;
        }
        // Quickly convert all pixels to the correct format
#if TGL_FEATURE_RENDER_BITS == 32
        if (needsRGBAFix)
            for (int i = 0; i < frameBuffer->xsize * frameBuffer->ysize; i++) {
#define DATONE (frameBuffer->pbuf[i])
                DATONE =
                    ((DATONE & 0x000000FF)) << screen->format->Rshift |
                    ((DATONE & 0x0000FF00) >> 8) << screen->format->Gshift |
                    ((DATONE & 0x00FF0000) >> 16) << screen->format->Bshift;
            }
#endif
        ZB_copyFrameBuffer(frameBuffer, screen->pixels, screen->pitch);
        if (SDL_MUSTLOCK(screen))
            SDL_UnlockSurface(screen);
        SDL_UpdateTexture(texture, NULL, screen->pixels, screen->pitch);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        if (fps > 0)
            if ((1000 / fps) > (SDL_GetTicks() - tNow)) {
                SDL_Delay((1000 / fps) -
                          (SDL_GetTicks() - tNow));  // Yay stable framerate!
            }
        // check for error conditions:
        {
            const char *sdl_error = SDL_GetError();
            if (sdl_error[0] != '\0') {
                fprintf(stderr, "SDL ERROR: \"%s\"\n", sdl_error);
                SDL_ClearError();
            }
        }
        // update fps:
        if (tNow >= tLastFps + 5000) {
            printf("%u frames in %f secs, %f frames per second.\n",
                   framesSinceFps, (float)(tNow - tLastFps) * 0.001f,
                   (float)framesSinceFps * 1000.0f / (float)(tNow - tLastFps));
            tLastFps = tNow;
            framesSinceFps = 0;
        }
    }
    unsigned int tEnd = SDL_GetTicks();
    float totalSeconds = (tEnd - tStart) * 0.001f;
    printf("%u frames in %f secs, %f frames per second.\n",
           frames,
           totalSeconds,
           totalSeconds > 0.0f ? frames / totalSeconds : 0.0f);
    // cleanup:
    ZB_close(frameBuffer);
    glClose();
    if (SDL_WasInit(SDL_INIT_VIDEO)) {
        SDL_QuitSubSystem(SDL_INIT_VIDEO);
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
    return 0;
}
