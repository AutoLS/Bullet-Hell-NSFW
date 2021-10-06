#ifndef RENDER_H
#define RENDER_H

#include "platform.h"
#include "math.h"

//Y is negated!
//Origin is at the center!

void RenderCopy(render* Graphics, SDL_Texture* Texture, SDL_Rect* Clip, SDL_Rect* Rect)
{
	int AdjustX = Rect->x - (int)(Rect->w * 0.5) + (int)(Graphics->WinDim.x * 0.5f);
	int AdjustY = Rect->y + (int)(Rect->h * 0.5) - (int)(Graphics->WinDim.y * 0.5f);
	SDL_Rect RenderRect = {AdjustX, -AdjustY, Rect->w, Rect->h};
	SDL_RenderCopy(Graphics->Renderer, Texture, Clip, &RenderRect);
}

void RenderCopyEx(render* Graphics, SDL_Texture* Texture, SDL_Rect* Clip, SDL_Rect* Rect, 
					real32 Angle = 0, SDL_Point* Center = 0, SDL_RendererFlip Flip = SDL_FLIP_NONE)
{
	int AdjustX = Rect->x - (int)(Rect->w * 0.5) + (int)(Graphics->WinDim.x * 0.5f);
	int AdjustY = Rect->y + (int)(Rect->h * 0.5) - (int)(Graphics->WinDim.y * 0.5f);
	SDL_Rect RenderRect = {AdjustX, -AdjustY, Rect->w, Rect->h};
	SDL_RenderCopyEx(Graphics->Renderer, Texture, Clip, &RenderRect, -Angle, Center, Flip);
}

void RenderFillRect(render* Graphics, SDL_Rect* Rect, SDL_Color Color)
{
	SDL_SetRenderDrawColor(Graphics->Renderer, Color.r, Color.g, Color.b, Color.a);
	int AdjustX = Rect->x - (int)(Rect->w * 0.5) + (int)(Graphics->WinDim.x * 0.5f);
	int AdjustY = Rect->y + (int)(Rect->h * 0.5) + (int)(Graphics->WinDim.y * 0.5f);
	SDL_Rect RenderRect = {AdjustX, -AdjustY, Rect->w, Rect->h};
	SDL_RenderFillRect(Graphics->Renderer, &RenderRect);
}

void RenderDrawRect(render* Graphics, SDL_Rect* Rect, SDL_Color Color)
{
	SDL_SetRenderDrawColor(Graphics->Renderer, Color.r, Color.g, Color.b, Color.a);
	int AdjustX = Rect->x - (int)(Rect->w * 0.5) + (int)(Graphics->WinDim.x * 0.5f);
	int AdjustY = Rect->y + (int)(Rect->h * 0.5) + (int)(Graphics->WinDim.y * 0.5f);
	SDL_Rect RenderRect = {AdjustX, -AdjustY, Rect->w, Rect->h};
	SDL_RenderDrawRect(Graphics->Renderer, &RenderRect);
}


#endif