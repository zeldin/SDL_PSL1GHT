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

#if SDL_VIDEO_OPENGL_OSMESA

#include <GL/osmesa.h>

SDL_GLContext
PSL1GHT_GL_CreateContext(_THIS, SDL_Window * window)
{
    OSMesaContext context;

    context = OSMesaCreateContext(OSMESA_ARGB, NULL);

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
    int width = 640;
    int height = 480;
    static Uint32 buffer[480][640];

    if (OSMesaMakeCurrent(context, buffer, GL_UNSIGNED_BYTE, width, height)
	!= GL_TRUE) {
        SDL_SetError("Unable to make GL context current");
	return -1;
    }
    return 0;
}

void
PSL1GHT_GL_SwapWindow(_THIS, SDL_Window * window)
{
}

void
PSL1GHT_GL_DeleteContext(_THIS, SDL_GLContext context)
{
    OSMesaDestroyContext(context);
}

#endif /* SDL_VIDEO_OPENGL_OSMESA */

/* vi: set ts=4 sw=4 expandtab: */

