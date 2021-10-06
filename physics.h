#ifndef PHYSICS_H
#define PHYSICS_H

#include "math.h"
#include "polygon.h"
#include "collision.h"

enum body_type
{
	BODY_CIRCLE,
	BODY_TRIANGLE,
	BODY_RECT,
};

struct particle
{
	v2 Position;
	v2 Velocity;
	
	real32 Mass;
};

struct shape
{
	polygon Polygon;
	v2 Dim;
	real32 Mass;
	real32 MomentOfInertia;
};

struct aabb
{
	v2 Min;
	v2 Max;
};

struct rigid_body
{
	shape Shape;
	v2 Position;
	v2 Velocity;
	real32 Angle;
	real32 AngularVelocity;
	v2 Force;
	real32 Torque;
	SDL_RendererFlip Flip;
	SDL_Rect RenderRect;
	rect32 Rect;
	v2 RenderDim;
	body_type Type;
	collide_info CollideInfo;
	bool Colliding;
	aabb BoundingBox;
};

rigid_body InitializeBody(v2 Pos, v2 Vel, v2 Dim, v2 RenderDim, v2 WinDim, real32 Angle, real32 Mass, 
							body_type Type = BODY_RECT, v2 Offset = {})
{
	rigid_body Body = {};
	Body.Type = Type;
	switch(Type)
	{
		case BODY_RECT:
		{
			Body.Shape.Polygon = CreatePolygon(POLYGON_RECTANGLE, Pos, Dim, WinDim, Offset);
			Body.Shape.Mass = Mass;
			Body.Shape.MomentOfInertia = (Mass * (Dim.x*Dim.x) * (Dim.y*Dim.y))/12;
			Body.Shape.Dim = Dim;
		} break;
		case BODY_TRIANGLE:
		{
			Body.Shape.Polygon = CreatePolygon(POLYGON_TRIANGLE, Pos, Dim, WinDim, Offset);
			Body.Shape.Mass = Mass;
			Body.Shape.MomentOfInertia = (Mass * (Dim.x*Dim.x) * (Dim.y*Dim.y))/12;
			Body.Shape.Dim = Dim;
		} break;
	}
	RotatePolygon(&Body.Shape.Polygon, Angle, WinDim);
	Body.Position = Pos;
	Body.Velocity = Vel;
	Body.Angle = Angle;
	Body.RenderDim = RenderDim;
	Body.Rect = {V2(Pos.x - Dim.x * 0.5f, Pos.y - Dim.y * 0.5f), Dim};
	Body.RenderRect = SetRectRounded(Pos, Dim);
	Body.BoundingBox = {RenderDim * -0.5f + Pos, RenderDim * 0.5f + Pos};
	
	return Body;
}

void ComputeTorque(rigid_body* Body, v2 R = {})
{
	Body->Torque = (R.x * Body->Force.y) - (R.y * Body->Force.x);
}

v2 ComputeForce(particle* Particle)
{
	v2 Force = {0, Particle->Mass * 98.0f};
	return Force;
}

void IntegrateLinearMotion(rigid_body* Body, real32 t, v2 WinDim, real32 Drag)
{
	v2 Delta = {};
	v2 Accel = Body->Force / Body->Shape.Mass;
	//Faking air resistence & friction
	Accel.x -= Body->Velocity.x * Drag;
	Body->Velocity += Accel * t;
	Delta += Body->Velocity * t;
	Body->Position += Delta;
	Body->BoundingBox.Min += Delta;
	Body->BoundingBox.Max += Delta;
	
	UpdateVertices(&Body->Shape.Polygon, Delta, WinDim);
	//PrintV2(Body->Shape.Polygon.MinA);
	//PrintV2(Body->Shape.Polygon.MinB);
}

void IntegrateAngularMotion(rigid_body* Body, real32 t, v2 WinDim)
{
	real32 Theta = 0;
	real32 AngularAccel = Body->Torque / Body->Shape.MomentOfInertia;
	//Faking air resistence & friction
	real32 Drag = 0.1f;
	AngularAccel -= Body->AngularVelocity * Drag;
	Body->AngularVelocity += AngularAccel;
	Theta += Body->AngularVelocity;
	Body->Angle += Theta;
	
	RotatePolygon(&Body->Shape.Polygon, Theta, WinDim);
}

bool IsBodyOnGround(rigid_body* Body, rigid_body* Ground)
{
	switch(Ground->Type)
	{
		case BODY_RECT:
		{
			v2 OffsetA = Body->Shape.Polygon.MinCenter + V2(5, -5);
			v2 OffsetB = Body->Shape.Polygon.MinCenter + V2(-5, -5);
			return IsPointInRectangle(Body->Shape.Polygon.MinCenter, Ground->Rect) ||
				   IsPointInRectangle(OffsetA, Ground->Rect) ||
				   IsPointInRectangle(OffsetB, Ground->Rect);
		} break;
		case BODY_TRIANGLE:
		{
			v2 Offset = Body->Shape.Polygon.MinCenter + V2(0, -5);
			return IsPointInTriangle(Body->Shape.Polygon.MinA, &Ground->Shape.Polygon) || 
					IsPointInTriangle(Body->Shape.Polygon.MinB, &Ground->Shape.Polygon)||	
					IsPointInTriangle(Offset, &Ground->Shape.Polygon);	
		} break;
	}
	return false;
}

bool IsBodyOnSlope(rigid_body* Body, rigid_body* Slope)
{
	return IsPointInTriangle(Body->Shape.Polygon.MinA, &Slope->Shape.Polygon) || 
					IsPointInTriangle(Body->Shape.Polygon.MinB, &Slope->Shape.Polygon);
}

void CheckCollision(rigid_body* BodyA, rigid_body* BodyB)
{
	BodyA->CollideInfo = TestSAT(&BodyA->Shape.Polygon, &BodyB->Shape.Polygon, &BodyA->Colliding);
}

void ResolveCollision(rigid_body* Body, v2 WinDim)
{
	collide_info* Info = &Body->CollideInfo;
	if(Info->Collide)
	{
		v2 Delta = {};
		v2 Normal = Info->Normal;
		if(Info->Sign)
		{
			Delta = (-Normal * Info->Depth);
		}
		else
		{
			Delta = (Normal * Info->Depth);
		}
		Body->Position += Delta;
		Body->BoundingBox.Min += Delta;
		Body->BoundingBox.Max += Delta;
		Body->Velocity += -Project(Body->Velocity, Normal);
		UpdateVertices(&Body->Shape.Polygon, Delta, WinDim);
	}
}

void ResolveCollisionReflect(rigid_body* Body, v2 WinDim)
{
	collide_info* Info = &Body->CollideInfo;
	if(Info->Collide)
	{
		v2 Delta = {};
		v2 Normal = Info->Normal;
		if(Info->Sign)
		{
			Delta = (-Normal * Info->Depth);
		}
		else
		{
			Delta = (Normal * Info->Depth);
		}
		Body->Position += Delta;
		Body->BoundingBox.Min += Delta;
		Body->BoundingBox.Max += Delta;
		Body->Velocity += -2 * Project(Body->Velocity, Normal);
		real32 R = Length(Body->Velocity);
		real32 X = Body->Velocity.y/R;
		real32 Angle = Degrees((real32)asin(X));
#if 1
		if(Angle == 0)
		{
			Angle += 180;
		}
		else if((int)Angle == 90)
		{
			Angle += 90;
		}
		else if(Body->Velocity.x < 0)
		{
			Angle = -Angle;
		}
#endif
		Body->Angle += Angle;
		printf("Angle: %.6f\n", Angle);
		UpdateVertices(&Body->Shape.Polygon, Delta, WinDim);
		RotatePolygon(&Body->Shape.Polygon, Angle, WinDim);
	}
}

bool TestAABB(aabb* A, aabb* B)
{
	if(A->Max.x < B->Min.x || A->Min.x > B->Max.x) return 0;
	if(A->Max.y < B->Min.y || A->Min.y > B->Max.y) return 0;
	
	return 1;
}

void PrintParticleStatus(particle* Particle)
{
	printf("Vx: %.1f Vy: %.1f Px: %.1f Py: %.1f\n",
			Particle->Velocity.x, Particle->Velocity.y, 
			Particle->Position.x, Particle->Position.y);
}

void PrintBodyStatus(rigid_body* Body)
{
	printf("F = (%.2f, %.2f), p = (%.2f, %.2f), V = (%.2f, %.2f), "
			"Theta = %.2f, AngulaVel = %.2f\n", 
			Body->Force.x, Body->Force.y,
			Body->Position.x, Body->Position.y,
			Body->Velocity.x, Body->Velocity.y,
			Body->Angle, Body->AngularVelocity);
}

SDL_Rect SetBodyRenderRect(rigid_body* Body)
{
	SDL_Rect Result = SetRectRounded(Body->Position, Body->Shape.Dim);
	return Result;
}

void DrawBodyLines(rigid_body* Body, SDL_Renderer* Renderer, v4i Color)
{
	SDL_SetRenderDrawColor(Renderer, Color.r, Color.g, Color.b, Color.a);
	SDL_RenderDrawLines(Renderer, Body->Shape.Polygon.Points, Body->Shape.Polygon.PointCount);
}

void RenderBody(rigid_body* Body, render* Graphics, SDL_Texture* Texture, 
				SDL_Rect* ClipRect, SDL_Rect* RenderRect)
{
	RenderCopyEx(Graphics, Texture, ClipRect, RenderRect, Body->Angle, 0, Body->Flip);
}

#endif