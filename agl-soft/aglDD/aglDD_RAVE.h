/*
** aglDD_ColorQD.h - Simple Color Device Driver for QuickDraw(slow!), version: 0.5
**
** Device driver module information:
** Name: ColorQD
** Depths: (Any??)
** DoubleBufferSupport: True
** Speed: Very slow
** Other: Should work every time.
*/

/*
** AGL driver for Mesa 3D Graphics Library.
** Version: 0.5
** Mesa version: 2.0
**
** Copyright(C) 1996 Miklos Fazekas
** E-Mail: boga@augusta.inf.elte.hu
** WWW: http://www.elte.hu/~boga/Mesa.html
** 
** Part of the port of the Mesa 3D Graphics Library for MacOSª
**
*/
 
/*
** Copyright 1996, Mikl—s Fazekas.
** All Rights Reserved.
** 
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Library General Public
** License as published by the Free Software Foundation; either
** version 2 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Library General Public License for more details.
**
** You should have received a copy of the GNU Library General Public
** License along with this library; if not, write to the Free
** Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifndef __AGLDD_RAVE__
#define __AGLDD_RAVE__

#include "aglConfig.h"
#include "aglContext.h"

#include "aglDDUtil.h"

#include "RAVE.h"

#include "FB.h"


typedef struct TRAVEColor {
	float r;
	float g;
	float b;
	float a;
} TRAVEColor;

/******************************************************************************************
 *
 * TRADriverPrivate type: drivers private draw-context.
 *
 ******************************************************************************************/
typedef struct TRADriverPrivate 
{
	
	TFBBuffer frameBuffer;	

	TAGLPixelFmtRec		pixFmt;		/* Pixel format information */
	
	CGrafPtr			drawable;		/* Creation buffer (drawable) */
	
	GDHandle			device;			/* Target device */
	Rect				deviceRect;		/* Target rect position */
	
	/*
	 * For set buffer:
	 */
	GLenum			drawBufferMode; /* One of GL_FRONT/GL_BACK */
	
	/*
	 * The dimension of the drawable:
	 */
	long			width;
	long			height;
	
	/*
	 * The RAVE engine
	 */
	TQAEngine		*engine;
	char			name[255];
	
	/*
	 * The RAVE Texture buffer:
	 */
	TQATexture		*textures[255];
	TQATexture		*currentTexture;
	
	
	/*
	 * The RAVE Draw Context.
	 */
	TQADrawContext	*drawContext;

	
	/*
	 * For the Front Buffer:
	 */ 
	CGrafPtr		frontPort;
	long			frontRowBytes;
	long			frontDepth;
	void			*frontBaseAddr;

	/*
	 * Storage for current/clear color
	 */
	TRAVEColor 	currentColor;
	TRAVEColor	clearColor;
	
	RGBColor 	color;		 /* The current color we are drawing with */
	/* RGBColor 	clearColor; */ /* The current color we are clearing with */
	
	/*
	 * Storage for Mesa/AGL DC's:
	 */
	AGLContext aglCtx;
	GLcontext *gl_ctx;

} TRADriverPrivate;

#endif

