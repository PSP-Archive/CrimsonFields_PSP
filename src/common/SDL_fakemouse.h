#ifndef _SDL_FAKEMOUSE_H
#define _SDL_FAKEMOUSE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "SDL.h"
#include "SDL_events.h"
#include "SDL_thread.h"
#include "SDL_timer.h"
#include "SDL_mutex.h"

enum
{
	JOY_TRIANGLE,
	JOY_CIRCLE,
	JOY_CROSS,
	JOY_SQUARE,
	JOY_LTRIGGER,
	JOY_RTRIGGER,
	JOY_DOWN,
	JOY_LEFT,
	JOY_UP,
	JOY_RIGHT,
	JOY_SELECT,
	JOY_START,
	JOY_HOME,
	JOY_HOLD,
};

extern int SDL_PrivateMouseMotion(Uint8 buttonstate, int relative, Sint16 x, Sint16 y);
extern int SDL_PrivateMouseButton(Uint8 state, Uint8 button, Sint16 x, Sint16 y);
extern int SDL_PrivateKeyboard(Uint8 state, SDL_keysym *key);

void fakemouse_event(SDL_Event ev);
int fakemouse_update(void *data);
void fakemouse_init(void);
void fakemouse_finish(void);

#ifdef __cplusplus
}
#endif
#endif// _SDL_FAKEMOUSE_H
