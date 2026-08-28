//First you have to include
//(Note that you must either link against libTinyGL.a or compile it in the same compilation unit as your program)
#include "../include/GL/gl.h"
#include "../include/zbuffer.h"

/*
	Somewhere in your program...
*/

//Next, open a framebuffer.
//The "0" parameter is where you pass in a framebuffer pointer if you've already made one.
ZBuffer* frameBuffer = ZB_open(winSizeX, winSizeY, mode, 0);

//Tell TinyGL to initialize on that framebuffer
glInit(frameBuffer);

//Begin making TinyGL calls!

//When you want to copy to your target screen
//Pitch is the width of the target in bytes, or bytes per pixel times width;
ZB_copyFrameBuffer(frameBuffer, screen->pixels, screen->pitch);




//At the end of your application, when you want to clean up.
ZB_close(frameBuffer);
glClose();
