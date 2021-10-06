#ifndef COLLISION_H
#define COLLISION_H

#include "polygon.h"

struct collide_info
{
	bool Collide;
	v2 Normal;
	real32 Depth;
	v2 Center;
	bool Sign;
};

v2 ProjectShapeToAxis(polygon* Shape, v2 Axis)
{
	v2 Result = V2();
	real32 Min = Dot(Shape->Vertices[0], Axis);
	real32 Max = Min;
	
	for(int i = 1; i < Shape->Count; ++i)
	{
		real32 P = Dot(Shape->Vertices[i], Axis);
		if(P < Min)
		{
			Min = P;
		}
		else if(P > Max)
		{
			Max = P;
		}
	}
	Result = V2(Min, Max);
	return Result;
}

bool IsProjectionOverlap(v2 ProjA, v2 ProjB)
{
	return !(ProjB.y < ProjA.x || ProjA.y < ProjB.x);
}

real32 GetOverlap(v2 ProjA, v2 ProjB)
{
	real32 Result = Min(ProjA.y, ProjB.y) - Max(ProjA.x, ProjB.x);
	return Result;
}

collide_info TestSAT(polygon* ShapeA, polygon* ShapeB, bool* CollideFlag)
{
	collide_info CollideResult = {};
	CollideResult.Collide = true;
	real32 Overlap = FLT_MAX;
	v2 Smallest = {};
	//Free this after function ends
	v2* AxesA = (v2*)malloc(sizeof(v2) * ShapeA->Count);
	v2* AxesB = (v2*)malloc(sizeof(v2) * ShapeB->Count);
	
	//Generate Axes
	for(int Index = 0; Index < ShapeA->Count; ++Index)
	{
		v2 Point1 = ShapeA->Vertices[Index];
		v2 Point2 = ShapeA->Vertices[Index+1 == ShapeA->Count ? 0 : Index+1];
		
		v2 Edge = Point1 - Point2;
		
		v2 Normal = Perp_v2(Edge);
		AxesA[Index] = Normalize(Normal);
	}
	
	for(int Index = 0; Index < ShapeB->Count; ++Index)
	{
		v2 Point1 = ShapeB->Vertices[Index];
		v2 Point2 = ShapeB->Vertices[Index+1 == ShapeB->Count ? 0 : Index+1];
		
		v2 Edge = Point1 - Point2;
		
		v2 Normal = Perp_v2(Edge);
		AxesB[Index] = Normalize(Normal);
	}
	
	//Projecting Axes
	for(int i = 0; i < ShapeA->Count; ++i)
	{
		v2 ProjA = ProjectShapeToAxis(ShapeA, AxesA[i]);
		v2 ProjB = ProjectShapeToAxis(ShapeB, AxesA[i]);
		
		if(!IsProjectionOverlap(ProjA, ProjB))
		{
			CollideResult.Collide = false;
		}
		else
		{
			real32 TempOverlap = GetOverlap(ProjA, ProjB);
			if(TempOverlap < Overlap)
			{
				Overlap = TempOverlap;
				Smallest = AxesA[i];
				CollideResult.Sign = ProjA.x < ProjB.x;
			}
		}
	}
	for(int i = 0; i < ShapeB->Count; ++i)
	{
		v2 ProjA = ProjectShapeToAxis(ShapeA, AxesB[i]);
		v2 ProjB = ProjectShapeToAxis(ShapeB, AxesB[i]);
		
		if(!IsProjectionOverlap(ProjA, ProjB))
		{
			CollideResult.Collide = false;
		}
		else
		{
			real32 TempOverlap = GetOverlap(ProjA, ProjB);
			if(TempOverlap < Overlap)
			{
				Overlap = TempOverlap;
				Smallest = AxesB[i];
				CollideResult.Sign = ProjA.x < ProjB.x;
			}
		}
	}
	
	if(CollideResult.Collide)
	{
		CollideResult.Normal = Smallest;
		CollideResult.Depth = Overlap;
		CollideResult.Center = ShapeB->Center;
		*CollideFlag = true;
	}
	
	free(AxesA);
	free(AxesB);
	
	return CollideResult;
}

int IndexOfFurthestPoint(v2* Vertices, int Count, v2 d)
{
	int Index = 0;
	real32 Max = Dot(d, Vertices[Index]);
	
	for(int i = 1; i < Count; ++i)
	{
		real32 Project = Dot(d, Vertices[i]);
		if(Project > Max)
		{
			Max = Project;
			Index = i;
		}
	}
	return Index;
}

v2 SupportGJK(v2* VerticesA, v2* VerticesB, int CountA, int CountB, v2 d)
{
	int IndexA = IndexOfFurthestPoint(VerticesA, CountA, d);
	int IndexB = IndexOfFurthestPoint(VerticesB, CountB, -d);
	
	return VerticesA[IndexA] - VerticesB[IndexB];
}

bool TestGJK(polygon* ShapeA, polygon* ShapeB)
{
	int Index = 0;
	
	v2 Simplex[3] = {};
	
	v2 d = ShapeB->Center - ShapeA->Center;
	
	Simplex[Index] = SupportGJK(ShapeA->Vertices, ShapeB->Vertices, ShapeA->Count, ShapeB->Count, d);
	
	v2 a = Simplex[0];
	if(Dot(a, d) <= 0)
	{
		return false;
	}
	d = -a;
	
	v2 b, c, ao, ab, ac, ab_perp, ac_perp;
	
	while(true)
	{
		a = Simplex[++Index] = SupportGJK(ShapeA->Vertices, ShapeB->Vertices, 
											ShapeA->Count, ShapeB->Count, d);
											
		if(Dot(a, d) <= 0)
		{
			return false;
		}
		
		ao = -a;
		
		if(Index < 2)
		{
			b = Simplex[0];
			ab = b - a;
			d = TripleProduct(ab, ao, ab);
			if(Length(d) * Length(d) == 0)
			{
				d = Perp_v2(ab);
			}
			continue;
		}
		
		b = Simplex[1];
        c = Simplex[0];
        ab = b - a; // from point A to B
        ac = c - a; // from point A to C
        
        ac_perp = TripleProduct(ab, ac, ac);
        
        if (Dot(ac_perp, ao) >= 0) 
		{
            d = ac_perp; // new direction is normal to AC towards Origin
        } 
		else 
		{
            ab_perp = TripleProduct(ac, ab, ab);
            
            if (Dot(ab_perp, ao) < 0)
                return true; // collision
            
            Simplex[0] = Simplex[1]; // swap first element (point C)

            d = ab_perp; // new direction is normal to AB towards Origin
        }
        
        Simplex[1] = Simplex[2]; // swap element in the middle (point B)
        --Index;
	}
	
	return false;
}

#endif