#ifndef ANIMATION_H
#define ANIMATION_H

#include "timer.h"
#include "physics.h"

enum player_animations
{
	PLAYER_IDLE_ANIMATION,
	PLAYER_RUN_ANIMATION,
	PLAYER_JUMP_ANIMATION,
	PLAYER_SLIDE_ANIMATION,
	PLAYER_ATTACK_ANIMATION,
	MAX_PLAYER_ANIMATION,
};

enum fireball_animation
{
	FIREBALL_SMALL_ANIMATION,
	MAX_FIREBALL_ANIMATION,
};

struct animation
{
	image_data* Sprite;
	SDL_Rect* Clips;
	int CurrentFrame;
	int Frames;
	timer Timer;
	bool Ended;
	bool Loop;
};

SDL_Rect* GetClipSlices(SDL_Rect* Clips, int StartIndex, int EndIndex)
{
	int Count = (EndIndex - StartIndex + 1);
	SDL_Rect* Result = (SDL_Rect*)malloc(sizeof(SDL_Rect) * Count);
	
	int Index = StartIndex;
	
	for(int i = 0; i < Count; ++i)
	{
		Result[i] = Clips[Index++];
	}
	
	return Result;
}

animation CreateAnimation(image_data* Sprite, SDL_Rect* Clips, int Frames, real32 SecPerFrame, 
							bool Loop = true)
{
	animation Animation = {};
	Animation.Sprite = Sprite;
	Animation.Clips = Clips;
	Animation.Frames = Frames;
	Animation.Timer = CreateTimer(SecPerFrame);
	Animation.Loop = Loop;
	
	return Animation;
}

void StartAnimation(animation* Animation)
{
	Animation->Ended = false;
}

void RenderAnimation(animation* Animation, render* Graphics, rigid_body* Body)
{
	if(Animation->Timer.Complete)
	{
		Animation->CurrentFrame++;
		if(Animation->CurrentFrame >= Animation->Frames)
		{
			Animation->CurrentFrame = 0;
			if(!Animation->Loop)
				Animation->Ended = true;
		}
	}
	
	if(Animation->Loop)
		RenderCopyEx(Graphics, Animation->Sprite->Texture, &Animation->Clips[Animation->CurrentFrame], 
						&Body->RenderRect, Body->Angle, 0, Body->Flip);
	else if(!Animation->Ended)
		RenderCopyEx(Graphics, Animation->Sprite->Texture, &Animation->Clips[Animation->CurrentFrame], 
						&Body->RenderRect, Body->Angle, 0, Body->Flip);
	else
	{
		RenderCopyEx(Graphics, Animation->Sprite->Texture, &Animation->Clips[Animation->Frames - 1], 
						&Body->RenderRect, Body->Angle, 0, Body->Flip);
	}
}

#endif