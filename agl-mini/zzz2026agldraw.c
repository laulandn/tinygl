
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
// NOTE: This is temp code, doesn't dither, and not efficient, but works
void sdl2Convert_16bpp_to_1bpp(SDL_Surface* src16, SDL_Surface* dest1) {
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
#endif

