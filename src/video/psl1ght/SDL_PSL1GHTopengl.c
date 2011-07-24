/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2011 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#include "SDL_config.h"

#include "SDL_PSL1GHTvideo.h"

/* PSL1GHT implementation of SDL OpenGL support */

#if SDL_VIDEO_OPENGL

#define EGL_EGLEXT_PROTOTYPES

#include <EGL/egl.h>
#include <EGL/eglext.h>

struct SDL_GLDriverData
{
    int initialized;
    EGLDisplay d;
    EGLConfig config;
    EGLScreenMESA screen;
};

struct SDL_GLWindowData
{
    EGLSurface surf;
};

int
PSL1GHT_GL_Initialize(_THIS)
{
    SDL_DeviceData *devdata = _this->driverdata;
    struct SDL_GLDriverData *gl;
    int maj, min, num_config, num_screens;
    static const EGLint confAttribs[] = {
      EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
      EGL_NATIVE_RENDERABLE, EGL_TRUE,
      EGL_SURFACE_TYPE, EGL_SCREEN_BIT_MESA,
      EGL_DEPTH_SIZE, 1,
      EGL_NONE
    };

    if (_this->gl_data) {
        return 0;
    }

    _this->gl_data = gl =
        (struct SDL_GLDriverData *) SDL_calloc(1,
                                               sizeof(struct
                                                      SDL_GLDriverData));
    if (!gl) {
        SDL_OutOfMemory();
        return -1;
    }
    gl->initialized = 0;

    ++gl->initialized;

    gl->d = eglGetDisplay(devdata->_CommandBuffer);
    if (!gl->d || !eglInitialize(gl->d, &maj, &min) ||
	!eglChooseConfig(gl->d, confAttribs, &gl->config, 1, &num_config) ||
	num_config != 1 ||
	!eglGetScreensMESA(gl->d, &gl->screen, 1, &num_screens) ||
	num_screens != 1) {
        SDL_SetError("Could not initialize EGL");
	return -1;
    }

    eglBindAPI(EGL_OPENGL_API);

    return 0;
}

void
PSL1GHT_GL_Shutdown(_THIS)
{
    if (!_this->gl_data || (--_this->gl_data->initialized > 0)) {
        return;
    }

    eglTerminate(_this->gl_data->d);

    SDL_free(_this->gl_data);
    _this->gl_data = NULL;
}


static int PSL1GHT_GL_CreateWindowSurface(_THIS, struct SDL_Window *window,
					  struct SDL_GLWindowData *windata)
{

    struct SDL_GLDriverData *gl = _this->gl_data;
    SDL_VideoDisplay *display = SDL_GetDisplayForWindow(window);
    SDL_DisplayMode *displayMode = &display->current_mode;
    EGLint w, h, bestw, besth;
    EGLModeMESA mode, modes[128];
    EGLint count, i;
    EGLint modeAttribs[] = {
      EGL_WIDTH, displayMode->w,
      EGL_HEIGHT, displayMode->h,
      EGL_NONE
    };
    if (!eglChooseModeMESA(gl->d, gl->screen, modeAttribs, modes, 128, &count)
	|| count < 1) {
        SDL_SetError("Unable to choose GL mode");
	return -1;
    }
    mode = modes[0];
    eglGetModeAttribMESA(gl->d, mode, EGL_WIDTH, &bestw);
    eglGetModeAttribMESA(gl->d, mode, EGL_HEIGHT, &besth);
    for (i=1; i<count; i++) {
      eglGetModeAttribMESA(gl->d, modes[i], EGL_WIDTH, &w);
      eglGetModeAttribMESA(gl->d, modes[i], EGL_HEIGHT, &h);
      if (w <= bestw && h <= besth &&
	  (w < bestw || h < besth)) {
	bestw = w;
	besth = h;
	mode = modes[i];
      }
    }
    w = bestw;
    h = besth;
    if (w < window->w) w = window->w;
    if (h < window->h) h = window->h;
    EGLint screenAttribs[] = {
      EGL_WIDTH, w,
      EGL_HEIGHT, h,
      EGL_NONE
    };
    if ((windata->surf = eglCreateScreenSurfaceMESA(gl->d, gl->config,
						    screenAttribs))
	== EGL_NO_SURFACE) {
      SDL_SetError("Unable to create GL surface");
      return -1;
    }
    if (!eglShowScreenSurfaceMESA(gl->d, gl->screen, windata->surf, mode)) {
      SDL_SetError("Unable to display GL surface");
      return -1;
    }

    return 0;
}

static void PSL1GHT_GL_DestroyWindowSurface(_THIS, struct SDL_GLWindowData *windata)
{
    if (windata->surf != EGL_NO_SURFACE) {
      if (eglGetCurrentSurface(EGL_DRAW) == windata->surf)
	eglMakeCurrent(_this->gl_data->d, EGL_NO_SURFACE, EGL_NO_SURFACE,
		       EGL_NO_CONTEXT);
      eglDestroySurface(_this->gl_data->d, windata->surf);
      windata->surf = EGL_NO_SURFACE;
    }
}

extern int PSL1GHT_GL_CreateWindow(_THIS, SDL_Window * window)
{
    if(!(window->flags & SDL_WINDOW_OPENGL))
      return 0;

    struct SDL_GLWindowData *windata =
      SDL_calloc(1, sizeof(struct SDL_GLWindowData));

    if (!windata) {
        SDL_OutOfMemory();
        return -1;
    }
    window->driverdata = windata;

    return PSL1GHT_GL_CreateWindowSurface(_this, window, windata);
}

extern void PSL1GHT_GL_DestroyWindow(_THIS, SDL_Window * window)
{
    struct SDL_GLWindowData *windata;
    
    if(!(window->flags & SDL_WINDOW_OPENGL) ||
       (windata = window->driverdata) == NULL)
      return;

    PSL1GHT_GL_DestroyWindowSurface(_this, windata);

    SDL_free(windata);
    window->driverdata = NULL;
}

extern void PSL1GHT_GL_SetWindowFullscreen(_THIS, SDL_Window * window, SDL_VideoDisplay * display, SDL_bool fullscreen)
{
    EGLContext oldContext = EGL_NO_CONTEXT;
    struct SDL_GLWindowData *windata;

    if(!(window->flags & SDL_WINDOW_OPENGL) ||
       (windata = window->driverdata) == NULL)
      return;

    if (windata->surf != EGL_NO_SURFACE &&
	windata->surf == eglGetCurrentSurface(EGL_DRAW))
      oldContext = eglGetCurrentContext();

    PSL1GHT_GL_DestroyWindowSurface(_this, windata);
    PSL1GHT_GL_CreateWindowSurface(_this, window, windata);

    if (oldContext != EGL_NO_CONTEXT)
      PSL1GHT_GL_MakeCurrent(_this, window, oldContext);
}

SDL_GLContext
PSL1GHT_GL_CreateContext(_THIS, SDL_Window * window)
{
    struct SDL_GLDriverData *gl = _this->gl_data;
    EGLContext context;

    context = eglCreateContext(gl->d, gl->config, EGL_NO_CONTEXT, NULL);

    if (!context) {
        SDL_SetError("Could not create GL context");
	return NULL;
    }

    if (PSL1GHT_GL_MakeCurrent(_this, window, context) < 0) {
        PSL1GHT_GL_DeleteContext(_this, context);
	return NULL;
    }

    return context;
}

int
PSL1GHT_GL_MakeCurrent(_THIS, SDL_Window * window, SDL_GLContext context)
{
    struct SDL_GLDriverData *gl = _this->gl_data;
    EGLSurface surf = EGL_NO_SURFACE;
    if (window) {
      struct SDL_GLWindowData *windata = window->driverdata;
      if (windata && windata->surf != EGL_NO_SURFACE)
	surf = windata->surf;
    }
    if (eglMakeCurrent(gl->d, surf, surf, context)
	!= EGL_TRUE) {
        SDL_SetError("Unable to make GL context current");
	return -1;
    }
    return 0;
}

void
PSL1GHT_GL_SwapWindow(_THIS, SDL_Window * window)
{
    struct SDL_GLDriverData *gl = _this->gl_data;
    struct SDL_GLWindowData *windata = window->driverdata;
    eglSwapBuffers( gl->d, windata->surf ); 
}

void
PSL1GHT_GL_DeleteContext(_THIS, SDL_GLContext context)
{
    struct SDL_GLDriverData *gl = _this->gl_data;
    if (eglGetCurrentContext() == context)
      eglMakeCurrent(gl->d, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(gl->d, context);
}

#endif /* SDL_VIDEO_OPENGL */

/* vi: set ts=4 sw=4 expandtab: */

