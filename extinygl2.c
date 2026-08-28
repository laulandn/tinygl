#include <GL/gl.h>
#include "zbuffer.h"

/* Open a framebuffer (pass NULL to allocate internally) */
ZBuffer *frameBuffer = ZB_open(winSizeX, winSizeY, mode, NULL);

/* Initialize TinyGL with the framebuffer */
glInit(frameBuffer);

/* Make TinyGL calls here */

/* Copy framebuffer to display (pitch = bytes per row) */
ZB_copyFrameBuffer(frameBuffer, screen->pixels, screen->pitch);

/* Clean up */
ZB_close(frameBuffer);
glClose();