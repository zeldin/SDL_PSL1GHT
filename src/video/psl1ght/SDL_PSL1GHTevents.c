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
#include "SDL_compat.h"

/* Being a null driver, there's no event stream. We just define stubs for
   most of the API. */

#include "../../events/SDL_sysevents.h"
#include "../../events/SDL_events_c.h"
#include "../../events/SDL_keyboard_c.h"

#include "SDL_PSL1GHTkeys.h"
#include "SDL_PSL1GHTvideo.h"
#include "SDL_PSL1GHTevents_c.h"
#include "SDL_PSL1GHTmouse_c.h"

#include <sysutil/sysutil.h>
#include <io/kb.h>

#include "SDL_timer.h"
#include "SDL_events.h"
#include "../../events/SDL_events_c.h"
#include "../../events/SDL_sysevents.h"

#include "keyboard.h"

#define NUM_PSL1GHTKEYMAPS	(1<<KG_CAPSSHIFT)
static Uint16 PSL1GHT_keymap[NUM_PSL1GHTKEYMAPS][NR_KEYS];
static SDLKey keymap[255];

static SDL_keysym *TranslateKey(int scancode, SDL_keysym * keysym);
int SDL_TranslateUNICODE = 0;
int posted = 0;

static void eventHandle(u64 status, u64 param, void * userdata) {
    _THIS = userdata;
    SDL_Window *window = NULL;

    // There should only be one window
    if (_this->num_displays == 1) {
        SDL_VideoDisplay *display = &_this->displays[0];
        if (display->windows != NULL) {
            window = display->windows;
        }
    }

    // Process event
    if (status == SYSUTIL_EXIT_GAME) {
	    deprintf(1, "Quit game requested\n");
	    SDL_SendQuit();
    } else if(status == SYSUTIL_MENU_OPEN) {
	    // XMB opened
	    if (window) {
	        SDL_SendWindowEvent(window, SDL_WINDOWEVENT_LEAVE, 0, 0);
	    }
    } else if(status == SYSUTIL_MENU_CLOSE) {
		// XMB closed
	    if (window) {
	        SDL_SendWindowEvent(window, SDL_WINDOWEVENT_ENTER, 0, 0);
	    }
    } else if(status == SYSUTIL_DRAW_BEGIN) {
    } else if(status == SYSUTIL_DRAW_END) {
    } else {
	    deprintf(1, "Unhandled event: %08llX\n", (unsigned long long int)status);
    }
}

void
PSL1GHT_PumpEvents(_THIS)
{
	do {
        posted = 0;
		/*Keyboard update stuff goes here*/
		PSL1GHT_PumpKeyboard();

    } while (posted);
    sysUtilCheckCallback();
    PSL1GHT_PumpMouse(_this);
}


void PSL1GHT_PumpKeyboard()
{
  //SDL_Keyboard *keyboard = &SDL_keyboard;
  KbData Keys;
  KbInfo kbinfo;
  //KbMkey mKey;
  SDL_Keyboard *keyboard = &SDL_keyboard;

  keyboard->modstate = 0;
  s32 ret;
  int x = 0, z =0;

  static int tmp = 1;

  for (z=0; z<1;z++)
  {
	//set raw keyboard code types to get scan codes
	ret = ioKbSetCodeType (z, KB_CODETYPE_RAW);
	if (ioKbRead(z,&Keys) == 0)
	{
		//reset keys if nothing is pressed
		if (Keys.nb_keycode > 0)
			SDL_ResetKeyboard();
		//read Keys.
		/* Left Shift */
		if (Keys.mkey._KbMkeyU._KbMkeyS.l_shift == 1)
		{
			keyboard->modstate = keyboard->modstate | ~(1<<1);
			ret = PSLIGHT_SendKeyboardKey(SDL_PRESSED, SDL_SCANCODE_LSHIFT, keyboard->modstate);
		}
		else if (Keys.mkey._KbMkeyU._KbMkeyS.l_shift == 0)
		{
			keyboard->modstate = keyboard->modstate & 1<<1;
		}
		
		/* Right Shift */
		if (Keys.mkey._KbMkeyU._KbMkeyS.r_shift == 1)
		{
			keyboard->modstate = keyboard->modstate | ~(1<<5);
			ret = PSLIGHT_SendKeyboardKey(SDL_PRESSED, SDL_SCANCODE_RSHIFT, keyboard->modstate);
		}
		else if (Keys.mkey._KbMkeyU._KbMkeyS.r_shift == 1)
		{
			keyboard->modstate = keyboard->modstate & 1<<5;
		}

		/* Left Control */
		if (Keys.mkey._KbMkeyU._KbMkeyS.l_ctrl == 1)
		{
			keyboard->modstate = keyboard->modstate | ~(1<<0);
			ret = PSLIGHT_SendKeyboardKey(SDL_PRESSED, SDL_SCANCODE_LCTRL, keyboard->modstate);
		}
		else if (Keys.mkey._KbMkeyU._KbMkeyS.l_ctrl == 1)
		{
			keyboard->modstate = keyboard->modstate & 1<<0;
		}

		/* Right Control */
		if (Keys.mkey._KbMkeyU._KbMkeyS.r_ctrl == 1)
		{
			keyboard->modstate = keyboard->modstate | ~(1<<4);
			ret = PSLIGHT_SendKeyboardKey(SDL_PRESSED, SDL_SCANCODE_RCTRL, keyboard->modstate);
		}
		else if (Keys.mkey._KbMkeyU._KbMkeyS.r_ctrl == 1)
		{
			keyboard->modstate = keyboard->modstate & 1<<4;
		}

		for (x=0;x<Keys.nb_keycode;x++)
		{
			if (Keys.keycode[x] != 0)
				ret = PSLIGHT_SendKeyboardKey(SDL_PRESSED, (SDL_scancode) (Keys.keycode[x] ), keyboard->modstate);
		}
	}
  }
}

void
PSL1GHT_InitSysEvent(_THIS)
{
    sysUtilRegisterCallback(SYSUTIL_EVENT_SLOT0, eventHandle, _this);
    PSL1GHT_InitMouse(_this);
}

void
PSL1GHT_QuitSysEvent(_THIS)
{
    sysUtilUnregisterCallback(SYSUTIL_EVENT_SLOT0);
    PSL1GHT_QuitMouse(_this);
}

int
PSL1GHT_initkeymaps(int fd)
{
    struct kbentry entry;
    int map, i;

    /* Load all the keysym mappings */
    for (map = 0; map < NUM_PSL1GHTKEYMAPS; ++map) {
		SDL_memset(PSL1GHT_keymap[map], 0, NR_KEYS * sizeof(Uint16));
		for (i = 0; i < NR_KEYS; ++i) {
		    entry.kb_table = map;
		       entry.kb_index = i;
	           if (1) {  //CSNOTE:ioctl needs replaced!
				/* The "Enter" key is a special case */
				if (entry.kb_value == K_ENTER) {
			        entry.kb_value = K(KT_ASCII, 13);
		        }
		           /* Handle numpad specially as well */
	               if (KTYP(entry.kb_value) == KT_PAD) {
			        switch (entry.kb_value) {
		            case K_P0:
		            case K_P1:
	                case K_P2:
					case K_P3:
				    case K_P4:
			        case K_P5:
		            case K_P6:
		            case K_P7:
	                case K_P8:
					case K_P9:
					    PSL1GHT_keymap[map][i] = entry.kb_value;
				        PSL1GHT_keymap[map][i] += '0';
			            break;
		            case K_PPLUS:
	                    PSL1GHT_keymap[map][i] = K(KT_ASCII, '+');
						break;
					case K_PMINUS:
					    PSL1GHT_keymap[map][i] = K(KT_ASCII, '-');
				        break;
			        case K_PSTAR:
		                PSL1GHT_keymap[map][i] = K(KT_ASCII, '*');
	                    break;
					case K_PSLASH:
					    PSL1GHT_keymap[map][i] = K(KT_ASCII, '/');
				        break;
			        case K_PENTER:
		                PSL1GHT_keymap[map][i] = K(KT_ASCII, '\r');
		                break;
	                case K_PCOMMA:
					    PSL1GHT_keymap[map][i] = K(KT_ASCII, ',');
				        break;
			        case K_PDOT:
		                PSL1GHT_keymap[map][i] = K(KT_ASCII, '.');
	                    break;
                    default:
					    break;
				    }
			    }
		        /* Do the normal key translation */
	            if ((KTYP(entry.kb_value) == KT_LATIN) ||
                    (KTYP(entry.kb_value) == KT_ASCII) ||
					(KTYP(entry.kb_value) == KT_LETTER)) {
						PSL1GHT_keymap[map][i] = entry.kb_value;
				    }
			    }
		    }
	    }
		return (0);
	}



static SDL_keysym
*TranslateKey(int scancode, SDL_keysym * keysym)
{
    /* Set the keysym information */
    keysym->scancode = scancode;
    keysym->sym = keymap[scancode];
    keysym->mod = KMOD_NONE;

    /* If UNICODE is on, get the UNICODE value for the key */
    keysym->unicode = 0;
    if (SDL_TranslateUNICODE) {
        int map;
        SDLMod modstate;

        modstate = SDL_GetModState();
        map = 0;
        if (modstate & KMOD_SHIFT) {
            map |= (1 << KG_SHIFT);
        }
        if (modstate & KMOD_CTRL) {
            map |= (1 << KG_CTRL);
        }
        if (modstate & KMOD_ALT) {
            map |= (1 << KG_ALT);
        }
        if (modstate & KMOD_MODE) {
            map |= (1 << KG_ALTGR);
        }
        if (KTYP(PSL1GHT_keymap[map][scancode]) == KT_LETTER) {
            if (modstate & KMOD_CAPS) {
                map ^= (1 << KG_SHIFT);
            }
        }
        if (KTYP(PSL1GHT_keymap[map][scancode]) == KT_PAD) {
            if (modstate & KMOD_NUM) {
                keysym->unicode = KVAL(PSL1GHT_keymap[map][scancode]);
            }
        } else {
            keysym->unicode = KVAL(PSL1GHT_keymap[map][scancode]);
        }
    }
    return (keysym);
}
/* vi: set ts=4 sw=4 expandtab: */
