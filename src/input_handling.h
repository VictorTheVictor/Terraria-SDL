#ifndef INPUT_HANDLING_H_INCLUDED
#define INPUT_HANDLING_H_INCLUDED

#include <SDL.h>
#include <stdbool.h>

//TODO: HANDLE JUST THE INPUT. NOTHING ELSE. SIGNAL HANDLING ELSEWHERE

typedef enum KeyAction
{
	MOVE_LEFT,
	MOVE_RIGHT,
	JUMP,
	CROUCH,
	SPRINT,
	ZOOM_IN,
	ZOOM_OUT,

	INVENTORY,
	SHOW_DEBUG,
	PAUSE,
	HOTBAR_1,
	HOTBAR_2,
	HOTBAR_3,
	HOTBAR_4,
	HOTBAR_5,
	HOTBAR_6,
	HOTBAR_7,
	HOTBAR_8,
	HOTBAR_9,

	RESPAWN,
	RESET_CAMERA,

	ACTION_COUNT
} KeyAction;


static const SDLKey actionKeys[ACTION_COUNT] =
{
	[MOVE_LEFT]		= SDLK_a,
	[MOVE_RIGHT]	= SDLK_d,
	[JUMP]			= SDLK_SPACE,
	[CROUCH]		= SDLK_LCTRL,
	[SPRINT]		= SDLK_LSHIFT,
	[ZOOM_IN]		= SDLK_UP,
	[ZOOM_OUT]		= SDLK_DOWN,
	[INVENTORY]		= SDLK_e,
	[SHOW_DEBUG]	= SDLK_F3,
	[PAUSE]			= SDLK_ESCAPE,
	[HOTBAR_1]		= SDLK_1,
	[HOTBAR_2]		= SDLK_2,
	[HOTBAR_3]		= SDLK_3,
	[HOTBAR_4]		= SDLK_4,
	[HOTBAR_5]		= SDLK_5,
	[HOTBAR_6]		= SDLK_6,
	[HOTBAR_7]		= SDLK_7,
	[HOTBAR_8]		= SDLK_8,
	[HOTBAR_9]		= SDLK_9,
	[RESPAWN]		= SDLK_r,
	[RESET_CAMERA]	= SDLK_BACKSPACE,
};

typedef struct
{
	bool pressed;
	int pressed_timer;
} keyState_t;

keyState_t keyState[ACTION_COUNT];

void processKeyPress(SDLKey key)
{
	for (int action = 0; action < ACTION_COUNT; action++)
	{
		if (actionKeys[action] == key)
		{
			keyState[action].pressed = true;
			keyState[action].pressed = 0;
			break;
		}
	}
}

void processKeyRelease(SDLKey key)
{
	for (int action = 0; action < ACTION_COUNT; action++)
	{
		if (actionKeys[action] == key)
		{
			keyState[action].pressed = false;
			break;
		}
	}
}

void tickKey()
{
	for (int action = 0; action < ACTION_COUNT; action++)
	{
		if(keyState[action].pressed == true)
			keyState[action].pressed_timer++;
	}
}

#endif // INPUT_HANDLING_H_INCLUDED
