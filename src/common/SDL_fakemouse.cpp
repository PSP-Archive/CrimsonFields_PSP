/*  

	SDL_fakemouse.c

	public domain.

	rinco 2005

	Modified for Crimson fields by Crust/Amnesty

*/

#ifdef __cplusplus
extern "C"
{
#endif

#include "SDL_fakemouse.h"

short fake_mouse_dx = 0;
short fake_mouse_dy = 0;
SDL_sem *fake_mouse_sem = NULL;
SDL_Thread *fake_mouse_thread = NULL;
int fake_mouse_running = 0;

void fakemouse_event(SDL_Event ev)
{
	int x;
	int y;

	bool key_pressed_set = true;
	bool key_released_set = true;

	SDL_keysym key;

	SDL_GetMouseState(&x, &y);

	switch(ev.type)
	{
		case SDL_JOYAXISMOTION:
		{
			key_pressed_set = false;
			key_released_set = false;

			SDL_SemWait(fake_mouse_sem);

			switch(ev.jaxis.axis)
			{
				case 0:
				{
					fake_mouse_dx = ev.jaxis.value >> 13;
					break;
				}
				case 1:
				{
					fake_mouse_dy = ev.jaxis.value >> 13;
					break;
				}
			}

			SDL_SemPost(fake_mouse_sem);

			break;
		}
		case SDL_JOYBUTTONUP:
		{
			key_pressed_set = false;

			switch(ev.jbutton.button)
			{
				// left mouse
				case JOY_LTRIGGER:
				{
					key_released_set = false;
					SDL_PrivateMouseButton(SDL_RELEASED, SDL_BUTTON_LEFT, x, y);
					break;
				}
				// right mouse
				case JOY_RTRIGGER:
				{
					key_released_set = false;
					SDL_PrivateMouseButton(SDL_RELEASED, SDL_BUTTON_RIGHT, x, y);
					break;
				}
				// middle mouse
				case JOY_TRIANGLE:
				{
					key_released_set = false;
					SDL_PrivateMouseButton(SDL_RELEASED, SDL_BUTTON_MIDDLE, x, y);
					break;
				}
				//keyboard part
				case JOY_UP:
				{
					key.sym = SDLK_UP;
					break;
				}
				case JOY_DOWN:
				{
					key.sym = SDLK_DOWN;
					break;
				}
				case JOY_LEFT:
				{
					key.sym = SDLK_LEFT;
					break;
				}
				case JOY_RIGHT:
				{
					key.sym = SDLK_RIGHT;
					break;
				}
				case JOY_CROSS:
				{
					key.sym = SDLK_RETURN;
					break;
				}
				case JOY_CIRCLE:
				{
					key.sym = SDLK_F2;
					break;
				}
				case JOY_SQUARE:
				{
					key.sym = SDLK_SPACE;
					break;
				}
				case JOY_SELECT:
				{
					key.sym = SDLK_F1;
					break;
				}
				case JOY_START:
				{
					key.sym = SDLK_ESCAPE;
					break;
				}
				default:
				{
					key_released_set = false;
					SDL_PrivateMouseButton(SDL_RELEASED, 1, x, y);
					break;
				}
			}

			break;
		}
		case SDL_JOYBUTTONDOWN:
		{
			key_released_set = false;

			switch(ev.jbutton.button)
			{
				// left mouse
				case JOY_LTRIGGER:
				{
					key_pressed_set = false;
					SDL_PrivateMouseButton(SDL_PRESSED, SDL_BUTTON_LEFT, x, y);
					break;
				}
				// right mouse
				case JOY_RTRIGGER:
				{
					key_pressed_set = false;
					SDL_PrivateMouseButton(SDL_PRESSED, SDL_BUTTON_RIGHT, x, y);
					break;
				}
				// middle mouse
				case JOY_TRIANGLE:
				{
					key_pressed_set = false;
					SDL_PrivateMouseButton(SDL_PRESSED, SDL_BUTTON_MIDDLE, x, y);
					break;
				}
				//keyboard part
				case JOY_UP:
				{
					key.sym = SDLK_UP;
					break;
				}
				case JOY_DOWN:
				{
					key.sym = SDLK_DOWN;
					break;
				}
				case JOY_LEFT:
				{
					key.sym = SDLK_LEFT;
					break;
				}
				case JOY_RIGHT:
				{
					key.sym = SDLK_RIGHT;
					break;
				}
				case JOY_CROSS:
				{
					key.sym = SDLK_RETURN;
					break;
				}
				case JOY_CIRCLE:
				{
					key.sym = SDLK_F2;
					break;
				}
				case JOY_SQUARE:
				{
					key.sym = SDLK_SPACE;
					break;
				}
				case JOY_SELECT:
				{
					key.sym = SDLK_F1;
					break;
				}
				case JOY_START:
				{
					key.sym = SDLK_ESCAPE;
					break;
				}
				default:
				{
					key_pressed_set = false;
					SDL_PrivateMouseButton(SDL_PRESSED, 1, x, y);
					break;
				}
			}

			break;
		}
		default:
		{
			key_pressed_set = false;
			key_released_set = false;
			break;
		}
	}

	if(key_pressed_set)
	{
		SDL_PrivateKeyboard(SDL_PRESSED, &key);
	}

	if(key_released_set)
	{
		SDL_PrivateKeyboard(SDL_RELEASED, &key);
	}

}

int fakemouse_update(void *data)
{
	while(fake_mouse_running)
	{
		SDL_SemWait(fake_mouse_sem);

		SDL_Delay(1000 / 60);
		
		if(fake_mouse_dx ||  fake_mouse_dy)
		{
			SDL_PrivateMouseMotion(0, 1, fake_mouse_dx, fake_mouse_dy);
		}

		SDL_SemPost(fake_mouse_sem);
	}

	return 0;
}

void fakemouse_init(void)
{
	if((fake_mouse_sem =  SDL_CreateSemaphore(1)) == NULL)
	{
		SDL_SetError("Can't create fakemouse semaphore\n");
		return;
	}

	fake_mouse_running = 1;

	if((fake_mouse_thread = SDL_CreateThread(fakemouse_update, NULL)) == NULL)
	{
		SDL_SetError("Can't create fakemouse thread\n");
		return;
	}
}

void fakemouse_finish(void)
{
	fake_mouse_running = 0;
	SDL_WaitThread(fake_mouse_thread, NULL);
	SDL_DestroySemaphore(fake_mouse_sem);
}
 
#ifdef __cplusplus
}
#endif
