#include "../common.h"

#include "input.h"

extern App app;

void handleKeyDown(Input * input, SDL_KeyboardEvent * event)
{
    if ((event->repeat == 0) && (event->keysym.scancode < MAX_KEYBOARD_KEYS))
    {
        input->keyboardEvents[event->keysym.scancode] = 1;
    }
}

void handleKeyUp(Input * input, SDL_KeyboardEvent * event)
{
    if ((event->repeat == 0) && (event->keysym.scancode < MAX_KEYBOARD_KEYS))
    {
        input->keyboardEvents[event->keysym.scancode] = 0;
    }
}

void handleMouseDown(Input * input, SDL_MouseButtonEvent * event)
{
	if (event->button >= 0 && event->button < MAX_MOUSE_BUTTONS)
	{
		input->mouse.buttons[event->button] = 1;
	}
}

void handleMouseUp(Input * input, SDL_MouseButtonEvent * event)
{
	if (event->button >= 0 && event->button < MAX_MOUSE_BUTTONS)
	{
		input->mouse.buttons[event->button] = 0;
	}
}

#define NUM_HOLD_KEYS 4

void handleInput(Input * input, double timeElapsedSeconds)
{
    input->secElapsed = timeElapsedSeconds;
    
    SDL_Event event;

    while(SDL_PollEvent(&event))
    {
        switch(event.type)
        {
            case SDL_QUIT:
                app.run = false;
                break;
            case SDL_KEYDOWN:
                handleKeyDown(input, &event.key);
                break;
            case SDL_KEYUP:
                handleKeyUp(input, &event.key);
                break;
            case SDL_MOUSEBUTTONDOWN:
                handleMouseDown(input, &event.button);
                break;
            case SDL_MOUSEBUTTONUP:
                handleMouseUp(input, &event.button);
                break;
            default:
                break;
        }
    }

    const uint8_t* keyboardState = SDL_GetKeyboardState(NULL);
    
    int keys[NUM_HOLD_KEYS] = {SDL_SCANCODE_W, SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D};

    for(int i = 0; i < NUM_HOLD_KEYS; i++)
    {
        if (keyboardState[keys[i]] == 1)
        {
            input->keyboardState[keys[i]] = 1;
        }
    }

    SDL_GetMouseState(&input->mouse.p.x, &input->mouse.p.y);

    app.mouseScreenPosition.x = input->mouse.p.x;
    app.mouseScreenPosition.y = input->mouse.p.y; 
}

void clearInput(Input * input)
{
    SDL_Event event;

    memset(input, 0, sizeof(Input));

    while (SDL_PollEvent(&event)){}
}