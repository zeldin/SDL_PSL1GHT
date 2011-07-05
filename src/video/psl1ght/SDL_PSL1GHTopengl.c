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

static int width, height, currbuf;
static void *buffer[2];

SDL_GLContext
PSL1GHT_GL_CreateContext(_THIS, SDL_Window * window)
{
    SDL_DeviceData *devdata = _this->driverdata;
    OSMesaContext context;
    uint32_t offset;
    int i;

    context = OSMesaCreateContext(OSMESA_ARGB, NULL);

    if (!context) {
        SDL_SetError("Could not create GL context");
	return NULL;
    }

    for (i=0; i<2; i++) {
        buffer[i] = rsxMemalign (64, 1920*1080*4);
	if (buffer[i] == NULL) {
	    SDL_OutOfMemory();
	    while (i>0)
	        rsxFree(buffer[--i]);
	    PSL1GHT_GL_DeleteContext(_this, context);
	    return NULL;
	}
	if (rsxAddressToOffset(buffer[i], &offset) ||
	    gcmSetDisplayBuffer (i, offset, 1920*4, 1920, 1080)) {
	    SDL_SetError("Could not set display buffer");
	    while (i>=0)
	        rsxFree(buffer[i--]);
	    PSL1GHT_GL_DeleteContext(_this, context);
	    return NULL;
	}
    }

    width=(window->w > 1920? 1920 : window->w);
    height=(window->h > 1080? 1080 : window->h);

    if (PSL1GHT_GL_MakeCurrent(_this, window, context) < 0) {
        for (i=0; i<2; i++)
	    rsxFree(buffer);
        PSL1GHT_GL_DeleteContext(_this, context);
	return NULL;
    }

    OSMesaPixelStore(OSMESA_ROW_LENGTH, 1920);
    OSMesaPixelStore(OSMESA_Y_UP, 0);

    gcmResetFlipStatus();
    gcmSetFlip(devdata->_CommandBuffer, 0);
    rsxFlushBuffer(devdata->_CommandBuffer);
    gcmSetWaitFlip(devdata->_CommandBuffer);
    
    return context;
}

int
PSL1GHT_GL_MakeCurrent(_THIS, SDL_Window * window, SDL_GLContext context)
{
    if (OSMesaMakeCurrent(context, buffer[currbuf^1],
			  GL_UNSIGNED_BYTE, width, height)
	!= GL_TRUE) {
        SDL_SetError("Unable to make GL context current");
	return -1;
    }
    return 0;
}

void
PSL1GHT_GL_SwapWindow(_THIS, SDL_Window * window)
{
    SDL_DeviceData *devdata = _this->driverdata;
    gcmSetFlip (devdata->_CommandBuffer, currbuf ^= 1);
    rsxFlushBuffer(devdata->_CommandBuffer);
    gcmSetWaitFlip(devdata->_CommandBuffer);
    while (gcmGetFlipStatus() != 0)
      usleep(200);
    gcmResetFlipStatus();
    PSL1GHT_GL_MakeCurrent(_this, window, OSMesaGetCurrentContext());
}

void
PSL1GHT_GL_DeleteContext(_THIS, SDL_GLContext context)
{
    OSMesaDestroyContext(context);
}

#endif /* SDL_VIDEO_OPENGL_OSMESA */

/* vi: set ts=4 sw=4 expandtab: */

