#include "platform.h"

bool InitLib(uint8 Flags)
{
	bool Success = true;
	if(Flags == 0)
	{
		printf("Error: Cannot set flags to 0\n");
		return false;
	}
	if(Flags & LIB_SDL)
	{
		if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0) 
		{
			printf("Unable to initialize SDL: %s\n", SDL_GetError());
			Success = false;
		}
	}
	if(Flags & LIB_SDL_FULL)
	{
		if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) != 0) 
		{
			printf("Unable to initialize SDL: %s\n", SDL_GetError());
			Success = false;
		}
		else
		{
			int Flags = IMG_INIT_JPG|IMG_INIT_PNG;
			int Initted = IMG_Init(Flags);
			if((Initted&Flags) != Flags) 
			{
				printf("IMG_Init: Failed to init required jpg and png support!\n");
				printf("IMG_Init: %s\n", IMG_GetError());
				Success = false;
			}
			int MusicFlags = MIX_INIT_OGG | MIX_INIT_MOD;
			int MixInitted = Mix_Init(MusicFlags);
			if(MixInitted && MusicFlags != MusicFlags) 
			{
				printf("Mix_Init: Failed to init required ogg and mod support!\n");
				printf("Mix_Init: %s\n", Mix_GetError());
				Success = false;
			}
			else
			{
				if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) 
				{
					printf("Mix_OpenAudio: %s\n", Mix_GetError());
					Success = false;
				}
			}
			if(TTF_Init() == -1) 
			{
				printf("TTF_Init: %s\n", TTF_GetError());
				Success = false;
			}
		}
	}
	return Success;
}

bool InitGraphics(render* Graphics, char* Title, v2i Dim, Uint32 Flags)
{
	bool Success = true;
	Graphics->WinDim = Dim;
	Graphics->Window = SDL_CreateWindow(Title, 
										SDL_WINDOWPOS_UNDEFINED, 
										SDL_WINDOWPOS_UNDEFINED, 
										Dim.x, Dim.y,
										Flags);
	if(!Graphics->Window)
	{
		printf("Create window failed: %s\n", SDL_GetError());
		Success = false;
	}
	else
	{
		Graphics->Renderer = SDL_CreateRenderer(Graphics->Window, -1, SDL_RENDERER_ACCELERATED);
		if(!Graphics->Renderer)
		{
			printf("Create Renderer failed: %s\n", SDL_GetError());
			Success = false;
		}
	}
	return Success;
}

int Win32GetWindowRefreshRate(SDL_Window *Window)
{
    SDL_DisplayMode Mode;
    int DisplayIndex = SDL_GetWindowDisplayIndex(Window);
    // If we can't find the refresh rate, we'll return this:
    int DefaultRefreshRate = 60;
    if (SDL_GetDesktopDisplayMode(DisplayIndex, &Mode) != 0)
    {
        return DefaultRefreshRate;
    }
    if (Mode.refresh_rate == 0)
    {
        return DefaultRefreshRate;
    }
    return Mode.refresh_rate;
}

void Push(stack* Stack, int Value)
{
	if(Stack->Size + 1 > Stack->Capacity)
	{
		Stack->Capacity *= 5;
		int* NewStack = (int*)malloc(sizeof(int) * Stack->Capacity);
		memcpy(NewStack, Stack->MyStack, Stack->Size * sizeof(int));
		free(Stack->MyStack);
		Stack->MyStack = NewStack;
		Stack->MyStack[Stack->Size] = Value;
		++Stack->Size;
	}
	else
	{
		Stack->MyStack[Stack->Size] = Value;
		++Stack->Size;
	}
	//printf("Index: %d Push value: %d \n", Stack->Size - 1, Stack->MyStack[Stack->Size - 1]);
}

void Pop(stack* Stack)
{
	if(Stack->Size > 0)
	{
		//printf("Index: %d Pop value: %d\n", Stack->Size, Stack->MyStack[Stack->Size - 1]);
		--Stack->Size;
	}	
	else
		printf("Stack size is 0!\n");
}

int Peek(stack* Stack)
{
	if(Stack->Size == 0)
		return 0;
	//printf("Index: %d Value: %d Capacity: %d\n", Stack->Size - 1, 
			//Stack->MyStack[Stack->Size-1], Stack->Capacity);
	return Stack->MyStack[Stack->Size-1];
}

bool LockFPS(uint64 LastCount, real32 TargetSecPerFrame)
{
	bool Result = false;
	real32 FrameCompleteTime = Win32GetSecondElapsed(LastCount, SDL_GetPerformanceCounter());
	if(FrameCompleteTime < TargetSecPerFrame)
	{
		int SleepTime = (int)((TargetSecPerFrame - Win32GetSecondElapsed(LastCount, 						SDL_GetPerformanceCounter())) * 1000) - 1;
		if(SleepTime > 0)
		{
			Sleep(SleepTime);
		}
		//assert(Win32GetSecondElapsed(LastCount, SDL_GetPerformanceCounter()) < TargetSecPerFrame);
		while(Win32GetSecondElapsed(LastCount, SDL_GetPerformanceCounter()) < TargetSecPerFrame);
	}
	else
	{
		Result = true;
		//printf("Missed a frame!\n");
	}
	return Result;
}

void PrintFPS(real32 FPS)
{
	printf("FPS: %.2f\n", FPS);
}

bool HandleEvents(SDL_Event* Event, game_input* Input)
{
	bool Quit = false;
	switch(Event->type)
	{
		case SDL_QUIT:
		{
			Quit = true;
		} break;
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		{
			bool IsDown = (Event->key.state == SDL_PRESSED);
			bool WasDown = false;
			if(Event->key.state == SDL_RELEASED)
			{
				WasDown = true;
			}
			else if(Event->key.repeat != 0)
			{
				WasDown = true;
			}
			
			//bool Down = Event->type == SDL_KEYDOWN;
			Input->KeyDown = IsDown;
			
			if(Event->key.repeat == 0)
			{
				switch(Event->key.keysym.sym)
				{
					case SDLK_a: Win32ProcessKeyPress(&Input->Keyboard.Keys[KEY_A], IsDown); break;
					
					case SDLK_b: break;
					
					case SDLK_c: break;
					
					case SDLK_d: Win32ProcessKeyPress(&Input->Keyboard.Keys[KEY_D], IsDown); break;
					
					case SDLK_e: break;
					
					case SDLK_f: break;
					
					case SDLK_g: break;
					
					case SDLK_h: break;
					
					case SDLK_i: break;
					
					case SDLK_j: break;
					
					case SDLK_k: break;
					
					case SDLK_l: break;
					
					case SDLK_m: break;
					
					case SDLK_n: break;
					
					case SDLK_o: break;
					
					case SDLK_p: break;
					
					case SDLK_q: break;
					
					case SDLK_r: Win32ProcessKeyPress(&Input->Keyboard.Keys[KEY_R], IsDown); break;
					
					case SDLK_s: Win32ProcessKeyPress(&Input->Keyboard.Keys[KEY_S], IsDown); break;
					
					case SDLK_t: break;
					
					case SDLK_u: break;
					
					case SDLK_v: break;
					
					case SDLK_w: Win32ProcessKeyPress(&Input->Keyboard.Keys[KEY_W], IsDown); break;
					
					case SDLK_x: break;
					
					case SDLK_y: break;
					
					case SDLK_z: break;
					
					case SDLK_UP: Win32ProcessKeyPress(&Input->Keyboard.Keys[KEY_UP], IsDown); break;
					
					case SDLK_DOWN: Win32ProcessKeyPress(&Input->Keyboard.Keys[KEY_DOWN], IsDown); break;
					
					case SDLK_LCTRL: Win32ProcessKeyPress(&Input->Keyboard.Keys[KEY_LCTRL], IsDown); break;
					
					case SDLK_BACKSPACE: Win32ProcessKeyPress(&Input->Keyboard.Keys[KEY_BACKSPACE], 
																IsDown); break;
					
					case SDLK_SPACE: Win32ProcessKeyPress(&Input->Keyboard.Keys[KEY_SPACE], IsDown); break;
				}
			}
		} break;
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		{
#if 0
			bool Down = Event.type == SDL_MOUSEBUTTONDOWN;
			switch(Event->button.button)
			{
				case SDL_BUTTON_LEFT: Input.MouseButtonDown[BUTTON_LEFT] = Down;
				Input.MouseButtonWasDown[BUTTON_LEFT] = Event->type == SDL_MOUSEBUTTONUP; break;
				case SDL_BUTTON_MIDDLE: Input.MouseButtonDown[BUTTON_MIDDLE] = Down;
				Input.MouseButtonWasDown[BUTTON_MIDDLE] = Event->type == SDL_MOUSEBUTTONUP; break;
				case SDL_BUTTON_RIGHT: Input.MouseButtonDown[BUTTON_RIGHT] = Down;
				Input.MouseButtonWasDown[BUTTON_RIGHT] = Event->type == SDL_MOUSEBUTTONUP; break;
			}
#endif
		} break;
	}
	
	Input->TextInput.Active = Event->type == SDL_TEXTINPUT;
	
	return Quit;
}

void ProcessTextInput(game_input* Input, SDL_Event* Event)
{
	if(Input->TextInput.Timer.Complete)
	{
		if(Input->Keyboard.Keys[KEY_BACKSPACE].IsDown)
		{
			if(Input->TextInput.Text.length() > 0)
			{
				Input->TextInput.Text.pop_back();
			}
		}
		else
		{
			if(Input->TextInput.Active)
				Input->TextInput.Text += Event->text.text;
		}
		if(Input->TextInput.Text.length() > 0)
			printf("%s\n", Input->TextInput.Text.c_str());
	}
}

void Win32ProcessKeyPress(game_button_state* NewState, bool IsDown)
{
	//assert(NewState->EndedDown != IsDown);
	NewState->IsDown = IsDown;
	NewState->EndedDown = IsDown;
	++NewState->HalfTransitions;
}

bool IsKeyPressed(input* Input, keys Key)
{
	bool Result = !Input->KeyDown[Key] && Input->KeyWasDown[Key];
	if(Result) Input->KeyWasDown[Key] = false;
	return Result;
}

bool IsButtonPressed(input* Input, mouse_button Button)
{
	bool Result = !Input->MouseButtonDown[Button] && Input->MouseButtonWasDown[Button];
	if(Result) Input->MouseButtonWasDown[Button] = false;
	return Result;
}

void LoadText(SDL_Renderer* Renderer, TTF_Font* Font, image_data* Image, 
				const char* Text, SDL_Color Color, uint8 Alpha = 255)
{
	if(Text)
	{
		SDL_Surface* Surface = TTF_RenderText_Solid(Font, Text, Color);
		if(!Surface)
		{
			printf("Loading text failed! TTF_Error: %s\n", TTF_GetError());
		}
		else
		{
			Image->Dim.x = Surface->w;
			Image->Dim.y = Surface->h;
			Image->Rect = {0, 0, Surface->w, Surface->h};
			Image->Texture = SDL_CreateTextureFromSurface(Renderer, Surface);
			Image->a = Alpha;
			Image->Transparency = true;
			if(!Image->Texture)
			{
				printf("Creating texture failed! SDL_Error: %s\n", SDL_GetError());
			}
			else
			{
				SDL_SetTextureBlendMode(Image->Texture, SDL_BLENDMODE_BLEND);
				SDL_SetTextureAlphaMod(Image->Texture, Alpha);
			}
		}
		SDL_FreeSurface(Surface);
	}
}

void LoadTexture(image_data* Image, char* Path, SDL_Renderer* Renderer, uint8 Alpha)
{
	SDL_Surface* Surface = IMG_Load(Path);
	if(!Surface)
	{
		printf("Loading image failed! IMG_Error: %s\n", IMG_GetError());
	}
	else
	{
		Image->Dim.x = Surface->w;
		Image->Dim.y = Surface->h;
		Image->Rect = {0, 0, Surface->w, Surface->h};
		Image->Texture = SDL_CreateTextureFromSurface(Renderer, Surface);
		Image->a = Alpha;
		Image->Transparency = true;
		if(!Image->Texture)
		{
			printf("Creating texture failed! SDL_Error: %s\n", SDL_GetError());
		}
		else
		{
			SDL_GetTextureColorMod(Image->Texture, &Image->r, &Image->g, &Image->b);
			SDL_SetTextureBlendMode(Image->Texture, SDL_BLENDMODE_BLEND);
			SDL_SetTextureAlphaMod(Image->Texture, Alpha);
		}
	}
	SDL_FreeSurface(Surface);
}

void LoadAndUpdateTexture(image_data* Image, char* Path, SDL_Rect* Rect)
{
	SDL_Surface* Surface = IMG_Load(Path);
	if(!Surface)
	{
		printf("Loading image failed! IMG_Error: %s\n", IMG_GetError());
	}
	else
	{
		Image->Dim.x = Surface->w;
		Image->Dim.y = Surface->h;
		Image->Rect = {0, 0, Surface->w, Surface->h};
		if(!SDL_UpdateTexture(Image->Texture, Rect, Surface->pixels, Surface->pitch))
		{
			printf("Update texture failed. Error: %s\n", SDL_GetError());
		}
	}
	SDL_FreeSurface(Surface);
}

SDL_Rect SetRectRounded(v2 Pos, v2 Dim)
{
	SDL_Rect Result = {Round32(Pos.x), Round32(Pos.y), Round32(Dim.x), Round32(Dim.y)};
	return Result;
}