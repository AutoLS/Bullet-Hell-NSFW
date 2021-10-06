#ifndef PLATFORM_H
#define PLATFORM_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_TTF.h>
#include <SDL_Mixer.h>
#include <stdio.h>
#include <windows.h>
#include <stdint.h>
#include "math.h"
#include <string>
#include <assert.h>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef int32 bool32;
typedef float real32;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

const uint64 PerformanceFrequency = SDL_GetPerformanceFrequency();
const real32 SecPerCount = 1.0f / (real32)PerformanceFrequency;

using namespace std;

enum keys
{
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_DOWN,
	KEY_UP,
	KEY_ESC,
	KEY_SPACE,
	KEY_BACKSPACE,
	KEY_LCTRL,
	KEY_LSHIFT,
	MAX_KEY,
};

enum mouse_button
{
	BUTTON_LEFT,
	BUTTON_MIDDLE,
	BUTTON_RIGHT,
	BUTTON_MAX,
};

enum lib_type
{
	LIB_SDL = 0x1,
	LIB_SDL_FULL = 0x2,
	LIB_OPENGL = 0x4,
};

struct render
{
	SDL_Window* Window;
	SDL_Renderer* Renderer;
	v2i WinDim;
};

struct image_data
{
	v2i Dim;
	SDL_Rect Rect;
	SDL_Texture* Texture;
	
	uint8 a;
	uint8 r;
	uint8 g;
	uint8 b;
	bool Transparency;
};

struct input
{	
	bool KeyIsDown;

	bool KeyDown[MAX_KEY];
	bool KeyWasDown[MAX_KEY];
	
	bool MouseButtonDown[BUTTON_MAX];
	bool MouseButtonWasDown[BUTTON_MAX];
};

struct game_button_state
{
	bool IsDown;
	bool EndedDown;
	int HalfTransitions;
};

struct game_controller_input
{
	game_button_state Keys[MAX_KEY];
};

#include "timer.h"

struct game_text_input
{
	bool Active;
	string Text;
	timer Timer;
};

struct game_input
{
	game_controller_input Keyboard;
	bool KeyDown;
	const Uint8* KeyState;
	
	game_text_input TextInput;
};

struct stack
{
	uint32 Size;
	uint32 Capacity;
	int* MyStack;
};

//Init libraries and graphics
bool InitLib(uint8 Flags);
bool InitGraphics(render* Graphics, char* Title, v2i Dim, Uint32 Flags);

//Window
int Win32GetWindowRefreshRate(SDL_Window *Window);
void PrintFPS(real32 FPS);
bool HandleEvents(SDL_Event* Event, game_controller_input* KeyboardInput);

//Inputs
void Win32ProcessKeyPress(game_button_state* NewState, bool IsDown);
bool IsKeyPressed(input* Input, keys Key);
bool IsButtonPressed(input* Input, mouse_button Button);

//Render
void LoadTexture(image_data* Image, char* Path, SDL_Renderer* Renderer, uint8 Alpha = 255);
void LoadAndUpdateTexture(image_data* Image, char* Path, SDL_Rect* Rect = 0);

//Rects
SDL_Rect SetRectRounded(v2 Pos, v2 Dim);

//Utility

#endif