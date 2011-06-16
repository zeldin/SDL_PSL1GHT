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

#include "../fbcon/SDL_fbkeys.h"
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

//SDL_KeyboardEvent PSL1GHT_KeyboardEvent;
//SDL_Event PSL1GHT_Event;

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
			//ret = SDL_SendKeyboardKey(SDL_PRESSED, SDL_SCANCODE_LSHIFT);
			printf("Left Shift Pressed\n");
			//printf("Events: Modstate: %d\n", keyboard->modstate);
			keyboard->modstate = keyboard->modstate | ~(1<<1);
			ret = PSLIGHT_SendKeyboardKey(SDL_PRESSED, SDL_SCANCODE_LSHIFT, keyboard->modstate);
			//printf("Events: Modstate: %d\n", keyboard->modstate);
		}
		else if (Keys.mkey._KbMkeyU._KbMkeyS.l_shift == 0)
		{
			//ret = SDL_SendKeyboardKey(SDL_RELEASED, SDL_SCANCODE_LSHIFT);
			//printf("Events: Modstate: %d\n", keyboard->modstate);
			keyboard->modstate = keyboard->modstate & 1<<1;
			//printf("Events: Modstate: %d\n", keyboard->modstate);
		}
		
		/* Right Shift 
		if (Keys.mkey._KbMkeyU._KbMkeyS.r_shift == 1)
		{
			//ret = SDL_SendKeyboardKey(SDL_PRESSED, SDL_SCANCODE_LSHIFT);
			printf("Right Shift Pressed.\n");
			//printf("Events: Modstate: %d\n", keyboard->modstate);
			keyboard->modstate = keyboard->modstate | 1<<5;
			//printf("Events: Modstate: %d\n", keyboard->modstate);
		}
		else
		{
			//ret = SDL_SendKeyboardKey(SDL_RELEASED, SDL_SCANCODE_LSHIFT);
			//printf("Events: Modstate: %d\n", keyboard->modstate);
			keyboard->modstate = keyboard->modstate & 1<<5;
			//printf("Events: Modstate: %d\n", keyboard->modstate);
		}*/


		/*
		case SDL_SCANCODE_LSHIFT:
            keyboard->modstate |= KMOD_LSHIFT;
            break;
		*/
        

		/*
		case SDL_SCANCODE_LCTRL:
            keyboard->modstate |= KMOD_LCTRL;
            break;
        case SDL_SCANCODE_RCTRL:
            keyboard->modstate |= KMOD_RCTRL;
            break;
        case SDL_SCANCODE_RSHIFT:
            keyboard->modstate |= KMOD_RSHIFT;
            break;
        case SDL_SCANCODE_LALT:
            keyboard->modstate |= KMOD_LALT;
            break;
        case SDL_SCANCODE_RALT:
            keyboard->modstate |= KMOD_RALT;
            break;
        case SDL_SCANCODE_LGUI:
            keyboard->modstate |= KMOD_LGUI;
            break;
        case SDL_SCANCODE_RGUI:
            keyboard->modstate |= KMOD_RGUI;
            break;
		*/


		for (x=0;x<Keys.nb_keycode;x++)
		{
			printf("got Keys... pass: %d keyboard: %d - KeyCode: %d Modstate: %d\n",x, z, Keys.keycode[x],keyboard->modstate);
			if (Keys.keycode[x] != 0)
				ret = PSLIGHT_SendKeyboardKey(SDL_PRESSED, (SDL_scancode) (Keys.keycode[x] ), keyboard->modstate);
		}
	}
  }
}


void PSL1GHT_InitSysEvent(_THIS)
{
    sysUtilRegisterCallback(SYSUTIL_EVENT_SLOT0, eventHandle, _this);
    PSL1GHT_InitMouse(_this);
}

void PSL1GHT_QuitSysEvent(_THIS)
{
    sysUtilUnregisterCallback(SYSUTIL_EVENT_SLOT0);
    PSL1GHT_QuitMouse(_this);
}

void PSL1GHT_InitPSL1GHTKeymap(_THIS)
{
    int i;

    /* Initialize the BeOS key translation table */
    for (i = 0; i < SDL_arraysize(keymap); ++i)
        keymap[i] = SDLK_UNKNOWN;

    keymap[SCANCODE_ESCAPE] = SDLK_ESCAPE;
    keymap[SCANCODE_1] = SDLK_1;
    keymap[SCANCODE_2] = SDLK_2;
    keymap[SCANCODE_3] = SDLK_3;
    keymap[SCANCODE_4] = SDLK_4;
    keymap[SCANCODE_5] = SDLK_5;
    keymap[SCANCODE_6] = SDLK_6;
    keymap[SCANCODE_7] = SDLK_7;
    keymap[SCANCODE_8] = SDLK_8;
    keymap[SCANCODE_9] = SDLK_9;
    keymap[SCANCODE_0] = SDLK_0;
    keymap[SCANCODE_MINUS] = SDLK_MINUS;
    keymap[SCANCODE_EQUAL] = SDLK_EQUALS;
    keymap[SCANCODE_BACKSPACE] = SDLK_BACKSPACE;
    keymap[SCANCODE_TAB] = SDLK_TAB;
    keymap[SCANCODE_Q] = SDLK_q;
    keymap[SCANCODE_W] = SDLK_w;
    keymap[SCANCODE_E] = SDLK_e;
    keymap[SCANCODE_R] = SDLK_r;
    keymap[SCANCODE_T] = SDLK_t;
    keymap[SCANCODE_Y] = SDLK_y;
    keymap[SCANCODE_U] = SDLK_u;
    keymap[SCANCODE_I] = SDLK_i;
    keymap[SCANCODE_O] = SDLK_o;
    keymap[SCANCODE_P] = SDLK_p;
    keymap[SCANCODE_BRACKET_LEFT] = SDLK_LEFTBRACKET;
    keymap[SCANCODE_BRACKET_RIGHT] = SDLK_RIGHTBRACKET;
    keymap[SCANCODE_ENTER] = SDLK_RETURN;
    keymap[SCANCODE_LEFTCONTROL] = SDLK_LCTRL;
    keymap[SCANCODE_A] = SDLK_a;
    keymap[SCANCODE_S] = SDLK_s;
    keymap[SCANCODE_D] = SDLK_d;
    keymap[SCANCODE_F] = SDLK_f;
    keymap[SCANCODE_G] = SDLK_g;
    keymap[SCANCODE_H] = SDLK_h;
    keymap[SCANCODE_J] = SDLK_j;
    keymap[SCANCODE_K] = SDLK_k;
    keymap[SCANCODE_L] = SDLK_l;
    keymap[SCANCODE_SEMICOLON] = SDLK_SEMICOLON;
    keymap[SCANCODE_APOSTROPHE] = SDLK_QUOTE;
    keymap[SCANCODE_GRAVE] = SDLK_BACKQUOTE;
    keymap[SCANCODE_LEFTSHIFT] = SDLK_LSHIFT;
    keymap[SCANCODE_BACKSLASH] = SDLK_BACKSLASH;
    keymap[SCANCODE_Z] = SDLK_z;
    keymap[SCANCODE_X] = SDLK_x;
    keymap[SCANCODE_C] = SDLK_c;
    keymap[SCANCODE_V] = SDLK_v;
    keymap[SCANCODE_B] = SDLK_b;
    keymap[SCANCODE_N] = SDLK_n;
    keymap[SCANCODE_M] = SDLK_m;
    keymap[SCANCODE_COMMA] = SDLK_COMMA;
    keymap[SCANCODE_PERIOD] = SDLK_PERIOD;
    keymap[SCANCODE_SLASH] = SDLK_SLASH;
    keymap[SCANCODE_RIGHTSHIFT] = SDLK_RSHIFT;
    keymap[SCANCODE_KEYPADMULTIPLY] = SDLK_KP_MULTIPLY;
    keymap[SCANCODE_LEFTALT] = SDLK_LALT;
    keymap[SCANCODE_SPACE] = SDLK_SPACE;
    keymap[SCANCODE_CAPSLOCK] = SDLK_CAPSLOCK;
    keymap[SCANCODE_F1] = SDLK_F1;
    keymap[SCANCODE_F2] = SDLK_F2;
    keymap[SCANCODE_F3] = SDLK_F3;
    keymap[SCANCODE_F4] = SDLK_F4;
    keymap[SCANCODE_F5] = SDLK_F5;
    keymap[SCANCODE_F6] = SDLK_F6;
    keymap[SCANCODE_F7] = SDLK_F7;
    keymap[SCANCODE_F8] = SDLK_F8;
    keymap[SCANCODE_F9] = SDLK_F9;
    keymap[SCANCODE_F10] = SDLK_F10;
    keymap[SCANCODE_NUMLOCK] = SDLK_NUMLOCK;
    keymap[SCANCODE_SCROLLLOCK] = SDLK_SCROLLOCK;
    keymap[SCANCODE_KEYPAD7] = SDLK_KP7;
    keymap[SCANCODE_CURSORUPLEFT] = SDLK_KP7;
    keymap[SCANCODE_KEYPAD8] = SDLK_KP8;
    keymap[SCANCODE_CURSORUP] = SDLK_KP8;
    keymap[SCANCODE_KEYPAD9] = SDLK_KP9;
    keymap[SCANCODE_CURSORUPRIGHT] = SDLK_KP9;
    keymap[SCANCODE_KEYPADMINUS] = SDLK_KP_MINUS;
    keymap[SCANCODE_KEYPAD4] = SDLK_KP4;
    keymap[SCANCODE_CURSORLEFT] = SDLK_KP4;
    keymap[SCANCODE_KEYPAD5] = SDLK_KP5;
    keymap[SCANCODE_KEYPAD6] = SDLK_KP6;
    keymap[SCANCODE_CURSORRIGHT] = SDLK_KP6;
    keymap[SCANCODE_KEYPADPLUS] = SDLK_KP_PLUS;
    keymap[SCANCODE_KEYPAD1] = SDLK_KP1;
    keymap[SCANCODE_CURSORDOWNLEFT] = SDLK_KP1;
    keymap[SCANCODE_KEYPAD2] = SDLK_KP2;
    keymap[SCANCODE_CURSORDOWN] = SDLK_KP2;
    keymap[SCANCODE_KEYPAD3] = SDLK_KP3;
    keymap[SCANCODE_CURSORDOWNRIGHT] = SDLK_KP3;
    keymap[SCANCODE_KEYPAD0] = SDLK_KP0;
    keymap[SCANCODE_KEYPADPERIOD] = SDLK_KP_PERIOD;
    keymap[SCANCODE_LESS] = SDLK_LESS;
    keymap[SCANCODE_F11] = SDLK_F11;
    keymap[SCANCODE_F12] = SDLK_F12;
    keymap[SCANCODE_KEYPADENTER] = SDLK_KP_ENTER;
    keymap[SCANCODE_RIGHTCONTROL] = SDLK_RCTRL;
    keymap[SCANCODE_CONTROL] = SDLK_RCTRL;
    keymap[SCANCODE_KEYPADDIVIDE] = SDLK_KP_DIVIDE;
    keymap[SCANCODE_PRINTSCREEN] = SDLK_PRINT;
    keymap[SCANCODE_RIGHTALT] = SDLK_RALT;
    keymap[SCANCODE_BREAK] = SDLK_BREAK;
    keymap[SCANCODE_BREAK_ALTERNATIVE] = SDLK_UNKNOWN;
    keymap[SCANCODE_HOME] = SDLK_HOME;
    keymap[SCANCODE_CURSORBLOCKUP] = SDLK_UP;
    keymap[SCANCODE_PAGEUP] = SDLK_PAGEUP;
    keymap[SCANCODE_CURSORBLOCKLEFT] = SDLK_LEFT;
    keymap[SCANCODE_CURSORBLOCKRIGHT] = SDLK_RIGHT;
    keymap[SCANCODE_END] = SDLK_END;
    keymap[SCANCODE_CURSORBLOCKDOWN] = SDLK_DOWN;
    keymap[SCANCODE_PAGEDOWN] = SDLK_PAGEDOWN;
    keymap[SCANCODE_INSERT] = SDLK_INSERT;
    keymap[SCANCODE_REMOVE] = SDLK_DELETE;
    keymap[119] = SDLK_PAUSE;
    keymap[SCANCODE_RIGHTWIN] = SDLK_RSUPER;
    keymap[SCANCODE_LEFTWIN] = SDLK_LSUPER;
    keymap[127] = SDLK_MENU;
}

void PSL1GHT_keyboardcallback(int scancode, int pressed)
{
    SDL_keysym keysym;

    if (pressed) {
        posted += 65;/*SDL_PrivateKeyboard(SDL_PRESSED,
                                      TranslateKey(scancode, &keysym));*/
    } else {
        posted += 32;/*SDL_PrivateKeyboard(SDL_RELEASED,
                                      TranslateKey(scancode, &keysym));*/
    }
}

/*void PSL1GHT__mousecallback(int button, int dx, int dy, int u1, int u2, int u3, int u4)
{
	//  do nothing--- for now
}*/

int PSL1GHT_initkeymaps(int fd)
{
    struct kbentry entry;
    int map, i;

    /* Load all the keysym mappings */
    for (map = 0; map < NUM_PSL1GHTKEYMAPS; ++map) {
		SDL_memset(PSL1GHT_keymap[map], 0, NR_KEYS * sizeof(Uint16));
		for (i = 0; i < NR_KEYS; ++i) {
		    entry.kb_table = map;
		       entry.kb_index = i;
//	           if (ioctl(fd, KDGKBENT, &entry) == 0) {  //CSNOTE:ioctl needs replaced!
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



static SDL_keysym *TranslateKey(int scancode, SDL_keysym * keysym)
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
