#ifndef INPUT_H_
#define INPUT_H_

void handleKeyDown(Input * input, SDL_KeyboardEvent * event);
void handleKeyUp(Input * input, SDL_KeyboardEvent * event);
void handleMouseDown(Input * input, SDL_MouseButtonEvent * event);
void handleMouseUp(Input * input, SDL_MouseButtonEvent * event);
void handleInput(Input * input, double timeElapsedSeconds);
void clearInput(Input * input);
#endif