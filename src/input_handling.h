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

typedef enum actionProperties
{
	ON_HOLD, //Action runs while key is pressed
	ON_REPEAT, //Action runs when key is pressed, and holding it will repeatedly run it after a few seconds
	ON_PRESS, //Action runs when key is pressed
	ON_RELEASE //Action runs when key is released
} actionProperties;

typedef struct
{
	bool pressed;
	bool just_pressed;
	bool just_released;
	bool triggered;

	unsigned int pressed_timer;
	unsigned int next_repeat_tick;

	actionProperties action_properties;
	SDLKey key;
} keyProperties;

static keyProperties key_properties[ACTION_COUNT] =
{
	[MOVE_LEFT] = 		{false, false, false, false, 0, 0, ON_HOLD, SDLK_a},
	[MOVE_RIGHT] = 		{false, false, false, false, 0, 0, ON_HOLD, SDLK_d},
	[JUMP] = 			{false, false, false, false, 0, 0, ON_HOLD, SDLK_SPACE},
	[CROUCH] = 			{false, false, false, false, 0, 0, ON_HOLD, SDLK_LCTRL},
	[SPRINT] = 			{false, false, false, false, 0, 0, ON_HOLD, SDLK_LSHIFT},
	[ZOOM_IN] = 		{false, false, false, false, 0, 0, ON_HOLD, SDLK_UP},
	[ZOOM_OUT] = 		{false, false, false, false, 0, 0, ON_HOLD, SDLK_DOWN},

	[INVENTORY] = 		{false, false, false, false, 0, 0, ON_REPEAT, SDLK_e},
	[SHOW_DEBUG] = 		{false, false, false, false, 0, 0, ON_REPEAT, SDLK_F3},
	[PAUSE] = 			{false, false, false, false, 0, 0, ON_REPEAT, SDLK_ESCAPE},

	[HOTBAR_1] = 		{false, false, false, false, 0, 0, ON_PRESS, SDLK_1},
	[HOTBAR_2] = 		{false, false, false, false, 0, 0, ON_PRESS, SDLK_2},
	[HOTBAR_3] = 		{false, false, false, false, 0, 0, ON_PRESS, SDLK_3},
	[HOTBAR_4] = 		{false, false, false, false, 0, 0, ON_PRESS, SDLK_4},
	[HOTBAR_5] = 		{false, false, false, false, 0, 0, ON_PRESS, SDLK_5},
	[HOTBAR_6] = 		{false, false, false, false, 0, 0, ON_PRESS, SDLK_6},
	[HOTBAR_7] = 		{false, false, false, false, 0, 0, ON_PRESS, SDLK_7},
	[HOTBAR_8] = 		{false, false, false, false, 0, 0, ON_PRESS, SDLK_8},
	[HOTBAR_9] = 		{false, false, false, false, 0, 0, ON_PRESS, SDLK_9},

	[RESPAWN] = 		{false, false, false, false, 0, 0, ON_RELEASE, SDLK_r},
	[RESET_CAMERA] = 	{false, false, false, false, 0, 0, ON_RELEASE, SDLK_BACKSPACE}
};

void processKeyPress(SDLKey key)
{
	for (int action = 0; action < ACTION_COUNT; action++)
	{
		if (key_properties[action].key == key && !key_properties[action].pressed)
		{
			key_properties[action].pressed = true;
			key_properties[action].just_pressed = true;
			key_properties[action].pressed_timer = 0;
			key_properties[action].next_repeat_tick = 0;
			break;
		}
	}
}

void processKeyRelease(SDLKey key)
{
	for (int action = 0; action < ACTION_COUNT; action++)
	{
		if (key_properties[action].key == key && key_properties[action].pressed)
		{
			key_properties[action].pressed = false;
			key_properties[action].just_released = true;
			key_properties[action].pressed_timer = 0;
			key_properties[action].next_repeat_tick = 0;
			break;
		}
	}
}

void TickKeyboard()
{
	for (int action = 0; action < ACTION_COUNT; action++)
	{
		key_properties[action].triggered = false;

		switch (key_properties[action].action_properties)
		{
			case ON_HOLD:
			{
				key_properties[action].triggered = key_properties[action].pressed;
				break;
			}

			case ON_PRESS:
			{
				key_properties[action].triggered = key_properties[action].just_pressed;
				break;
			}

			case ON_RELEASE:
			{
				key_properties[action].triggered = key_properties[action].just_released;
				break;
			}

			case ON_REPEAT:
			{
				if (key_properties[action].just_pressed)
				{
					key_properties[action].triggered = true;
					key_properties[action].next_repeat_tick = 90;
				}
				else if (key_properties[action].pressed)
				{
					if (key_properties[action].pressed_timer >= key_properties[action].next_repeat_tick)
					{
						key_properties[action].triggered = true;
						key_properties[action].next_repeat_tick += 6;
					}
				}

				break;
			}
		}

		if (key_properties[action].pressed)
			key_properties[action].pressed_timer++;
		else
			key_properties[action].pressed_timer = 0;

		key_properties[action].just_pressed = false;
		key_properties[action].just_released = false;
	}
}

#endif // INPUT_HANDLING_H_INCLUDED
