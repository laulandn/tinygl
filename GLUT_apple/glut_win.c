/* Copyright (c) Mark J. Kilgard, 1994. */

/* This program is freely distributable without licensing fees 
   and is provided without guarantee or warrantee expressed or 
   implied. This program is -not- in the public domain. */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "gl.h"
#include "agl.h"
#include "glut.h"
#include "glut_const.h"
#include "glutint.h"


#include <MacWindows.h>


#ifndef GLUT_FULLSCREEN
#define GLUT_FULLSCREEN 1024
#endif


GLUTwindow *__glutCurrentWindow = NULL;
GLUTwindow **__glutWindowList = NULL;
int __glutWindowListSize = 0;

static GLUTwindow *__glutWindowCache = NULL;

GLUTwindow * __glutGetWindow(CWindowPtr win)
{
	int i;

	/* Does win belong to the last window ID looked up? */
	if(__glutWindowCache && (win == __glutWindowCache->win))
	{
		return __glutWindowCache;
	}
	
	/* Otherwise scan the window list looking for the window ID. */
	for(i = 0; i < __glutWindowListSize; i++)
	{
		if(__glutWindowList[i])
		{
			if(win == __glutWindowList[i]->win)
			{
				__glutWindowCache = __glutWindowList[i];
				return __glutWindowCache;
			}
		}
	}

	return NULL;
}

int glutGetWindow(void)
{
	if(__glutCurrentWindow)
		return __glutCurrentWindow->num + 1;
	else
		return 0;
}

int __glutSetCurrentWindow(GLUTwindow *window)
{
	__glutCurrentWindow = window;

	if(__glutCurrentWindow)
	{
		if(!window->fullscreen)
			aglSetDrawable(__glutCurrentWindow->ctx, __glutCurrentWindow->win);
		else
			aglSetFullScreen(__glutCurrentWindow->ctx, __glutCurrentWindow->width, __glutCurrentWindow->height, 60, 0);
			
		aglSetCurrentContext(__glutCurrentWindow->ctx);
	}
	else
	{
		AGLContext ctx = aglGetCurrentContext();
		
		if(ctx)
		{
			aglSetCurrentContext(NULL);
			aglSetDrawable(ctx, NULL);
		}
	}
	
	return (__glutCurrentWindow ? true : false);
}

void glutSetWindow(int win)
{
	GLUTwindow *window;

	if(win < 1 || win > __glutWindowListSize)
	{
		__glutWarning("glutWindowSet attempted on bogus window.");
		return;
	}

	window = __glutWindowList[win - 1];
	if(!window)
	{
		__glutWarning("glutWindowSet attempted on bogus window.");
		return;
	}

	__glutSetCurrentWindow(window);
}

static int getUnusedWindowSlot(void)
{
	int i;

	/* Look for allocated, unused slot. */
	for(i = 0; i < __glutWindowListSize; i++)
	{
		if(!__glutWindowList[i])
		{
			return i;
		}
	}
	
	/* Allocate a new slot. */
	__glutWindowListSize++;
	
	if(__glutWindowList)
	{
		__glutWindowList = (GLUTwindow **) realloc(__glutWindowList,
						__glutWindowListSize * sizeof(GLUTwindow *));
	}
	else
	{
		__glutWindowList = (GLUTwindow **) malloc(sizeof(GLUTwindow *));
	}
	
	if(!__glutWindowList) __glutFatalError("out of memory.");
	
	__glutWindowList[__glutWindowListSize - 1] = NULL;
	
	return __glutWindowListSize - 1;
}

static AGLPixelFormat getVisualInfoCI(unsigned int mode)
{
	static GLint bufSizeList[] = { 16, 12, 8, 4, 2, 1, 0 };
	AGLPixelFormat vi = NULL;
	GLint list[32];
	GLint i, n = 0;

	list[n++] = AGL_BUFFER_SIZE;
	list[n++] = 1;
	
	if (GLUT_WIND_IS_DOUBLE(mode))
	{
		list[n++] = AGL_DOUBLEBUFFER;
	}
	
	if (GLUT_WIND_IS_STEREO(mode))
	{
		list[n++] = AGL_STEREO;
	}
	
	if (GLUT_WIND_HAS_DEPTH(mode))
	{
		list[n++] = AGL_DEPTH_SIZE;
		list[n++] = 1;
	}
	
	if (GLUT_WIND_HAS_STENCIL(mode))
	{
		list[n++] = AGL_STENCIL_SIZE;
		list[n++] = 1;
	}
	
	if(GLUT_WIND_IS_FULLSCREEN(mode))
	{
		list[n++] = AGL_FULLSCREEN;
	}
	
	list[n] = AGL_NONE;

	aglConfigure(AGL_RETAIN_RENDERERS, GL_TRUE);	/* beretta */
	for(i = 0; bufSizeList[i]; i++)
	{
		list[1] = bufSizeList[i];
		vi = aglChoosePixelFormat(NULL, 0, list);
	}
	aglConfigure(AGL_RETAIN_RENDERERS, GL_FALSE);	/* beretta */
	
	return vi;
}

static AGLPixelFormat getVisualInfoRGB(unsigned int mode)
{
	GLint list[40];
	GLint n = 0;

	list[n++] = AGL_RGBA;
	
/*
	list[n++] = AGL_RED_SIZE;
	list[n++] = 1;
	list[n++] = AGL_GREEN_SIZE;
	list[n++] = 1;
	list[n++] = AGL_BLUE_SIZE;
	list[n++] = 1;
*/

	if(GLUT_WIND_HAS_ALPHA(mode))
	{
		list[n++] = AGL_ALPHA_SIZE;
		list[n++] = 1;
	}
	
	if(GLUT_WIND_IS_DOUBLE(mode))
	{
		list[n++] = AGL_DOUBLEBUFFER;
	}
	
	if(GLUT_WIND_IS_STEREO(mode))
	{
		list[n++] = AGL_STEREO;
	}
	
	if(GLUT_WIND_HAS_DEPTH(mode))
	{
		list[n++] = AGL_DEPTH_SIZE;
		list[n++] = 16/*24*/;
	}
	
	if(GLUT_WIND_HAS_STENCIL(mode))
	{
		list[n++] = AGL_STENCIL_SIZE;
		list[n++] = 1;
	}
	
	if(GLUT_WIND_HAS_ACCUM(mode))
	{
		list[n++] = AGL_ACCUM_RED_SIZE;
		list[n++] = 16;
		list[n++] = AGL_ACCUM_GREEN_SIZE;
		list[n++] = 16;
		list[n++] = AGL_ACCUM_BLUE_SIZE;
		list[n++] = 16;
		
		if(GLUT_WIND_HAS_ALPHA(mode))
		{
			list[n++] = AGL_ACCUM_ALPHA_SIZE;
			list[n++] = 16;
		}
	}
	
	if(GLUT_WIND_IS_FULLSCREEN(mode))
	{
		list[n++] = AGL_FULLSCREEN;
	}
	
	list[n++] = AGL_ALL_RENDERERS;
	
	#if defined(AGL_VERSION_1_1) && defined(AGL_SGIS_multisample)
		if(GLUT_WIND_IS_MULTISAMPLE(mode))
		{
			if(!__glutIsSupportedByAGL("AGL_SGIS_multisample"))
				return NULL;
			list[n++] = AGL_SAMPLES_SGIS;
			list[n++] = 4;
		}
	#endif
	
	list[n] = (int) AGL_NONE; /* terminate list */

	return aglChoosePixelFormat(NULL, 0, list);
}

AGLPixelFormat __glutGetVisualInfo(unsigned int mode)
{
	if (GLUT_WIND_IS_LUMINANCE(mode)) return NULL;

	if (GLUT_WIND_IS_RGB(mode))
		return getVisualInfoRGB(mode);
	else
		return getVisualInfoCI(mode);
}

AGLPixelFormat __glutDetermineVisual(unsigned int displayMode, Bool *treatAsSingle,
  									AGLPixelFormat (getVisualInfo) (unsigned int))
{
	AGLPixelFormat vis;

	*treatAsSingle = GLUT_WIND_IS_SINGLE(displayMode);
	vis = getVisualInfo(displayMode);
	
	if (!vis)
	{
		if(GLUT_WIND_IS_SINGLE(displayMode))
		{
			displayMode |= GLUT_DOUBLE;
			vis = getVisualInfo(displayMode);
			*treatAsSingle = true;
		}
		
		if (!vis && GLUT_WIND_IS_MULTISAMPLE(displayMode))
		{
			displayMode &= ~GLUT_MULTISAMPLE;
			vis = getVisualInfo(displayMode);
		}
	}
	
	return vis;
}

static AGLPixelFormat __glutDetermineWindowVisual(Bool *treatAsSingle)
{
	if(__glutDisplayString)
	{
		return __glutVisualInfoFromString(__glutDisplayString);
	}
	else
	{
		return __glutDetermineVisual(__glutDisplayMode, treatAsSingle, __glutGetVisualInfo);
	}
}


void __glutSetupColormap(AGLPixelFormat vi, GLUTcolormap ** colormap, CSpecArray * cmap)
{
}

void __glutDefaultDisplay(void)
{
	__glutWarning("The following is a new check for GLUT 3.0; update your code.");
	
	if(__glutCurrentWindow)
		__glutFatalError( "redisplay needed for window %d, but no display callback.", __glutCurrentWindow->num + 1);
	else
		__glutFatalError( "redisplay needed for window ?, but no display callback.");
}

void __glutDefaultReshape(int width, int height)
{
}

GLUTwindow *__glutCreateWindow(GLUTwindow *parent, const char *title, int x, int y, int width, int height, int win_type)
{
	Str255 ps;
	GLUTwindow *window;
	GLint value;
	int winnum;
	int i, arc = 0;
	
	__glutInitApp();

	winnum = getUnusedWindowSlot();
	
	window = (GLUTwindow *) malloc(sizeof(GLUTwindow));
	if(!window)
	{
		__glutFatalError("out of memory.");
	}
	
	window->num = winnum;

	window->vis = __glutDetermineWindowVisual(&window->treatAsSingle);
	if(!window->vis)
	{
		__glutFatalError("visual with necessary capabilities not found.");
	}
	
	window->ctx = aglCreateContext(window->vis, NULL);
	if(!window->ctx)
	{
		__glutFatalError("context could not be created.");
	}
	
	window->renderCtx = window->ctx;
	
	aglDescribePixelFormat(window->vis, AGL_FULLSCREEN, &value);
	window->fullscreen = value;
	
	aglDescribePixelFormat(window->vis, AGL_RGBA, &value);
	if(!value) aglDisable(window->ctx, AGL_COLORMAP_TRACKING);

	if(!window->fullscreen)
	{
		window->win = (CWindowPtr) GetNewCWindow(win_type,nil,(WindowPtr)-1L);
		if(!window->win)
		{
			__glutFatalError("window could not be created.");
		}
		
		if(title)
		{
			strcpy(window->title, title);
			__glutCToStr255(window->title, ps);
			SetWTitle((GrafPort *) window->win, ps);
		}
		else
		{
			strcpy(window->title, "GLUT Window");
			__glutCToStr255("GLUT Window", ps);
			SetWTitle((GrafPort *) window->win, ps);
		}

		MoveWindow((GrafPort *) window->win, x, y, GL_FALSE);
		SizeWindow((GrafPort *) window->win, width, height,GL_FALSE);
		ShowWindow((GrafPort *) window->win);
		HiliteWindow((GrafPort *) window->win, TRUE);
		SetPort((GrafPort *) window->win);
	}
	else
	{
		window->win = NULL;
		strcpy(window->title, "");
	}
	
	window->renderWin = window->win;
	
	window->width = width;
	window->height = height;
	window->forceReshape = true;
	window->button_press = 0;
	
	window->parent = parent;
	if(parent)
	{
		window->siblings = parent->children;
		parent->children = window;
	}
	else
	{
		window->siblings = NULL;
	}

	window->children = NULL;
	window->prevWorkWin = NULL;

	window->display = __glutDefaultDisplay;
	window->reshape = __glutDefaultReshape;
	window->visibility = NULL;
	window->workMask = GLUT_NO_WORK;
	window->mouse = NULL;
	window->special = NULL;
	window->motion = NULL;
	window->passive = NULL;
	window->keyboard = NULL;
	window->entry = NULL;
	window->menu_state = NULL;
	window->menu_status = NULL;
	window->window_status = NULL;
	window->entry_mode = 0;
	window->cursor = GLUT_CURSOR_INHERIT;
	
	for(i = 0; i < MAX_MENUS; i++) window->menus[i] = 0;

	__glutWindowList[winnum] = window;
	__glutSetCurrentWindow(window);
	
	return window;
}

int glutCreateWindow(const char *title)
{
	GLUTwindow *window;

	window = __glutCreateWindow(NULL, title,
		__glutInitX, __glutInitY,
		__glutInitWidth, __glutInitHeight, kMainWindow);

	return window->num + 1;
}

int glutCreatePlainWindow(const char *title)
{
	GLUTwindow *window;

	window = __glutCreateWindow(NULL, title,
		__glutInitX, __glutInitY,
		__glutInitWidth, __glutInitHeight, kPlainWindow);

	return window->num + 1;
}

int glutCreateSubWindow(int win, int x, int y, int width, int height)
{
  GLUTwindow *window;

  if(__glutWindowList[win - 1])
  {
  	window = __glutCreateWindow(__glutWindowList[win - 1], "GLUT Window",
    	x, y, width, height, kMainWindow);
  }
  else
  {
  	window = __glutCreateWindow(__glutWindowList[win - 1], __glutWindowList[win - 1]->title,
    	x, y, width, height, kMainWindow);
  }

  return window->num + 1;
}

void __glutDestroyWindow(GLUTwindow * window, GLUTwindow * initialWindow)
{
	GLUTwindow **prev, *cur, *parent, *siblings;

	if(!window) return;
	
	__glutRemoveFromWorkList(window);

	/* Recursively destroy any children. */
	cur = window->children;
	while(cur)
	{
		siblings = cur->siblings;
		__glutDestroyWindow(cur, initialWindow);
		cur = siblings;
	}
	
	/* Remove from parent's children list (only necessary for
	non-initial windows and subwindows!). */
	parent = window->parent;
	if(parent && parent == initialWindow->parent)
	{
		prev = &parent->children;
		cur = parent->children;
		
		while(cur)
		{
			if(cur == window)
			{
				*prev = cur->siblings;
				break;
			}
			
			prev = &(cur->siblings);
			cur = cur->siblings;
		}
	}
	
	aglDestroyContext(window->ctx);

	/* Unbind if bound to this window. */
	if(window == __glutCurrentWindow)
		__glutSetCurrentWindow(NULL);
	
	if(!window->fullscreen)
		DisposeWindow((GrafPort *) window->win);

	/* NULLing the __glutWindowList helps detect is a window
	instance has been destroyed, given a window number. */
	__glutWindowList[window->num] = NULL;

	/* Remove window from the "get window cache" if it is there. */
	if(__glutWindowCache == window)
		__glutWindowCache = NULL;

	aglDestroyPixelFormat(window->vis);
	free(window);
}

void glutDestroyWindow(int win)
{
  GLUTwindow *window = __glutWindowList[win - 1];

  __glutDestroyWindow(window, window);
}

void glutSwapBuffers(void)
{
	aglSwapBuffers(aglGetCurrentContext());
}

void glutDisplayFunc(GLUTdisplayCB displayFunc)
{
	if(!__glutCurrentWindow)
	{
		__glutWarning("glutDisplayFunc: no active window");
		return;
	}

	if (!displayFunc) __glutFatalError("NULL display callback not allowed in GLUT 3.0; update your code.");

	__glutCurrentWindow->display = displayFunc;
}

void glutKeyboardFunc(GLUTkeyboardCB keyboardFunc)
{
	if(!__glutCurrentWindow)
	{
		__glutWarning("glutKeyboardFunc: no active window");
		return;
	}
	
  __glutCurrentWindow->keyboard = keyboardFunc;
}

void glutSpecialFunc(GLUTspecialCB specialFunc)
{
	if(!__glutCurrentWindow)
	{
		__glutWarning("glutSpecialFunc: no active window");
		return;
	}
	
  __glutCurrentWindow->special = specialFunc;
}

void glutMouseFunc(GLUTmouseCB mouseFunc)
{
	if(!__glutCurrentWindow)
	{
		__glutWarning("glutMouseFunc: no active window");
		return;
	}
	
  __glutCurrentWindow->mouse = mouseFunc;
}

void glutMotionFunc(GLUTmotionCB motionFunc)
{
	if(!__glutCurrentWindow)
	{
		__glutWarning("glutMotionFunc: no active window");
		return;
	}
	
  __glutCurrentWindow->motion = motionFunc;
  
  if(motionFunc) __glutPutOnWorkList(__glutCurrentWindow, GLUT_BUTTON_WORK);
}

void glutPassiveMotionFunc(GLUTpassiveCB passiveMotionFunc)
{
	if(!__glutCurrentWindow)
	{
		__glutWarning("glutPassiveMotionFunc: no active window");
		return;
	}
	
  __glutCurrentWindow->passive = passiveMotionFunc;
  
  if(passiveMotionFunc) __glutPutOnWorkList(__glutCurrentWindow, GLUT_PASSIVE_WORK);
}

void glutEntryFunc(GLUTentryCB entryFunc)
{
	if(!__glutCurrentWindow)
	{
		__glutWarning("glutEntryFunc: no active window");
		return;
	}
	
	__glutCurrentWindow->entry = entryFunc;
	__glutCurrentWindow->entry_mode = GLUT_ENTERED;
		
	if(entryFunc) __glutPutOnWorkList(__glutCurrentWindow, GLUT_ENTRY_WORK);
}

void glutWindowStatusFunc(GLUTwindowStatusCB windowStatusFunc)
{
	if(!__glutCurrentWindow)
	{
		__glutWarning("glutWindowStatusFunc: no active window");
		return;
	}
	
	__glutCurrentWindow->window_status = windowStatusFunc;
}

void glutVisibilityFunc(GLUTvisibilityCB visibilityFunc)
{
	if(!__glutCurrentWindow)
	{
		__glutWarning("glutVisibilityFunc: no active window");
		return;
	}
	
	__glutCurrentWindow->visibility = visibilityFunc;
	
	if(visibilityFunc) visibilityFunc(GLUT_VISIBLE);
}

void glutReshapeFunc(GLUTreshapeCB reshapeFunc)
{
	if(!__glutCurrentWindow)
	{
		__glutWarning("glutReshapeFunc: no active window");
		return;
	}
	
  if(reshapeFunc)
  {
		__glutCurrentWindow->reshape = reshapeFunc;
  }
  else
  {
		__glutCurrentWindow->reshape = __glutDefaultReshape;
  }
}
