#include "game.h"
#include "platform.h"
#include "platform.cpp"
#include "math.h"
#include "render.h"
#include "physics.h"
#include "timer.h"
#include "collision.h"
#include "animation.h"
#include "entity.h"
#include "boss.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{	
	InitLib(LIB_SDL_FULL);
	srand(0);
	render Graphics = {};
	InitGraphics(&Graphics, "( ͡° ͜ʖ ͡°)", {1280, 780}, 0);
	v2 WinDim = V2(Graphics.WinDim);
	
	//printf("PerF: %lld s/c: %.f\n", PerformanceFrequency, SecPerCount);
	int RefreshRate = Win32GetWindowRefreshRate(Graphics.Window);
	real32 GameUpdateHz = (real32)RefreshRate;
	real32 MsPerFrame = 1000.0f / GameUpdateHz;
	real32 TargetSecPerFrame = 1.0f / GameUpdateHz;
	UINT DesiredSchedulerMS = 1;
	bool SleepIsGranular = timeBeginPeriod(DesiredSchedulerMS) == TIMERR_NOERROR;
	
	uint64 LastCount = SDL_GetPerformanceCounter();
	
	real32 t = 0;
	real32 SecFrameMissed = 0;
	bool FrameMissed = false;
	
	Mix_Volume(-1, 25);
	Mix_VolumeMusic(30);
	Mix_Music* GST[MAX_MUSIC] = {};
	GST[MUSIC_1] = Mix_LoadMUS("ast/sound/music/already_dead.mp3");
	
	Mix_Chunk* SFX[MAX_SFX] = {};
	SFX[HENTAI_MOAN_SFX] = Mix_LoadWAV("ast/sound/sfx/moan.wav");
	SFX[HENTAI_MOAN_2_SFX] = Mix_LoadWAV("ast/sound/sfx/moan2.wav");
	SFX[DAME_SFX] = Mix_LoadWAV("ast/sound/sfx/dame.wav");
	SFX[CUMMING_SFX] = Mix_LoadWAV("ast/sound/sfx/cumming.wav");
	SFX[OOF_SFX] = Mix_LoadWAV("ast/sound/sfx/oof.wav");
	
	game_input GameInput = {};
	GameInput.KeyState = SDL_GetKeyboardState(0);
	GameInput.TextInput.Timer = CreateTimer(0.15f);
	//StartTimer(&GameInput.TextInput.Timer);
	
	entity_pool EntityPool = InitializeEntityPool(1000, 5);
	
	image_data GameAsset[MAX_ASSET] = {};
	
	LoadTexture(&GameAsset[PANTSU_ASSET], "ast/boss/pantsu.png", Graphics.Renderer);
	LoadTexture(&GameAsset[BOSS_1_ASSET], "ast/boss/boss_1.jpg", Graphics.Renderer);
	LoadTexture(&GameAsset[DILDO_ATTACK_ASSET], "ast/boss/dildo.png", Graphics.Renderer);
	LoadTexture(&GameAsset[WATER_DROP_ATTACK_ASSET], "ast/boss/water_drop.png", Graphics.Renderer);
	LoadTexture(&GameAsset[CUM_DROP_ATTACK_ASSET], "ast/boss/cum_drop.png", Graphics.Renderer);
	LoadTexture(&GameAsset[SPIKE_ASSET], "ast/boss/spike.png", Graphics.Renderer);
	LoadTexture(&GameAsset[HEART_EMPTY_ASSET], "ast/hud_heartEmpty.png", Graphics.Renderer);
	LoadTexture(&GameAsset[HEART_ASSET], "ast/hud_heartFull.png", Graphics.Renderer);
	
	LoadTexture(&GameAsset[PLAYER_SPRITE_ASSET], "ast/Player/playerSprite.png", Graphics.Renderer);
	SDL_Rect PlayerClips[109];
	for(int y = 0; y < 15; ++y)
	{
		for(int x = 0; x < 7; ++x)
		{
			PlayerClips[y * 7 + x] = {x * 50, y * 37, 50, 37};
		}
	}
	
	LoadTexture(&GameAsset[FIREBALL_SMALL_ASSET], "ast/fireball/Small_Fireball_10x26.png", Graphics.Renderer);
	SDL_Rect SmallFireballClips[60];
	for(int y = 0; y < 6; ++y)
	{
		for(int x = 0; x < 10; ++x)
		{
			SmallFireballClips[y * 10 + x] = {x * 10, y * 26, 10, 26};
		}
	}
	
	animation FireballAnimation[MAX_FIREBALL_ANIMATION] = {};
	FireballAnimation[FIREBALL_SMALL_ANIMATION] = 
	CreateAnimation(&GameAsset[FIREBALL_SMALL_ASSET], SmallFireballClips, 60, 0.0033f);
	
	animation PlayerAnimations[MAX_PLAYER_ANIMATION] = {};
	PlayerAnimations[PLAYER_IDLE_ANIMATION] = 
	CreateAnimation(&GameAsset[PLAYER_SPRITE_ASSET], GetClipSlices(PlayerClips, 0, 3), 4, 0.16f);
	PlayerAnimations[PLAYER_RUN_ANIMATION] = 
	CreateAnimation(&GameAsset[PLAYER_SPRITE_ASSET], GetClipSlices(PlayerClips, 8, 13), 6, 0.16f);
	PlayerAnimations[PLAYER_JUMP_ANIMATION] = 
	CreateAnimation(&GameAsset[PLAYER_SPRITE_ASSET], GetClipSlices(PlayerClips, 16, 23), 8, 0.1f, false);
	PlayerAnimations[PLAYER_SLIDE_ANIMATION] = 
	CreateAnimation(&GameAsset[PLAYER_SPRITE_ASSET], GetClipSlices(PlayerClips, 24, 28), 5, 0.16f, false);
	PlayerAnimations[PLAYER_ATTACK_ANIMATION] = 
	CreateAnimation(&GameAsset[PLAYER_SPRITE_ASSET], GetClipSlices(PlayerClips, 49, 52), 4, 0.03f, false);

	timer EntitySpawnTimer[MAX_SPAWN_TIMER] = {};
	EntitySpawnTimer[PLATFORM_SPAWN_TIMER] = CreateTimer(2.0f);
	EntitySpawnTimer[PLAYER_ATTACK_SPAWN_TIMER] = CreateTimer(0.1f);
	EntitySpawnTimer[DILDO_ATTACK_SPAWN_TIMER] = CreateTimer(0.3f);
	EntitySpawnTimer[DILDO_ATTACK_2_SPAWN_TIMER] = CreateTimer(0.4f);
	EntitySpawnTimer[DILDO_ATTACK_3_SPAWN_TIMER] = CreateTimer(2.0f);
	EntitySpawnTimer[WATER_DROP_ATTACK_SPAWN_TIMER] = CreateTimer(2.5f);
	EntitySpawnTimer[CUM_DROP_ATTACK_SPAWN_TIMER] = CreateTimer(0.05f);
	EntitySpawnTimer[CUM_DROP_ATTACK_2_SPAWN_TIMER] = CreateTimer(0.05f);
	EntitySpawnTimer[CUM_DROP_ATTACK_3_SPAWN_TIMER] = CreateTimer(1.5f);
	
	SDL_Rect PlayerHeartRect[5] = {};
	for(int i = 0; i < 5; ++i)
	{
		PlayerHeartRect[i] = {600 - (i * GameAsset[HEART_ASSET].Dim.x), 350, 
								GameAsset[HEART_ASSET].Dim.x, GameAsset[HEART_ASSET].Dim.y};
	}
	
	boss_initiallize_info BossStageInfo = 
	InitializeBossStage(&EntityPool, EntitySpawnTimer, GameAsset, GST,
						BOSS_TUTORIAL, PlayerAnimations, WinDim);
	
	rigid_body BigDildo = 
	InitializeBody(V2(0, 500), V2(0, -10), V2(45, 170), V2(180, 180), WinDim, 180, 50);
	int BigDildoIndex = 0; 
	
	TTF_Font* MidiumFont = TTF_OpenFont("ast/font.ttf", 32);
	image_data CongratTextA = {};
	image_data CongratTextB = {};
	LoadText(Graphics.Renderer, MidiumFont, &CongratTextA, 
			"Congratuation you've beaten the game!", {255, 255, 255, 255});
	LoadText(Graphics.Renderer, MidiumFont, &CongratTextB, 
				"Press space to restart or ESC to quit the game", {255, 255, 255, 255});
	TTF_CloseFont(MidiumFont);
	
	// Tutorial
	TTF_Font* TutorialFont = TTF_OpenFont("ast/font.ttf", 12);
	image_data TutorialText[TUTORIAL_TEXT_MAX] = {};
	LoadText(Graphics.Renderer, TutorialFont, &TutorialText[TUTORIAL_TEXT_A], 
			"Press left right arrow keys to move.", {255, 255, 255, 255});
	LoadText(Graphics.Renderer, TutorialFont, &TutorialText[TUTORIAL_TEXT_B], 
			"Press Up arrow key to jump", {255, 255, 255, 255});
	LoadText(Graphics.Renderer, TutorialFont, &TutorialText[TUTORIAL_TEXT_C], 
			"Press twice to double jump", {255, 255, 255, 255});
	LoadText(Graphics.Renderer, TutorialFont, &TutorialText[TUTORIAL_TEXT_D], 
			"Press \'down\' arrow key to slide while running", {255, 255, 255, 255});
	LoadText(Graphics.Renderer, TutorialFont, &TutorialText[TUTORIAL_TEXT_E], 
			"When you're ready press z to attack the pantsu to start", {255, 255, 255, 255});
	TTF_CloseFont(TutorialFont);
	
	timer HitEffectTimer = CreateTimer(0.3f);
	timer GlobalTimer = CreateTimer(0);	
	bool Running = true;
	
	game_state AppState = BossStageInfo.LastState;
	
	//SDL_StartTextInput();
	while(Running)
	{
#if 1
		ProcessTimer(&GameInput.TextInput.Timer, t);
		ProcessTimer(&GlobalTimer, t);
		
		game_controller_input* KeyInput = &GameInput.Keyboard;
		for(int ButtonIndex = 0; ButtonIndex < MAX_KEY; ++ButtonIndex)
		{
			KeyInput->Keys[ButtonIndex].EndedDown = 0;
		}
		
		SDL_Event Event;
		while(SDL_PollEvent(&Event) != 0)
		{
			if(HandleEvents(&Event, &GameInput))
			{
				Running = false;
			}
		}
#endif	
		SDL_SetRenderDrawColor(Graphics.Renderer, 0, 0, 0, 255);
		SDL_RenderClear(Graphics.Renderer);
		
		switch(AppState)
		{
			case STATE_GAMEOVER:
			{
				BossStageInfo = 
				InitializeBossStage(&EntityPool, EntitySpawnTimer, GameAsset, GST, BossStageInfo.Boss, 
									PlayerAnimations, WinDim);
				AppState = STATE_BOSS_MAIN;
				Mix_RewindMusic();
			} break;
			case STATE_BOSS_MAIN:
			{
				if(!Mix_PlayingMusic())
				{
					Mix_PlayMusic(BossStageInfo.BossMusic, -1);
				}
				ProcessEntitySpawnTimers(EntitySpawnTimer, t);
				ProcessEntityAnimationTimers(&EntityPool, t);
				ProcessTimer(&HitEffectTimer, t);
				entity* Player = &EntityPool.Entities[EntityPool.PlayerIndex];
				entity* Boss = &EntityPool.Entities[BossStageInfo.BossIndex];
				entity* Ground = &EntityPool.Entities[BossStageInfo.GroundIndex];
				ProcessTimer(&Player->Timer, t);
				
				if(HitEffectTimer.Complete)
				{
					ResetEntityTextureMod(&EntityPool);
				}
				
				Player->Body.Colliding = false;
				
				switch(BossStageInfo.Boss)
				{
					case BOSS_TUTORIAL:
					{
						if(Boss->HP <= 0)
						{
							BossStageInfo = 
							InitializeBossStage(&EntityPool, EntitySpawnTimer, GameAsset, GST, 
												BOSS_HENTAI, PlayerAnimations, WinDim);
							Mix_RewindMusic();
						}
					} break;
					case BOSS_HENTAI:
					{
						switch(BossStageInfo.BossState)
						{
							case BOSS_STATE_1:
							{
								if(EntitySpawnTimer[DILDO_ATTACK_SPAWN_TIMER].Complete)
								{
									DildoAttack(&EntityPool, WinDim, GameAsset[DILDO_ATTACK_ASSET], V2(-500, 500));
								}
								
								if(Boss->HP < 900)
								{
									BossStageInfo.BossState = BOSS_STATE_2;
								}
							} break;
							case BOSS_STATE_2:
							{
								if(EntitySpawnTimer[DILDO_ATTACK_SPAWN_TIMER].Complete)
								{
									DildoAttack(&EntityPool, WinDim, GameAsset[DILDO_ATTACK_ASSET], V2(-500, 500));
								}
								if(EntitySpawnTimer[WATER_DROP_ATTACK_SPAWN_TIMER].Complete)
								{
									Mix_PlayChannel(-1, SFX[HENTAI_MOAN_2_SFX], 0);
									WaterDropAttack(&EntityPool, WinDim, GameAsset[WATER_DROP_ATTACK_ASSET], 
													Boss->Body.Position);
								}
								
								if(Boss->HP < 700)
								{
									BossStageInfo.BossState = BOSS_STATE_3;
									TranslateEntity(Boss, V2(Boss->Body.Position.x, 0));
									TranslateEntity(Ground, V2(0, -500), 15);
									EntitySpawnTimer[DILDO_ATTACK_SPAWN_TIMER].EndTime = 3.0f;
									EntitySpawnTimer[PLATFORM_SPAWN_TIMER].EndTime = 3.0f;
									//Mix_PlayChannel(-1, SFX[DAME_SFX], 0);
								}
							} break;
							case BOSS_STATE_3:
							{
								if(EntitySpawnTimer[PLATFORM_SPAWN_TIMER].Complete)
								{
									SpawnFloatingPlatform(&EntityPool, WinDim, V2(-500, -100));
								}
								if(EntitySpawnTimer[DILDO_ATTACK_SPAWN_TIMER].Complete)
								{
									DildoAttack(&EntityPool, WinDim, GameAsset[DILDO_ATTACK_ASSET], V2(-500, -200));
								}
								if(EntitySpawnTimer[DILDO_ATTACK_2_SPAWN_TIMER].Complete)
								{
									DildoAttack2(&EntityPool, WinDim, GameAsset[DILDO_ATTACK_ASSET], Boss->Body.Position);
								}
								
								if(!Boss->Translating)
								Boss->Body.Velocity.y = 
								Sin(GlobalTimer.Time * 10) * 500;
								
								if(Boss->HP < 500)
								{
									BossStageInfo.BossState = BOSS_STATE_4;
									
									BigDildoIndex = AddEntity(&EntityPool, BigDildo, 0, 0, ENTITY_TYPE_DILDO, 0, 
												GameAsset[DILDO_ATTACK_ASSET]);
									EntityPool.Entities[BigDildoIndex].Invincible = true;
									TranslateEntity(Ground, V2(0, -300), 30);
									TranslateEntity(Boss, V2(Boss->Body.Position.x, -150), 50);
									TranslateEntity(&EntityPool.Entities[BigDildoIndex], V2(0, 400), 15);
									//Mix_PlayChannel(-1, SFX[CUMMING_SFX], 0);
								}
							} break;
							case BOSS_STATE_4:
							{
								entity* CumAttackDildo = &EntityPool.Entities[BigDildoIndex];
								if(EntitySpawnTimer[DILDO_ATTACK_3_SPAWN_TIMER].Complete)
								{
									DildoAttack3(&EntityPool, GameAsset[DILDO_ATTACK_ASSET], WinDim);
								}
								if(EntitySpawnTimer[CUM_DROP_ATTACK_SPAWN_TIMER].Complete && 
									CumAttackDildo->Body.Velocity.y == 0)
								{
									CumDropAttack(&EntityPool, WinDim, GameAsset[CUM_DROP_ATTACK_ASSET], 
													CumAttackDildo->Body.Position - V2(0, CumAttackDildo->Body.Shape.Dim.y * 0.5f), GlobalTimer.Time);
									CumAttackDildo->Body.Angle = 180 + (Sin(GlobalTimer.Time * 10) * 70);
								}
								
								if(Boss->HP < 300)
								{
									BossStageInfo.BossState = BOSS_STATE_5;
									
									EntitySpawnTimer[CUM_DROP_ATTACK_SPAWN_TIMER].EndTime = 1.5f;
									
									TranslateEntity(Ground, V2(0, -500), 15);
									TranslateEntity(Boss, V2(0, 50), 500);
									real32 Theta = 45;
									real32 Radius = 700;
									
									for(int i = 0; i < 4; ++i)
									{
										real32 X = Cos(Radians(Theta));
										real32 Y = Sin(Radians(Theta));
										v2 Dim = V2(100, 25);
										rigid_body Platform = 
										InitializeBody(V2(X, Y) * Radius, V2(), Dim, Dim, WinDim, 0, 10);
										int Index = 
										AddEntity(&EntityPool, Platform, 0, 0, ENTITY_TYPE_WALL, 0, {}, Theta);
										entity* Entity = &EntityPool.Entities[Index];
										TranslateEntity(Entity, V2(X * (Radius * 0.4f), Y * (Radius * 0.3f)), 200.0f);
										PrintBodyStatus(&Platform);
										Theta += 90;
									}
									Theta = 0;
									for(int i = 0; i < 3; ++i)
									{
										real32 X = Cos(Radians(Theta));
										real32 Y = Sin(Radians(Theta));
										v2 Dim = V2(100, 25);
										rigid_body Platform = 
										InitializeBody(V2(X, Y) * Radius, V2(), Dim, Dim, WinDim, 0, 10);
										int Index = 
										AddEntity(&EntityPool, Platform, 0, 0, ENTITY_TYPE_WALL, 0, {}, Theta);
										entity* Entity = &EntityPool.Entities[Index];
										TranslateEntity(Entity, V2(X * (Radius * 0.5f), Y * (Radius * 0.3f)), 200.0f);
										PrintBodyStatus(&Platform);
										Theta += 90;
									}
									
									for(int i = 0; i < 3; ++i)
									{
										v2 Dim = {100, 500};
										rigid_body Platform =
										InitializeBody(V2(-400 + (i * 400.0f), -700), V2(), Dim, Dim, WinDim, 0, 10);
										BossStageInfo.WallIndex[i] = 
										AddEntity(&EntityPool, Platform, 0, 0, ENTITY_TYPE_WALL);
										entity* Entity = &EntityPool.Entities[BossStageInfo.WallIndex[i]];
										TranslateEntity(Entity, V2(Entity->Body.Position.x, -500), 30);
									}
								}
							} break;
							case BOSS_STATE_5:
							{
								entity* CumAttackDildo = &EntityPool.Entities[BigDildoIndex];
								if(EntitySpawnTimer[CUM_DROP_ATTACK_2_SPAWN_TIMER].Complete && !Boss->Translating)
								{
									CumDropAttack2(&EntityPool, WinDim, GameAsset[CUM_DROP_ATTACK_ASSET], 
													Boss->Body.Position, GlobalTimer.Time);
								}
								
								if(EntitySpawnTimer[CUM_DROP_ATTACK_3_SPAWN_TIMER].Complete && !Boss->Translating
									&& Boss->HP < 200)
								{
									CumDropAttack4(&EntityPool, WinDim, GameAsset[CUM_DROP_ATTACK_ASSET], 
													Boss->Body.Position);
								}
								
								if(EntitySpawnTimer[CUM_DROP_ATTACK_SPAWN_TIMER].Complete && 
									CumAttackDildo->Body.Velocity.y == 0 && Player->Body.Position.y < 0)
								{
									CumAttackDildo->Body.Angle = 90 + 
									CumDropAttack3(&EntityPool, WinDim, GameAsset[CUM_DROP_ATTACK_ASSET], 
													(CumAttackDildo->Body.Shape.Polygon.MinCenter));
									RotatePolygon(&CumAttackDildo->Body.Shape.Polygon, CumAttackDildo->Body.Angle -CumAttackDildo->Theta, WinDim);
									CumAttackDildo->Theta = CumAttackDildo->Body.Angle;
								}
								
								if(Boss->HP <= 0)
								{
									AppState = STATE_MAIN;
									Mix_PlayChannel(-1, SFX[DAME_SFX], 0);
								}
							} break;
						}
					} break;
				}
				if(EntitySpawnTimer[PLAYER_ATTACK_SPAWN_TIMER].Complete && 
					GameInput.KeyState[SDL_SCANCODE_Z] && !Player->Sliding)
				{
					//printf("Attacked\n");
					Player->Attack = true;
					Player->Body.Velocity.x = 0;
					PlayerAnimations[PLAYER_ATTACK_ANIMATION].CurrentFrame = 0;
					StartAnimation(&PlayerAnimations[PLAYER_ATTACK_ANIMATION]);
					rigid_body Fireball = {};
					v2 P, V, Dim;
					real32 Theta;
					if(Player->Body.Flip == SDL_FLIP_HORIZONTAL)
					{
						P = Player->Body.Position + V2(-10, 0);
						V = V2(-500, 0);
						Theta = 270;
					}
					else
					{
						P = Player->Body.Position + V2(10, 0);
						V = V2(500, 0);
						Theta = 90;
					}
					Dim = V2(10, 26);
					Fireball = 
					InitializeBody(P, V, Dim, Dim, WinDim, Theta, 10);
					animation Animation = FireballAnimation[FIREBALL_SMALL_ANIMATION];
					Animation.CurrentFrame = 0;
					AddEntity(&EntityPool, Fireball, &Animation, 1, ENTITY_TYPE_FIREBALL_1);
				}
				
				UpdateEntities(&EntityPool, &Graphics, &GameInput, t, GameAsset, EntitySpawnTimer);
				Player->OnGround = false;
				Player->OnSlope = false;
				ResolveEntitiesCollision(&EntityPool, WinDim, t, &AppState, SFX);
				if(Player->HP <= 0)
				{
					AppState = STATE_GAMEOVER;
					BossStageInfo.LastState = STATE_BOSS_MAIN;
				}
								
				if(Player->OnGround)
				{
					PlayerAnimations[PLAYER_JUMP_ANIMATION].CurrentFrame = 0;
				}
				if(Player->Body.Angle)
				{
					StartAnimation(&PlayerAnimations[PLAYER_SLIDE_ANIMATION]);
				}
				
				if(!Player->Jump && !Player->OnGround && KeyInput->Keys[KEY_UP].EndedDown)
				{
					PlayerAnimations[PLAYER_JUMP_ANIMATION].CurrentFrame = 0;
					Player->Jump = true;
					Player->Body.Velocity.y = 400.0f;
					StartAnimation(&PlayerAnimations[PLAYER_JUMP_ANIMATION]);
				}
				
				if(KeyInput->Keys[KEY_UP].IsDown && Player->OnGround)
				{
					Player->Body.Velocity.y = 500.0f;
					StartAnimation(&PlayerAnimations[PLAYER_JUMP_ANIMATION]);
				}

				if(IsEntityOutOfScreen(Player, WinDim))
				{
					AppState = STATE_GAMEOVER;
					BossStageInfo.LastState = STATE_BOSS_MAIN;
				}
				
				//PrintBodyStatus(&Player->Body);
			} break;
			case STATE_MAIN:
			{
				if(KeyInput->Keys[KEY_SPACE].EndedDown)
				{
					AppState = STATE_GAMEOVER;
					BossStageInfo.LastState = STATE_BOSS_MAIN;
				}
				
				if(GameInput.KeyState[SDL_SCANCODE_ESCAPE])
				{
					Running = false;
				}
#if 0	
				if(GameInput.KeyDown)
				{
					DecreaseEndtime(&GameInput.TextInput.Timer, -5, 0.03f);
				}
				else
				{
					SetEndtimeToDefault(&GameInput.TextInput.Timer);
				}
				ProcessTextInput(&GameInput, &Event);
#endif
			} break;
			case STATE_TEST:
			{
				/*  Old test code for rotating polygons
					v2 Delta = {};
					real32 Theta = 0;
					if(GameInput.KeyState[SDL_SCANCODE_LEFT])
					{
						Theta += 200 * t;
					}
					if(GameInput.KeyState[SDL_SCANCODE_RIGHT])
					{
						Theta -= 200 * t;
					}
					if(GameInput.KeyState[SDL_SCANCODE_UP])
					{
						Delta += V2(Cos(Radians(Triangle1.Theta)), Sin(Radians(Triangle1.Theta))) * (500 * t);
					}
					
					if(KeyInput->Keys[KEY_R].EndedDown)
					{
						Wall = CreatePolygon(POLYGON_RECTANGLE, V2(800, -500), V2(100, 100), 
												WinDim);
					}
					
					RotatePolygon(&Triangle1, Theta);			
					UpdateVertices(&Triangle1, Delta, WinDim);
					Triangle1.Theta += Theta;
				*/
			} break;
		}
		real32 MissedTime = 0;
		FrameMissed = LockFPS(LastCount, TargetSecPerFrame);
#if 0
		real32 FrameCompleteTime = Win32GetSecondElapsed(LastCount, SDL_GetPerformanceCounter());
		if(FrameCompleteTime < TargetSecPerFrame)
		{
			FrameMissed = false;
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
			FrameMissed = true;
			//printf("Missed a frame!\n");
		}
#endif
		uint64 EndCount = SDL_GetPerformanceCounter();
		
		switch(AppState)
		{
			case STATE_BOSS_MAIN:
			{
				if(BossStageInfo.Boss == BOSS_TUTORIAL)
				{
					RenderCopy(&Graphics, TutorialText[TUTORIAL_TEXT_A].Texture, 0, 
					&SetRectRounded(EntityPool.Entities[BossStageInfo.WallIndex[TUTORIAL_TEXT_A]].Body.Position, V2(TutorialText[TUTORIAL_TEXT_A].Dim)));
					RenderCopy(&Graphics, TutorialText[TUTORIAL_TEXT_B].Texture, 0, 
					&SetRectRounded(V2(-100, -250), V2(TutorialText[TUTORIAL_TEXT_B].Dim)));
					RenderCopy(&Graphics, TutorialText[TUTORIAL_TEXT_C].Texture, 0, 
					&SetRectRounded(V2(-100, -280), V2(TutorialText[TUTORIAL_TEXT_C].Dim)));
					RenderCopy(&Graphics, TutorialText[TUTORIAL_TEXT_D].Texture, 0, 
					&SetRectRounded(EntityPool.Entities[BossStageInfo.WallIndex[TUTORIAL_TEXT_B]].Body.Position, V2(TutorialText[TUTORIAL_TEXT_D].Dim)));
					RenderCopy(&Graphics, TutorialText[TUTORIAL_TEXT_E].Texture, 0, 
					&SetRectRounded(V2(0, 300), V2(TutorialText[TUTORIAL_TEXT_E].Dim)));
				}
				entity* Player = &EntityPool.Entities[EntityPool.PlayerIndex];
				RenderEntities(&EntityPool, &Graphics);
				for(int i = 0; i < 5; ++i)
				{
					RenderCopy(&Graphics, GameAsset[HEART_EMPTY_ASSET].Texture, 0, &PlayerHeartRect[i]);
				}
				for(int i = 0; i < (int)Player->HP; ++i)
				{
					RenderCopy(&Graphics, GameAsset[HEART_ASSET].Texture, 0, &PlayerHeartRect[i]);
				}
#if 0
				if(Player->Body.Colliding)
					DrawBodyLines(&Player->Body, Graphics.Renderer, {255, 0, 0, 255});
				else 
					DrawBodyLines(&Player->Body, Graphics.Renderer, {255, 255, 255, 255});
#endif
		} break;
			case STATE_MAIN:
			{
				RenderCopy(&Graphics, CongratTextA.Texture, 0, &SetRectRounded(V2(), V2(CongratTextA.Dim)));
				RenderCopy(&Graphics, CongratTextB.Texture, 0, &SetRectRounded(V2(0, -200), V2(CongratTextB.Dim)));
			} break;
			case STATE_TEST:
			{
				
			} break;
		}
		
		SDL_RenderPresent(Graphics.Renderer);
		
		real32 ElapsedTime = Win32GetSecondElapsed(LastCount, EndCount);
		if(FrameMissed)
			t = 1.0f / GameUpdateHz;
		else
			t = ElapsedTime;
		uint64 CounterElapsed = EndCount - LastCount;
		real32 FPS = (real32)PerformanceFrequency / (real32)CounterElapsed;
		
		PrintFPS(FPS);
		
		LastCount = EndCount;
	}
	
	//SDL_StopTextInput();
	return 0;
}
