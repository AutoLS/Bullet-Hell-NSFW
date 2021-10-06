#ifndef ENTITY_H
#define ENTITY_H

#include "physics.h"
#include "animation.h"

enum entity_type
{
	ENTITY_TYPE_PLAYER,
	ENTITY_TYPE_WALL,
	ENTITY_TYPE_WALL_DYNAMIC,
	ENTITY_TYPE_SPIKE,
	ENTITY_TYPE_FIREBALL_1,
	ENTITY_TYPE_BOSS_1,
	ENTITY_TYPE_DILDO,
	ENTITY_TYPE_WATER_DROP,
	ENTITY_TYPE_CUM_DROP,
};

enum entity_spawn_timer
{
	PLATFORM_SPAWN_TIMER,
	PLAYER_ATTACK_SPAWN_TIMER,
	DILDO_ATTACK_SPAWN_TIMER,
	DILDO_ATTACK_2_SPAWN_TIMER,
	DILDO_ATTACK_3_SPAWN_TIMER,
	WATER_DROP_ATTACK_SPAWN_TIMER,
	CUM_DROP_ATTACK_SPAWN_TIMER,
	CUM_DROP_ATTACK_2_SPAWN_TIMER,
	CUM_DROP_ATTACK_3_SPAWN_TIMER,
	MAX_SPAWN_TIMER,
};

struct entity
{
	rigid_body Body;
	entity_type Type;
	
	real32 HP;
	real32 Theta;
	
	bool Sliding;
	bool Attack;
	bool OnGround;
	bool OnSlope;
	bool Jump;
	bool Translating;
	bool Invincible;
	
	bool Static;
	
	v2 TranslatePos;
	
	bool Alive;
	
	animation* Animation;
	int AnimationCount;
	
	image_data Texture;
	timer Timer;
};

struct entity_pool
{
	entity* Entities;
	stack DeadStack;
	int UsedEntities;
	int PlayerIndex;
	int EntitySize;
};

entity_pool InitializeEntityPool(int EntitySize, uint32 StackCapacity)
{
	entity_pool EntityPool = {};
	EntityPool.EntitySize = EntitySize;
	EntityPool.Entities = (entity*)malloc(sizeof(entity) * EntitySize);
	EntityPool.DeadStack.Size = 0;
	EntityPool.DeadStack.Capacity = StackCapacity;
	EntityPool.DeadStack.MyStack = (int*)malloc(sizeof(int) * EntityPool.DeadStack.Capacity);
	Push(&EntityPool.DeadStack, -1);
	
	return EntityPool;
}

void ClearEntityPool(entity_pool* Pool)
{
	Pool->UsedEntities = 0;
	Pool->Entities = (entity*)malloc(sizeof(entity) * Pool->EntitySize);
	Pool->DeadStack.Size = 0;
	Pool->DeadStack.Capacity = 5;
	free(Pool->DeadStack.MyStack);
	Pool->DeadStack.MyStack = 
	(int*)malloc(sizeof(int) * Pool->DeadStack.Capacity);
	Push(&Pool->DeadStack, -1);
}

int AddEntity(entity_pool* Pool, rigid_body Body, animation* Animation, int AnimationCount, 
				entity_type Type, real32 HP = 0, image_data Texture = {}, real32 Theta = 0, timer Timer = {})
{
	entity Entity = {};
	Entity.Body = Body;
	Entity.Type = Type;
	Entity.Alive = true;
	Entity.Animation = Animation;
	Entity.AnimationCount = AnimationCount;
	Entity.Texture = Texture;
	Entity.HP = HP;
	if(Timer.Time == 0)
		Entity.Timer = CreateTimer(0);
	else
		Entity.Timer = Timer;
	Entity.Theta = Theta;
	
	switch(Type)
	{
		case ENTITY_TYPE_WALL:
		case ENTITY_TYPE_WALL_DYNAMIC:
		case ENTITY_TYPE_SPIKE:
		Entity.Static = true;
	}
	
	int DeadIndex = Peek(&Pool->DeadStack);
	if(DeadIndex == -1)
	{
		Pool->Entities[Pool->UsedEntities++] = Entity;
		return Pool->UsedEntities - 1;
	}
	else
	{
		Pool->Entities[DeadIndex] = Entity;
		Pop(&Pool->DeadStack);
		return DeadIndex;
	}
}

void KillEntity(entity_pool* Pool, int Index)
{
	entity* Entity = &Pool->Entities[Index];
	if(!Entity->Invincible)
	{
		Entity->Alive = false;
		Push(&Pool->DeadStack, Index);	
	}
	//printf("Killed. Entity index: %d\n", Pool->UsedEntities);
}

bool IsEntityOutOfScreen(entity* Entity, v2 WinDim)
{
	return Entity->Body.Position.x > WinDim.x || 
			Entity->Body.Position.x < -WinDim.x ||
			Entity->Body.Position.y > WinDim.y ||
			Entity->Body.Position.y < -WinDim.y;
}

void ProcessEntityAnimationTimers(entity_pool* Pool, real32 t)
{
	for(int i = 0; i < Pool->UsedEntities; ++i)
	{
		entity* Entity = &Pool->Entities[i];
		if(Entity->Animation && Entity->Alive)
		{
			for(int j = 0; j < Entity->AnimationCount; ++j)
			{
				ProcessTimer(&Entity->Animation[j].Timer, t);
			}
		}
	}
}

void ProcessEntitySpawnTimers(timer* Timer, real32 t)
{
	for(int i = 0; i < MAX_SPAWN_TIMER; ++i)
	{
		ProcessTimer(&Timer[i], t);
	}
}

void TranslateEntity(entity* Entity, v2 Pos, real32 Speed = 200.0f)
{
	v2 Distance = Pos - Entity->Body.Position;
	real32 R = Length(Distance);
	real32 Theta = Degrees((real32)acos(Distance.x/R));
	if(R)
	{
		Entity->Translating = true;
		Entity->TranslatePos = Pos;
		if(Distance.y > 0)
			Entity->Body.Velocity = V2(Cos(Radians(Theta)), Sin(Radians(Theta))) * Speed;
		else
			Entity->Body.Velocity = V2(Cos(Radians(Theta)), -Sin(Radians(Theta))) * Speed;
	}
}

void SpawnFloatingPlatform(entity_pool* Pool, v2 WinDim, v2 Range)
{
	rigid_body Platform = {};
	v2 P = V2(Rand32(Range.x, Range.y), -400);
	Platform = InitializeBody(P, V2(0, 30), V2(100, 25), V2(100, 25), WinDim, 0, 50);
	AddEntity(Pool, Platform, 0, 0, ENTITY_TYPE_WALL);
}

void DildoAttack(entity_pool* Pool, v2 WinDim, image_data Texture, v2 Range)
{
	v2 P = V2(Rand32(Range.x, Range.y), 400);
	rigid_body Dildo = 
	InitializeBody(P, V2(0, -300), V2(40, 145), V2(180, 180), WinDim, 180, 50, BODY_RECT, V2(0, -15));
	AddEntity(Pool, Dildo, 0, 0, ENTITY_TYPE_DILDO, 0, Texture);
}

void DildoAttack2(entity_pool* Pool, v2 WinDim, image_data Texture, v2 Pos)
{
	real32 Theta = Rand32(-30, 30);
	rigid_body Dildo = 
	InitializeBody(Pos, V2(-Cos(Radians(Theta)), -Sin(Radians(Theta))) * 200, V2(10, 50), V2(50, 50), WinDim, 90 + Theta, 50);
	AddEntity(Pool, Dildo, 0, 0, ENTITY_TYPE_DILDO, 0, Texture);
}

void DildoAttack3(entity_pool* Pool, image_data Texture, v2 WinDim)
{
	v2 Pos = V2(Rand32(-500, 500), Rand32(100, 400));
	v2 Distance = Pos - Pool->Entities[Pool->PlayerIndex].Body.Position;
	real32 R = Length(Distance);
	real32 X = Distance.x/R;
	real32 Theta = Degrees((real32)acos(X));
	rigid_body Dildo = 
	InitializeBody(Pos, V2(), V2(45, 180), V2(180, 180), WinDim, Theta + 90, 10);
	Dildo.Force = V2(-Cos(Radians(Theta)), -Sin(Radians(Theta))) * 3000;
	AddEntity(Pool, Dildo, 0, 0, ENTITY_TYPE_DILDO, 0, Texture);
}

void WaterDropAttack(entity_pool* Pool, v2 WinDim, image_data Texture, v2 Pos)
{
	int Count = rand() % 5 + 1;
	real32 Theta = 0;
	for(int i = 0; i < Count; ++i)
	{
		Theta = Rand32(-60, -30);
		rigid_body WaterDrop = 
		InitializeBody(Pos, V2(-Cos(Radians(Theta)), Sin(Radians(Theta)*100)) * 300, V2(30, 45), V2(100, 100), WinDim, Theta, 50);
		AddEntity(Pool, WaterDrop, 0, 0, ENTITY_TYPE_WATER_DROP, 0, Texture);
	}
}

void CumDropAttack(entity_pool* Pool, v2 WinDim, image_data Texture, v2 Pos, real32 t)
{
	real32 Theta = Sin(t*10) * 70 + 270;
	v2 Dim = V2(20.38f, 38.76f);
	v2 RenderDim = V2(24.38f, 44.76f);
	rigid_body Cum =
	InitializeBody(Pos, V2(Cos(Radians(Theta)), Sin(Radians(Theta))) * 400, Dim, RenderDim,
					WinDim, Theta - 270, 10);
	AddEntity(Pool, Cum, 0, 0, ENTITY_TYPE_CUM_DROP, 0, Texture);
}

void CumDropAttack2(entity_pool* Pool, v2 WinDim, image_data Texture, v2 Pos, real32 t)
{
	real32 Theta = t * 30;
	v2 Dim = V2(20.38f, 38.76f);
	v2 RenderDim = V2(24.38f, 44.76f);
	for(int i = 0; i < 4; ++i)
	{
		rigid_body Cum =
		InitializeBody(Pos, V2(Cos(Radians(Theta)), Sin(Radians(Theta))) * 400, Dim, RenderDim,
						WinDim, Theta + 90, 10);
		AddEntity(Pool, Cum, 0, 0, ENTITY_TYPE_CUM_DROP, 0, Texture);
		Theta += 90;
	}
}

real32 CumDropAttack3(entity_pool* Pool, v2 WinDim, image_data Texture, v2 Pos)
{
	v2 Distance = Pos - Pool->Entities[Pool->PlayerIndex].Body.Position;
	real32 R = Length(Distance);
	real32 X = Distance.x/R;
	real32 Theta = Degrees((real32)acos(X));
	v2 Dim = V2(15.38f, 38.76f);
	v2 RenderDim = V2(24.38f, 44.76f);
	rigid_body Dildo = 
	InitializeBody(Pos, V2(), Dim, RenderDim, WinDim, Theta + 270, 10);
	Dildo.Force = V2(-Cos(Radians(Theta)), -Sin(Radians(Theta))) * 3000;
	AddEntity(Pool, Dildo, 0, 0, ENTITY_TYPE_DILDO, 0, Texture);
	return Theta;
}

void CumDropAttack4(entity_pool* Pool, v2 WinDim, image_data Texture, v2 Pos)
{
	real32 Theta = 10;
	v2 Dim = V2(20.38f, 38.76f);
	v2 RenderDim = V2(24.38f, 44.76f);
	for(int i = 0; i < 12; ++i)
	{
		rigid_body Cum =
		InitializeBody(Pos, V2(Cos(Radians(Theta)), Sin(Radians(Theta))) * 200, Dim, RenderDim,
						WinDim, Theta + 90, 10);
		AddEntity(Pool, Cum, 0, 0, ENTITY_TYPE_DILDO, 0, Texture);
		Theta += 30;
	}
}

void UpdatePlayer(entity* Player, game_input* GameInput, v2 WinDim, real32 t)
{
	v2 Force = V2();
	real32 Theta = 0;
	real32 Drag = 5.0f;
	if(GameInput->KeyState[SDL_SCANCODE_LEFT] && !Player->Attack && !Player->Sliding)
	{
		Player->Body.Flip = SDL_FLIP_HORIZONTAL;
		Force += V2(-12500, 0);
	}
	if(GameInput->KeyState[SDL_SCANCODE_RIGHT] && !Player->Attack && !Player->Sliding)
	{
		Player->Body.Flip = SDL_FLIP_NONE;
		Force += V2(12500, 0);
	}
	if(GameInput->Keyboard.Keys[KEY_DOWN].EndedDown && Length(Player->Body.Velocity) > 200 && Player->OnGround)
	{
		Player->Sliding = true;
		if(Length(Player->Body.Velocity) < 250)
		Player->Body.Velocity.x *= 1.3f;
		if(Player->Body.Flip == SDL_FLIP_HORIZONTAL)
		{
			Theta -= 90;
		}
		else
		{
			Theta += 90;
		}
		if(Player->Body.Angle != 0)
		{
			Theta = 0;
		}
	}
	
	if(Player->Sliding)
	{
		Drag = 1.5f;
		//Force += V2(3000,0);
	}
	
	if(Player->Body.Angle != 0 && (!GameInput->KeyState[SDL_SCANCODE_DOWN] || 
		Length(Player->Body.Velocity) < 70))
	{
		Player->Sliding = false;
		Theta += -Player->Body.Angle;
		//Player->Body.Angle += Theta;
		printf("%.1f\n", Theta);
	}
	
	
	Force += V2(0, -9800);

	Player->Body.Force = Force;
	Player->Body.Angle += Theta;
	ComputeTorque(&Player->Body);
	IntegrateLinearMotion(&Player->Body, t, WinDim, Drag);
	RotatePolygon(&Player->Body.Shape.Polygon, Theta, WinDim);
	//IntegrateAngularMotion(&Player->Body, t);
}

void UpdateEntities(entity_pool* Pool, render* Graphics, game_input* GameInput, real32 t, 
					image_data* GameAssets, timer* EntitySpawnTimer)
{
	int PlayerIndex = Pool->PlayerIndex;
	v2 WinDim = V2(Graphics->WinDim);
	for(int i = 0; i < Pool->UsedEntities; ++i)
	{
		entity* Entity = &Pool->Entities[i];
		if(Entity->Alive)
		{
			switch(Entity->Type)
			{
				case ENTITY_TYPE_PLAYER:
				{
					UpdatePlayer(Entity, GameInput, WinDim, t);
					Entity->Body.RenderRect = SetRectRounded(Entity->Body.Position, Entity->Body.RenderDim);
				} break;
				case ENTITY_TYPE_FIREBALL_1:
				{
					real32 Drag = 0.0f;
					IntegrateLinearMotion(&Entity->Body, t, WinDim, Drag);
					v2 P = Entity->Body.Position;
					v2 Dim = Entity->Body.Shape.Dim;
					Entity->Body.Rect = Rect32(V2(P.x - Dim.x * 0.5f, P.y - Dim.y * 0.5f), Dim);
					Entity->Body.RenderRect = SetRectRounded(Entity->Body.Position, Entity->Body.RenderDim);
					if(IsEntityOutOfScreen(Entity, WinDim))
					{
						KillEntity(Pool, i);
					}
				} break;
				case ENTITY_TYPE_WALL:
				case ENTITY_TYPE_WALL_DYNAMIC:
				{
					real32 Drag = 0.0f;
					IntegrateLinearMotion(&Entity->Body, t, WinDim, Drag);
					v2 P = Entity->Body.Position;
					v2 Dim = Entity->Body.Shape.Dim;
					Entity->Body.Rect = Rect32(V2(P.x - Dim.x * 0.5f, P.y - Dim.y * 0.5f), Dim);
					Entity->Body.RenderRect = SetRectRounded(Entity->Body.Position, Entity->Body.RenderDim);
					if(IsEntityOutOfScreen(Entity, WinDim))
					{
						KillEntity(Pool, i);
					}
				} break;
				case ENTITY_TYPE_BOSS_1:
				{
					real32 Drag = 0.0f;
					IntegrateLinearMotion(&Entity->Body, t, WinDim, Drag);
					Entity->Body.RenderRect = SetRectRounded(Entity->Body.Position, Entity->Body.RenderDim);
				} break;
				case ENTITY_TYPE_DILDO:
				{
					real32 Drag = 0;
					IntegrateLinearMotion(&Entity->Body, t, WinDim, Drag);
					Entity->Body.RenderRect = SetRectRounded(Entity->Body.Position, Entity->Body.RenderDim);
					if(IsEntityOutOfScreen(Entity, WinDim))
					{
						KillEntity(Pool, i);
					}
				} break;
				case ENTITY_TYPE_CUM_DROP:
				{
					real32 Drag = 0;
					IntegrateLinearMotion(&Entity->Body, t, WinDim, Drag);
					Entity->Body.RenderRect = SetRectRounded(Entity->Body.Position, Entity->Body.RenderDim);
					if(IsEntityOutOfScreen(Entity, WinDim))
					{
						KillEntity(Pool, i);
					}
				} break;
				case ENTITY_TYPE_WATER_DROP:
				{
					v2 Force = V2(-3000, -9800);
					real32 Drag = 0;
					Entity->Body.Force = Force;
					ComputeTorque(&Entity->Body, Entity->Body.Shape.Dim * 0.5f);
					IntegrateLinearMotion(&Entity->Body, t, WinDim, Drag);
					IntegrateAngularMotion(&Entity->Body, t, WinDim);
					Entity->Body.RenderRect = SetRectRounded(Entity->Body.Position, Entity->Body.RenderDim);
					if(IsEntityOutOfScreen(Entity, WinDim))
					{
						KillEntity(Pool, i);
					}
				} break;
			}
			if(Entity->Translating)
			{
				if(Length(Entity->TranslatePos - Entity->Body.Position) < 5)
				{
					Entity->Translating = false;
					Entity->Body.Velocity = V2();
				}
			}
		}
	}
}

void ResolveEntitiesCollision(entity_pool* Pool, v2 WinDim, real32 t, 
								game_state* GameState, Mix_Chunk** SFX)
{
	for(int i = 0; i < Pool->UsedEntities; ++i)
	{
		entity* EntityA = &Pool->Entities[i];
		if(*GameState == STATE_GAMEOVER)
			break;
		if(EntityA->Alive && !EntityA->Static)
		{
			for(int j = 0; j < Pool->UsedEntities; ++j)
			{
				entity* EntityB = &Pool->Entities[j];
				if(i != j && EntityB->Alive && EntityB->Type != EntityA->Type &&
					TestAABB(&EntityA->Body.BoundingBox, &EntityB->Body.BoundingBox))
				{
					switch(EntityA->Type)
					{
						case ENTITY_TYPE_PLAYER:
						{
							CheckCollision(&EntityA->Body, &EntityB->Body);
							switch(EntityB->Type)
							{
								case ENTITY_TYPE_WALL:
								case ENTITY_TYPE_WALL_DYNAMIC:
								{
									ResolveCollision(&EntityA->Body, WinDim);
									if(IsBodyOnGround(&EntityA->Body, &EntityB->Body))
									{
										EntityA->OnGround = true;
										EntityA->Jump = false;
									}
									if(IsBodyOnSlope(&EntityA->Body, &EntityB->Body))
									{
										EntityA->OnSlope = true;
									}
								} break;
								case ENTITY_TYPE_FIREBALL_1: break;
								case ENTITY_TYPE_SPIKE:
								case ENTITY_TYPE_BOSS_1:
								{
									if(EntityA->Body.CollideInfo.Collide)
										EntityA->HP -= 100;
								} break;
								default:
								{
									if(EntityA->Body.CollideInfo.Collide && !EntityA->Timer.Active)
									{
										//printf("Dead\n");
										//*GameState = STATE_GAMEOVER;
										Mix_PlayChannel(-1, SFX[OOF_SFX], 0);
										StartTimer(&EntityA->Timer);
										EntityA->HP--;
										KillEntity(Pool, j);
										break;
									}
								}
							};
						} break;
						case ENTITY_TYPE_FIREBALL_1:
						{
							CheckCollision(&EntityA->Body, &EntityB->Body);
							switch(EntityB->Type)
							{
								case ENTITY_TYPE_WALL:
								case ENTITY_TYPE_WALL_DYNAMIC:
								{
									ResolveCollisionReflect(&EntityA->Body, WinDim);
								} break;
								case ENTITY_TYPE_BOSS_1:
								{
									if(EntityA->Body.CollideInfo.Collide)
									{
										if(!Mix_Playing(1))
										{
											Mix_Volume(1, 20);
											Mix_PlayChannel(1, SFX[HENTAI_MOAN_SFX], 0);
										}
										SDL_SetTextureColorMod(EntityB->Texture.Texture, 255, 50, 50);
										EntityB->HP -= 2;
										KillEntity(Pool, i);
										printf("HP: %.1f\n", EntityB->HP);
									}
								} break;
							};
						} break;
						case ENTITY_TYPE_WATER_DROP:
						{
							CheckCollision(&EntityA->Body, &EntityB->Body);
							switch(EntityB->Type)
							{
								case ENTITY_TYPE_WALL:
								case ENTITY_TYPE_WALL_DYNAMIC:
								{
									ResolveCollisionReflect(&EntityA->Body, WinDim);
								} break;
							};
						} break;
						case ENTITY_TYPE_CUM_DROP:
						{
							if(TestGJK(&EntityA->Body.Shape.Polygon, &EntityB->Body.Shape.Polygon))
							{
								switch(EntityB->Type)
								{
									case ENTITY_TYPE_WALL:
									case ENTITY_TYPE_WALL_DYNAMIC:
									{
										KillEntity(Pool, i);
									} break;
								};
							}
						} break;
					}
				}
			}	
		}
		
	}
}

void ResetEntityTextureMod(entity_pool* Pool)
{
	for(int i = 0; i < Pool->UsedEntities; ++i)
	{
		entity* Entity = &Pool->Entities[i];
		if(Entity->Alive && Entity->Texture.Texture)
		{
			uint8 r = Entity->Texture.r;
			uint8 g = Entity->Texture.g;
			uint8 b = Entity->Texture.b;
			SDL_SetTextureColorMod(Entity->Texture.Texture, r, g, b);
		}
	}
}

void RenderEntities(entity_pool* Pool, render* Graphics)
{
	for(int i = 0; i < Pool->UsedEntities; ++i)
	{
		entity Entity = Pool->Entities[i];
		if(Entity.Alive)
		{
			switch(Entity.Type)
			{
				case ENTITY_TYPE_PLAYER:
				{
					if(Entity.Body.Angle != 0)
					{
						Entity.Body.Angle -= Entity.Body.Angle;
						Entity.Body.RenderRect.y += 10;
						RenderAnimation(&Entity.Animation[PLAYER_SLIDE_ANIMATION], Graphics, &Entity.Body);
					}
					else if(Entity.Attack)
					{
						RenderAnimation(&Entity.Animation[PLAYER_ATTACK_ANIMATION], Graphics, &Entity.Body);
						if(Entity.Animation[PLAYER_ATTACK_ANIMATION].Ended)
							Pool->Entities[i].Attack = false;
					}
					else if(Length(Entity.Body.Velocity) >= 1.3f && Entity.Body.Colliding)
					{
						RenderAnimation(&Entity.Animation[PLAYER_RUN_ANIMATION], Graphics, &Entity.Body);
					}
					else if(Length(Entity.Body.Velocity) >= 1.3f)
					{
						if(Entity.Body.Velocity.y < -200)
						{
							Pool->Entities[i].Animation[PLAYER_JUMP_ANIMATION].Ended = true;
						}
						RenderAnimation(&Entity.Animation[PLAYER_JUMP_ANIMATION], Graphics, &Entity.Body);
					}
					else
					{
						RenderAnimation(&Entity.Animation[PLAYER_IDLE_ANIMATION], Graphics, &Entity.Body);
					}
				} break;
				case ENTITY_TYPE_WALL:
				case ENTITY_TYPE_WALL_DYNAMIC:
				{
					DrawBodyLines(&Entity.Body, Graphics->Renderer, {255, 255, 255, 255});
				} break;
				case ENTITY_TYPE_FIREBALL_1:
				{
					//DrawBodyLines(&Entity.Body, Graphics->Renderer, {255, 255, 255, 255});
					StartAnimation(Pool->Entities[i].Animation);
					RenderAnimation(Pool->Entities[i].Animation, Graphics, &Entity.Body);
				} break;
				case ENTITY_TYPE_BOSS_1:
				{
					//DrawBodyLines(&Entity.Body, Graphics->Renderer, {255, 255, 255, 255});
					RenderBody(&Entity.Body, Graphics, Entity.Texture.Texture, 0, &Entity.Body.RenderRect);
				} break;
				default:
				{
					//DrawBodyLines(&Entity.Body, Graphics->Renderer, {255, 255, 255, 255});
					RenderBody(&Entity.Body, Graphics, Entity.Texture.Texture, 0, &Entity.Body.RenderRect);
				}
			}			
		}
	}
}

#endif