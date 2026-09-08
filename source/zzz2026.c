#include "zgl.h"


/* For now... */
#ifndef DEBUG
#define DEBUG
#endif


typedef float		GLclampf;	/* single precision float in [0,1] */


/* Feature test macro */
#define TGL_HAS(x) TGL_FEATURE_##x


#define xstr(s) str(s)
#define str(s) #s


/*GLContext *gl_get_context(void)
{
    return gl_ctx;
}*/


/* clang-format off */
const GLubyte *license_string =
    (const GLubyte *) ""; /* will be generated automatically */
const GLubyte *vendor_string = (const GLubyte *) "Fabrice Bellard et alia.";
const GLubyte *renderer_string = (const GLubyte *) "TinyGL";
const GLubyte* version_string = (const GLubyte*)"" xstr(TINYGL_VERSION) " TinyGLv" xstr(TINYGL_VERSION) " "
#ifdef __GNUC__
"Compiled using GCC " __VERSION__
#endif
#ifdef __TINYC__
"Compiled using Tiny C Compiler " xstr(__TINYC__);
#endif
#ifdef _MSVC_VER
"Compiled using the worst compiler on earth, M$VC. "
#endif
#ifdef __clang__
"Compiled Using Clang " __clang_version__
#endif
    ;
const GLubyte *extensions_string = (const GLubyte *)
               "TGL_TEXTURE "
               "TGL_SMOOTHSHADING "
               "TGL_LIGHTING "
               "TGL_OPTIMIZATION_HINT_BRANCH_COST=" xstr(TGL_OPTIMIZATION_HINT_BRANCH_COST)
               " "
#if TGL_HAS(ERROR_CHECK)
               "TGL_FEATURE_ERROR_CHECK "
#endif
#if TGL_HAS(STRICT_OOM_CHECKS)
               "TGL_FEATURE_STRICT_OOM_CHECKS "
#endif
#if TGL_HAS(CUSTOM_MALLOC)
               "TGL_FEATURE_CUSTOM_MALLOC "
#endif
#if TGL_HAS(FISR)
               "TGL_FEATURE_FISR "
#endif
#if TGL_HAS(ARRAYS)
               "TGL_FEATURE_ARRAYS "
#endif
#if TGL_HAS(DISPLAYLISTS)
               "TGL_FEATURE_DISPLAYLISTS "
#endif
#if ZB_POINT_T_FRAC_BITS == (ZB_POINT_S_FRAC_BITS + TGL_FEATURE_TEXTURE_POW2)
               "TGL_FEATURE_OPTIMIZED_TEXTURE_ACCESS "
#endif
               "TGL_FEATURE_TEXTURE_POW2=" xstr(TGL_FEATURE_TEXTURE_POW2) " "
#if TGL_HAS(LIT_TEXTURES)
               "TGL_FEATURE_LIT_TEXTURES "
#endif
#if TGL_HAS(SPECULAR_BUFFERS)
               "TGL_FEATURE_SPECULAR_BUFFERS "
#endif
#if TGL_HAS(POLYGON_OFFSET)
               "TGL_FEATURE_POLYGON_OFFSET "
#endif
#if TGL_HAS(POLYGON_STIPPLE)
               "TGL_FEATURE_POLYGON_STIPPLE "
#endif
#if TGL_HAS(GL_POLYGON)
               "TGL_FEATURE_GL_POLYGON "
#endif
#if TGL_HAS(BLEND)
               "TGL_FEATURE_BLEND "
#endif
#if TGL_HAS(BLEND_DRAW_PIXELS)
               "TGL_FEATURE_BLEND_DRAW_PIXELS "
#endif
#if TGL_HAS(NO_DRAW_COLOR)
               "TGL_FEATURE_NO_DRAW_COLOR "
#endif
#if TGL_HAS(NO_COPY_COLOR)
               "TGL_FEATURE_NO_COPY_COLOR "
#endif
#if TGL_HAS(FORCE_CLEAR_NO_COPY_COLOR)
               "TGL_FEATURE_FORCE_CLEAR_NO_COPY_COLOR "
#endif
#if TGL_HAS(16_BITS)
               "TGL_FEATURE_16_BITS "
#endif
#if TGL_HAS(32_BITS)
               "TGL_FEATURE_32_BITS "
#endif
#if COMPILETIME_TINYGL_COMPAT_TEST == 1
               "TGL_COMPILETIME_TINYGL_COMPAT_TEST "
#endif
#if TGL_HAS(TINYGL_RUNTIME_COMPAT_TEST)
               "TGL_FEATURE_TINYGL_RUNTIME_COMPAT_TEST "
#endif
#if defined(_OPENMP)
               "TGL_FEATURE_MULTI_THREADED "
               "TGL_FEATURE_MULTI_THREADED_POST_PROCESS "
    #if TGL_HAS(MULTITHREADED_DRAWPIXELS)
               "TGL_FEATURE_MULTITHREADED_DRAWPIXELS "
    #endif
    #if TGL_HAS(MULTITHREADED_COPY_TEXIMAGE_2D)
               "TGL_FEATURE_MULTITHREADED_COPY_TEXIMAGE_2D "
    #endif
    #if TGL_HAS(MULTITHREADED_ZB_COPYBUFFER)
               "TGL_FEATURE_MULTITHREADED_ZB_COPYBUFFER "
    #endif
#else
               "TGL_FEATURE_SINGLE_THREADED "
#endif

#if TGL_HAS(ALIGNAS)
	       "TGL_FEATURE_ALIGNAS "
#endif
               "TGL_BUFFER_EXT "
#if TGL_HAS(ALT_RENDERMODES)
               "TGL_FEEDBACK "
               "TGL_SELECT "
#endif
               "TGL_SOFTWARE_ACCELERATED";
/* clang-format on */


const GLubyte *glGetString(GLenum name)
{
    switch (name) {
    case GL_VENDOR:
        return vendor_string;
    case GL_RENDERER:
        return renderer_string;
    case GL_VERSION:
        return version_string;
    case GL_EXTENSIONS:
        return extensions_string;
    case GL_LICENSE:
        return license_string;
    }
    return NULL;
}


GLenum glGetError()
{
#if TGL_HAS(ERROR_CHECK)
    GLContext *c = gl_get_context();
    GLenum eflag = c->error_flag;
    if (eflag != GL_OUT_OF_MEMORY)
        c->error_flag = GL_NO_ERROR;
    return eflag;
#else
    return GL_NO_ERROR;
#endif
}


void glPointSize(GLfloat f)
{
    //GLContext *c = gl_get_context();
    //c->zb->pointsize = f;
}


void glReadBuffer(GLenum mode)
{
    GLContext *c = gl_get_context();

    if ((mode != GL_FRONT && mode != GL_NONE) || c->in_begin) {
#if TGL_HAS(ERROR_CHECK)
#define ERROR_FLAG GL_INVALID_OPERATION

#else
        return;
#endif
    }
    //c->readbuffer = mode;
}


void glReadPixels(GLint x,
                  GLint y,
                  GLsizei width,
                  GLsizei height,
                  GLenum format,
                  GLenum type,
                  void *data)
{
    //GLContext *c = gl_get_context();

/*
    if (c->readbuffer != GL_FRONT ||
        (format != GL_RGBA && format != GL_RGB &&
         format != GL_DEPTH_COMPONENT) ||
#if TGL_FEATURE_RENDER_BITS == 32
        (type != GL_UNSIGNED_INT && type != GL_UNSIGNED_INT_8_8_8_8)
#elif TGL_FEATURE_RENDER_BITS == 16
        (type != GL_UNSIGNED_SHORT && type != GL_UNSIGNED_SHORT_5_6_5)
#else
#error "Unsupported TGL_FEATURE_RENDER_BITS"
#endif
    ) {
#if TGL_HAS(ERROR_CHECK)
#define ERROR_FLAG GL_INVALID_OPERATION

#else
        return;
#endif
    }
    */
    /* TODO: implement read pixels.*/
}


void glRectf(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2)
{
    glBegin(GL_QUADS);
    glVertex2f(x1, y1);
    glVertex2f(x2, y1);
    glVertex2f(x2, y2);
    glVertex2f(x1, y2);
    glEnd();
}


void glBlendFunc(GLenum sfactor, GLenum dfactor)
{
    //GLContext *c = gl_get_context();
    //c->zb->sfactor = sfactor;
    //c->zb->dfactor = dfactor;
}


void glBlendEquation(GLenum mode)
{
    //GLContext *c = gl_get_context();
    //c->zb->blendeq = mode;
}


void glDrawPixels(GLsizei width,
                  GLsizei height,
                  GLenum format,
                  GLenum type,
                  void *data)
{
/*
    / * TODO: Come up with a clever scheme for storing the data to avoid pointer
     * dependency. * /
#if TGL_FEATURE_RENDER_BITS == 32
    if (type != GL_UNSIGNED_INT && type != GL_UNSIGNED_INT_8_8_8_8) {
        tgl_warning(
            "\nERROR: Incorrect type for glDrawPixels. It MUST be "
            "GL_UNSIGNED_INT or GL_UNSIGNED_INT_8_8_8_8, A R G B!");
        return;
    }
#elif TGL_FEATURE_RENDER_BITS == 16
    if (type != GL_UNSIGNED_SHORT && type != GL_UNSIGNED_SHORT_5_6_5) {
        tgl_warning(
            "\nERROR: Incorrect type for glDrawPixels. it MUST be "
            "GL_UNSIGNED_SHORT or GL_UNSIGNED_SHORT_5_6_5, R5 G6 B5!");
        return;
    }
#else
#error "Bad TGL_FEATURE_RENDER_BITS"
#endif
    if (format != GL_RGB) {
        tgl_warning("\nERROR: Incorrect format for glDrawPixels.");
        return;
    }
    GLParam p[6];
    p[0].op = OP_DrawPixels;
    p[1].i = width;
    p[2].i = height;
    p[3].p = data;
    gl_add_op(p);
}
#define ZCMP(z, zpix) (!(zbdt) || z >= (zpix))
#define CLIPTEST(_x, _y, _w, _h) \
    ((0 <= _x) && (_w > _x) && (0 <= _y) && (_h > _y))
void glopDrawPixels(GLParam *p)
{
    GLContext *c = gl_get_context();
    GLint sy, sx, ty, tx;

    GLint w = p[1].i;
    GLint h = p[2].i;
    V4 rastpos = c->rasterpos;
    ZBuffer *zb = c->zb;
    PIXEL *d = p[3].p;
    PIXEL *pbuf = zb->pbuf;
    GLushort *zbuf = zb->zbuf;

    GLubyte zbdw = zb->depth_write;
    GLubyte zbdt = zb->depth_test;
    GLint tw = zb->xsize;
    GLint th = zb->ysize;
    GLfloat pzoomx = c->pzoomx;
    GLfloat pzoomy = c->pzoomy;

    GLint zz = c->rasterpos_zz;
#if TGL_HAS(BLEND_DRAW_PIXELS)
    TGL_BLEND_VARS
#endif
#if TGL_HAS(BLEND)
#if TGL_HAS(BLEND_DRAW_PIXELS)
    GLuint zbeb = zb->enable_blend;
#endif
#endif
    if (!c->rasterposvalid)
        return;

#if TGL_HAS(DIRTY_RECTANGLE)
    / * Mark dirty region for the pixel rectangle being drawn * /
    {
        GLint xmin = (GLint) rastpos.v[0];
        GLint xmax = (GLint) (rastpos.v[0] + (GLfloat) w * pzoomx);
        GLint ymin = (GLint) (rastpos.v[1] - (GLfloat) h * pzoomy);
        GLint ymax = (GLint) rastpos.v[1];
        ZB_markDirty(zb, xmin, ymin, xmax, ymax);
    }
#endif

#if TGL_HAS(ALT_RENDERMODES)
    if (c->render_mode == GL_SELECT) {
        gl_add_select(zz, zz);
        return;
    } else if (c->render_mode == GL_FEEDBACK) {
        gl_add_feedback(GL_DRAW_PIXEL_TOKEN, &(c->rastervertex), NULL, NULL, 0);
        return;
    }
#endif

#if TGL_HAS(MULTITHREADED_DRAWPIXELS)
#ifdef _OPENMP
#pragma omp parallel for
#endif
    for (sy = h; --sy;)
        for (sx = w; --sx;) {
            PIXEL col = d[sy * w + sx];
            V4 rastoffset;
            rastoffset.v[0] = rastpos.v[0] + (GLfloat) sx * pzoomx;
            rastoffset.v[1] = rastpos.v[1] - ((GLfloat) (h - sy) * pzoomy);
            rastoffset.v[2] = rastoffset.v[0] + pzoomx;
            rastoffset.v[3] = rastoffset.v[1] - pzoomy;

            for (ty = rastoffset.v[1]; (GLfloat) ty > rastoffset.v[3]; ty--)
                for (tx = rastoffset.v[0]; (GLfloat) tx < rastoffset.v[2]; tx++)
                    if (CLIPTEST(tx, ty, tw, th)) {
                        GLushort *pz = zbuf + (ty * tw + tx);

                        if (ZCMP(zz, *pz)) {
#if TGL_HAS(BLEND)
#if TGL_HAS(BLEND_DRAW_PIXELS)
                            if (!zbeb)
                                pbuf[tx + ty * tw] = col;
                            else
                                TGL_BLEND_FUNC(col, pbuf[tx + ty * tw])
#else
                            pbuf[tx + ty * tw] = col;
#endif
#else
                            pbuf[tx + ty * tw] = col;
#endif
                            if (zbdw)
                                *pz = zz;
                        }
                    }
        }
#else
    for (sy = h; --sy;)
        for (sx = w; --sx;) {
            PIXEL col = d[sy * w + sx];
            V4 rastoffset;
            rastoffset.v[0] = rastpos.v[0] + (GLfloat) sx * pzoomx;
            rastoffset.v[1] = rastpos.v[1] - ((GLfloat) (h - sy) * pzoomy);
            rastoffset.v[2] = rastoffset.v[0] + pzoomx;
            rastoffset.v[3] = rastoffset.v[1] - pzoomy;

            for (ty = rastoffset.v[1]; (GLfloat) ty > rastoffset.v[3]; ty--)
                for (tx = rastoffset.v[0]; (GLfloat) tx < rastoffset.v[2]; tx++)
                    if (CLIPTEST(tx, ty, tw, th)) {
                        GLushort *pz = zbuf + (ty * tw + tx);

                        if (ZCMP(zz, *pz)) {

#if TGL_HAS(BLEND)
#if TGL_HAS(BLEND_DRAW_PIXELS)
                            if (!zbeb)
                                pbuf[tx + ty * tw] = col;
                            else
                                TGL_BLEND_FUNC(col, pbuf[tx + ty * tw])
#else
                            pbuf[tx + ty * tw] = col;
#endif
#else
                            pbuf[tx + ty * tw] = col;
#endif
                            if (zbdw)
                                *pz = zz;
                        }
                    }
        }
#endif
*/
}


void glVertex2i(int x,int y) 
{
  glVertex4f(x,y,0,1);
}


void glPushAttrib(int v)
{
  // TODO really should implement this...
}


void glPopAttrib(int v)
{
  // TODO really should implement this...
}


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


/*void glBlendEquation(GLenum mode)
{
  fprintf(stderr,"glBlendEquation...not implemented\n"); fflush(stderr);
}
*/


void glGetIntegerv2( GLenum pname, GLint *params )
{
  /* TODO: Handle ones old Mesa can't or won't */
  fprintf(stderr,"FYI glGetIntegerv %d 0x%x\n",pname,pname); fflush(stderr);
  //glGetIntegerv_old(pname,params);
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


const GLubyte * glGetString2( GLenum name )
{
  /* TODO: Handle ones old Mesa can't or won't */
  const char *ret="nothing";
  fprintf(stderr,"FYI glGetString %d 0x%x\n",name,name); fflush(stderr);
  //ret=(char *)glGetString_old(name);
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


void glTexSubImage2D( GLenum target, GLint level,
                             GLint xoffset, GLint yoffset,
                             GLsizei width, GLsizei height,
                             GLenum format, GLenum type,
                             const GLvoid *pixels )
{
  fprintf(stderr,"glTexSubImage2D...not implemented\n"); fflush(stderr);
}


void glTexEnvf( GLenum target, GLenum pname, GLfloat param )
{
  fprintf(stderr,"glTexEnvf...not implemented\n"); fflush(stderr);
}


void glScissor( GLint x, GLint y, GLsizei width, GLsizei height)
{
  fprintf(stderr,"glScissor...not implemented\n"); fflush(stderr);
}


void glRasterPos2i( GLint x, GLint y )
{
  fprintf(stderr,"glRasterPos2i...not implemented\n"); fflush(stderr);
}


/*
void glPixelStorei( GLenum pname, GLint param )
{
  fprintf(stderr,"glPixelStorei...not implemented\n"); fflush(stderr);
}
*/


/*
void glOrtho( GLdouble left, GLdouble right,
		     GLdouble bottom, GLdouble top,
		     GLdouble near_val, GLdouble far_val )
{
  fprintf(stderr,"glOrtho...not implemented\n"); fflush(stderr);
}
*/


void glLineWidth( GLfloat width )
{
  fprintf(stderr,"glLineWidth...not implemented\n"); fflush(stderr);
}


void glGetPointerv( GLenum pname, void **params )
{
  fprintf(stderr,"glGetPointerv...not implemented\n"); fflush(stderr);
}


void glDepthFunc( GLenum func )
{
  fprintf(stderr,"glDepthFunc...not implemented\n"); fflush(stderr);
}


/*
void glColor4ub( GLubyte red, GLubyte green,
			GLubyte blue, GLubyte alpha )
{
  fprintf(stderr,"glColor4ub...not implemented\n"); fflush(stderr);
}
*/


void glDepthMask(int)
{
  fprintf(stderr,"glDepthFunc...not implemented\n"); fflush(stderr);
}


void glFogi(int, int)
{
  fprintf(stderr,"glDepthFunc...not implemented\n"); fflush(stderr);
}


void glFogfv(int, const float*)
{
  fprintf(stderr,"glDepthFunc...not implemented\n"); fflush(stderr);
}


void glFogf(int, float)
{
  fprintf(stderr,"glDepthFunc...not implemented\n"); fflush(stderr);
}


void glRasterPos2f(float, float)
{
  fprintf(stderr,"glDepthFunc...not implemented\n"); fflush(stderr);
}


void glPolygonStipple(void*)
{
  fprintf(stderr,"glDepthFunc...not implemented\n"); fflush(stderr);
}


void glTexParameterf(int, int, int)
{
  fprintf(stderr,"glDepthFunc...not implemented\n"); fflush(stderr);
}


void glTexImage1D( GLenum target, GLint level, GLint components,
			  GLsizei width, GLint border,
			  GLenum format, GLenum type, const GLvoid *pixels )
{
  fprintf(stderr,"glTexImage1D...not implemented\n"); fflush(stderr);
}


void glTranslated( GLdouble x, GLdouble y, GLdouble z )
{
  fprintf(stderr,"glTranslated...not implemented\n"); fflush(stderr);
}


void glMultMatrixd( const GLdouble *m )
{
  fprintf(stderr,"glMultMatrixd...not implemented\n"); fflush(stderr);
}


void glTexCoord4fv( GLfloat *v )
{
  fprintf(stderr,"glTexCoord4fv...not implemented\n"); fflush(stderr);
}


void glAlphaFunc( GLenum func, GLclampf ref )
{
  fprintf(stderr,"glAlphaFunc...not implemented\n"); fflush(stderr);
}


void glFinish( void )
{
  fprintf(stderr,"glFinish...not implemented\n"); fflush(stderr);
}


void glColorMask( GLboolean red, GLboolean green,
			 GLboolean blue, GLboolean alpha )
{
  fprintf(stderr,"glColorMask...not implemented\n"); fflush(stderr);
}


void glDrawBuffer( GLenum mode )
{
  fprintf(stderr,"glDrawBuffer...not implemented\n"); fflush(stderr);
}



