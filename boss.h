#ifndef BOSS_H
#define BOSS_H

enum boss_stage
{
	BOSS_TUTORIAL,
	BOSS_HENTAI,
};

enum boss_state
{
	BOSS_STATE_1,
	BOSS_STATE_2,
	BOSS_STATE_3,
	BOSS_STATE_4,
	BOSS_STATE_5,
};

#include "entity.h"
#include "timer.h"
#include "game.h"
#include "platform.h"

struct boss_initiallize_info
{
	int GroundIndex;
	int BossIndex;
	int* WallIndex;
	boss_stage Boss;
	boss_state BossState;
	game_state LastState;
	Mix_Music* BossMusic;
	SDL_Rect HPBar;
	SDL_Rect HPBarFill;
};

boss_initiallize_info 
InitializeBossStage(entity_pool* Pool, timer* SpawnTimer, image_data* GameAsset, Mix_Music** SoundTracks,
					boss_stage BossStage, animation* PlayerAnimations, v2 WinDim)
{
	boss_initiallize_info Info = {};
	ClearEntityPool(Pool);
	switch(BossStage)
	{
		case BOSS_TUTORIAL:
		{
			Info.WallIndex = (int*)malloc(sizeof(int) * 4);
			rigid_body PlayerBody = 
			InitializeBody(V2(-550, -300), V2(), V2(15, 30), V2(50, 37), WinDim, 0, 10, BODY_RECT, V2(0, -3));
			Pool->PlayerIndex = 
			AddEntity(Pool, PlayerBody, PlayerAnimations, MAX_PLAYER_ANIMATION, ENTITY_TYPE_PLAYER, 5, {},
						0, CreateTimer(3.0f, false, TIMER_COUNTDOWN));
						
			rigid_body PlatformA = InitializeBody(V2(-400, -350), V2(), V2(400, 50), V2(400, 50), WinDim,
													0, 10, BODY_RECT);
			Info.WallIndex[0] = AddEntity(Pool, PlatformA, 0, 0, ENTITY_TYPE_WALL);
			rigid_body PlatformB = InitializeBody(V2(300, -350), V2(), V2(600, 50), V2(600, 50), WinDim,
													0, 10, BODY_RECT);
			Info.WallIndex[1] = AddEntity(Pool, PlatformB, 0, 0, ENTITY_TYPE_WALL);
			
			rigid_body FloatingPlatform[4] = {};
			for(int i = 0; i < 4; ++i)
			{
				FloatingPlatform[i] = 
				InitializeBody(V2(500 + i * -100.0f, -200 + i * 100.0f), V2(), V2(100, 25), V2(100, 25), WinDim, 0, 10, BODY_RECT);
				AddEntity(Pool, FloatingPlatform[i], 0, 0, ENTITY_TYPE_WALL);
			}
			
			rigid_body Boss =
			InitializeBody(V2(-500, 150), V2(), V2(180, 150), V2(240, 181), WinDim, 0, 50);
			Info.BossIndex = AddEntity(Pool, Boss, 0, 0, ENTITY_TYPE_BOSS_1, 1, GameAsset[PANTSU_ASSET]);
		} break;
		case BOSS_HENTAI:
		{
			Info.WallIndex = (int*)malloc(sizeof(int) * 4);
			rigid_body PlayerBody = 
			InitializeBody(V2(-300, 0), V2(), V2(15, 30), V2(50, 37), WinDim, 0, 10, BODY_RECT, V2(0, -3));
			Pool->PlayerIndex = 
			AddEntity(Pool, PlayerBody, PlayerAnimations, MAX_PLAYER_ANIMATION, ENTITY_TYPE_PLAYER, 5, {},
						0, CreateTimer(3.0f, false, TIMER_COUNTDOWN));
			
			rigid_body Wall = 
			InitializeBody(V2(0, -300), V2(), V2(1280, 100), V2(1280, 100), WinDim, 0, 50);
			Info.GroundIndex = AddEntity(Pool, Wall, 0, 0, ENTITY_TYPE_WALL);
			
			rigid_body Boss =
			InitializeBody(V2(500, -120), V2(), V2(225, 225), V2(225, 225), WinDim, 0, 50);
			Boss.Flip = SDL_FLIP_HORIZONTAL;
			Info.BossIndex = AddEntity(Pool, Boss, 0, 0, ENTITY_TYPE_BOSS_1, 1000, GameAsset[BOSS_1_ASSET]);
			
			for(int i = 0; i < 15; ++i)
			{
				v2 P = V2(-600 + (i * 32.0f), 400);
				v2 Dim = V2(64, 100);
				rigid_body Spike = InitializeBody(P, V2(), Dim, Dim, WinDim, 180, 10, BODY_TRIANGLE);
				AddEntity(Pool, Spike, 0, 0, ENTITY_TYPE_SPIKE, 0, GameAsset[SPIKE_ASSET]);
			}
			for(int i = 0; i < 40; ++i)
			{
				v2 P = V2(-620 + (i * 32.0f), -400);
				v2 Dim = V2(64, 100);
				rigid_body Spike = InitializeBody(P, V2(), Dim, Dim, WinDim, 0, 10, BODY_TRIANGLE);
				AddEntity(Pool, Spike, 0, 0, ENTITY_TYPE_SPIKE, 0, GameAsset[SPIKE_ASSET]);
			}
			for(int i = 0; i < 22; ++i)
			{
				v2 P = V2(-640, 400 - (i * 37.0f));
				v2 Dim = V2(78, 100);
				rigid_body Spike = InitializeBody(P, V2(), Dim, Dim, WinDim, -90, 10, BODY_TRIANGLE);
				AddEntity(Pool, Spike, 0, 0, ENTITY_TYPE_SPIKE, 0, GameAsset[SPIKE_ASSET]);
			}
			
			Info.BossMusic = SoundTracks[MUSIC_1];
		} break;
	}
	
	for(int i = 0; i < MAX_SPAWN_TIMER; ++i)
	{
		SetEndtimeToDefault(&SpawnTimer[i]);
	}
	
	Info.BossState = BOSS_STATE_1;
	Info.LastState = STATE_BOSS_MAIN;
	Info.Boss = BossStage;
	
	return Info;
}

#endif