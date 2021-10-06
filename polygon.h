#ifndef POLYGON_H
#define POLYGON_H

#define POLYGON_RECTANGLE_VERTICES_COUNT 4
#define POLYGON_TRIANGLE_VERTICES_COUNT 3

enum polygon_type
{
	POLYGON_TRIANGLE,
	POLYGON_RECTANGLE,
};

struct polygon
{
	v2* Vertices;
	v2 Center;
	v2 MinA;
	v2 MinB;
	v2 MinCenter;
	v2 MaxCenter;
	v2 Dim;
	SDL_Point* Points;
	int Count;
	int PointCount;
	real32 Theta;
	polygon_type Type;
};

polygon CreatePolygon(polygon_type Type, v2 Pos, v2 Dim, v2 WinDim, v2 Offset)
{
	polygon Polygon = {};
	Polygon.Dim = Dim;
	Polygon.Type = Type;
	switch(Type)
	{
		case POLYGON_TRIANGLE:
		{
			Polygon.Count = POLYGON_TRIANGLE_VERTICES_COUNT;
			Polygon.PointCount = POLYGON_TRIANGLE_VERTICES_COUNT + 1;
			Polygon.Vertices = (v2*)malloc(sizeof(v2) * Polygon.Count);
			Polygon.Points = (SDL_Point*)malloc(sizeof(SDL_Point) * Polygon.PointCount);
			
			Polygon.Vertices[0] = V2(0,Dim.y*0.5f) + Pos;
			Polygon.Vertices[1] = V2(-Dim.x*0.5f, -Dim.y*0.5f) + Pos;
			Polygon.Vertices[2] = V2(Dim.x*0.5f, -Dim.y*0.5f) + Pos;
			
			Polygon.Center += Pos;
			Polygon.Theta = 0;
			
			for(int i = 0; i < Polygon.PointCount; ++i)
			{
				Polygon.Points[i].x = (int)Polygon.Vertices[i == Polygon.Count ? 0 : i].x + 
										(int)(WinDim.x * 0.5f); 
				Polygon.Points[i].y = -((int)Polygon.Vertices[i == Polygon.Count ? 0 : i].y - 
										(int)(WinDim.y *0.5f)); 
			}
		} break;
		case POLYGON_RECTANGLE:
		{
			Polygon.Count = POLYGON_RECTANGLE_VERTICES_COUNT;
			Polygon.PointCount = POLYGON_RECTANGLE_VERTICES_COUNT + 1;
			
			Polygon.Vertices = (v2*)malloc(sizeof(v2) * Polygon.Count);
			Polygon.Points = (SDL_Point*)malloc(sizeof(SDL_Point) * Polygon.PointCount);
			
			Polygon.Vertices[0] = V2(-Dim.x * 0.5f, Dim.y  * 0.5f) + Pos + Offset; //TOP LEFT
			Polygon.Vertices[1] = V2(-Dim.x * 0.5f, -Dim.y * 0.5f) + Pos + Offset; //BOTTOM LEFT
			Polygon.Vertices[2] = V2(Dim.x  * 0.5f, -Dim.y * 0.5f) + Pos + Offset; //BOTTOM RIGHT
			Polygon.Vertices[3] = V2(Dim.x  * 0.5f, Dim.y  * 0.5f) + Pos + Offset; //TOP RIGHT
			
			Polygon.MinA = V2(Pos.x - (Dim.x*0.5f), Pos.y - (Dim.y*0.5f));
			Polygon.MinB = V2(Pos.x + (Dim.x*0.5f), Pos.y - (Dim.y*0.5f));
			Polygon.MinCenter = V2(Pos.x, Pos.y - (Dim.y*0.5f));
			Polygon.MaxCenter = V2(Pos.x, Pos.y + (Dim.y*0.5f));
			
			Polygon.Center += Pos;
			Polygon.Theta = 0;
			
			for(int i = 0; i < Polygon.PointCount; ++i)
			{
				Polygon.Points[i].x = (int)Polygon.Vertices[i == Polygon.Count ? 0 : i].x + 
										(int)(WinDim.x * 0.5f); 
				Polygon.Points[i].y = -((int)Polygon.Vertices[i == Polygon.Count ? 0 : i].y - 
										(int)(WinDim.y *0.5f)); 
			}
		} break;
	}
	return Polygon;
}

real32 TriangleArea(v2 p1, v2 p2, v2 p3) 
{  //find area of triangle formed by p1, p2 and p3
   return (real32)abs((p1.x*(p2.y-p3.y) + p2.x*(p3.y-p1.y)+ p3.x*(p1.y-p2.y))/2.0);
}

bool IsPointInTriangle(v2 P, polygon* Triangle)
{
	real32 Area = TriangleArea(Triangle->Vertices[0], Triangle->Vertices[1], Triangle->Vertices[2]);
	real32 AreaA = TriangleArea(P, Triangle->Vertices[1], Triangle->Vertices[2]);
	real32 AreaB = TriangleArea(Triangle->Vertices[0], P, Triangle->Vertices[2]);
	real32 AreaC = TriangleArea(Triangle->Vertices[0], Triangle->Vertices[1], P);
	
	return (Area == AreaA + AreaB + AreaC);
}

bool IsPointInRectangle(v2 P, rect32 Rect)
{
	if(P.x < Rect.Pos.x)
	{
		return false;
	}
	else if(P.x > Rect.Pos.x + Rect.Dim.x)
	{
		return false;
	}
	else if(P.y < Rect.Pos.y)
	{
		return false;
	}
	else if(P.y > Rect.Pos.y + Rect.Dim.y)
	{
		return false;
	}
	return true;
}

void RotatePolygon(polygon* Shape, real32 Theta, v2 WinDim)
{
#if 1
	for(int i = 0; i < Shape->Count; ++i)
	{
		Shape->Vertices[i] -= Shape->Center;
		Shape->Vertices[i] = Rotate(Shape->Vertices[i], Radians(Theta));
		Shape->Vertices[i] += Shape->Center;
	}
	
	Shape->MinA -= Shape->Center;
	Shape->MinA = Rotate(Shape->MinA, Radians(Theta));
	Shape->MinA += Shape->Center;
	
	Shape->MinB -= Shape->Center;
	Shape->MinB = Rotate(Shape->MinB, Radians(Theta));
	Shape->MinB += Shape->Center;
	
	Shape->MinCenter -= Shape->Center;
	Shape->MinCenter = Rotate(Shape->MinCenter, Radians(Theta));
	Shape->MinCenter += Shape->Center;
	
	Shape->MaxCenter -= Shape->Center;
	Shape->MaxCenter = Rotate(Shape->MaxCenter, Radians(Theta));
	Shape->MaxCenter += Shape->Center;
#endif
#if 0
	v2 RotatedXAxis = V2(Cos(Theta), Sin(Theta));
	v2 RotatedYAxis = Perp_v2(RotatedXAxis);
	
	for(int i = 0; i < Shape->Count; ++i)
	{
		real32 x = Shape->Vertices[i].x;
		real32 y = Shape->Vertices[i].y;
		Shape->Vertices[i] = (x*RotatedXAxis) + (y*RotatedYAxis);
	}
	
	Shape->MinA = (Shape->MinA.x * RotatedXAxis) + (Shape->MinA.y * RotatedYAxis);
	
	Shape->MinB = (Shape->MinB.x * RotatedXAxis) + (Shape->MinB.y * RotatedYAxis);
	
	Shape->MinCenter = (Shape->MinCenter.x * RotatedXAxis) + (Shape->MinCenter.y * RotatedYAxis);
#endif
	for(int i = 0; i < Shape->Count + 1; ++i)
	{
		Shape->Points[i].x = (int)Shape->Vertices[i == Shape->Count ? 0 : i].x + (int)(WinDim.x * 0.5f);
		Shape->Points[i].y = -((int)Shape->Vertices[i == Shape->Count ? 0 : i].y - (int)(WinDim.y *0.5f));
	}
}

void OffsetTopVertices(polygon* Shape, v2 Offset)
{
	switch(Shape->Type)
	{
		case POLYGON_RECTANGLE:
		{
			Shape->Vertices[0] += Offset;
			Shape->Vertices[3] += Offset;
		} break;
	}
}

void UpdateVertices(polygon* Shape, v2 Delta, v2 WinDim)
{
	if(!(Delta.x == 0 && Delta.y == 0))
	{
		for(int i = 0; i < Shape->Count; ++i)
		{
			Shape->Vertices[i] += Delta;
		}
		
		Shape->MinA += Delta;
		Shape->MinB += Delta;
		Shape->MinCenter += Delta;
		Shape->MaxCenter += Delta;
	}
	
	for(int i = 0; i < Shape->Count + 1; ++i)
	{
		Shape->Points[i].x = (int)Shape->Vertices[i == Shape->Count ? 0 : i].x + (int)(WinDim.x * 0.5f);
		Shape->Points[i].y = -((int)Shape->Vertices[i == Shape->Count ? 0 : i].y - (int)(WinDim.y *0.5f));
	}
	Shape->Center += Delta;
}

void ResetVertices(polygon* Shape, v2 WinDim, v2 Pos)
{
	switch(Shape->Type)
	{
		case POLYGON_RECTANGLE:
		{
			Shape->Vertices[0] = V2(-Shape->Dim.x * 0.5f, Shape->Dim.y  * 0.5f) + Pos;
			Shape->Vertices[1] = V2(-Shape->Dim.x * 0.5f, -Shape->Dim.y * 0.5f) + Pos;
			Shape->Vertices[2] = V2(Shape->Dim.x  * 0.5f, -Shape->Dim.y * 0.5f) + Pos;
			Shape->Vertices[3] = V2(Shape->Dim.x  * 0.5f, Shape->Dim.y  * 0.5f) + Pos;
			
			Shape->MinA = V2(Pos.x - (Shape->Dim.x*0.5f), Pos.y - (Shape->Dim.y*0.5f));
			Shape->MinB = V2(Pos.x + (Shape->Dim.x*0.5f), Pos.y - (Shape->Dim.y*0.5f));
			Shape->MinCenter = V2(Pos.x, Pos.y - (Shape->Dim.y*0.5f));
			
			Shape->Center = Pos;
			UpdateVertices(Shape, V2(), WinDim);
		} break;
	}
}

#endif