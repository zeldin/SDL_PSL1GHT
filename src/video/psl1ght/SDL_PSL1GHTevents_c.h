/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2010 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/
#include "SDL_config.h"

#include "SDL_PSL1GHTvideo.h"
#include "kd.h"

extern void PSL1GHT_PumpEvents(_THIS);
extern void PSL1GHT_InitSysEvent(_THIS);
extern void PSL1GHT_QuitSysEvent(_THIS);
extern int PSL1GHT_initkeymaps(int fd);
/*extern void PSL1GHT_mousecallback(int button, int dx, int dy,
                               int u1, int u2, int u3, int u4);*/
extern void PSL1GHT_keyboardcallback(int scancode, int pressed);
extern void PSL1GHT_InitPSL1GHTKeymap(_THIS);
extern void PSL1GHT_PumpKeyboard();
/* vi: set ts=4 sw=4 expandtab: */
