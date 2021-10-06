#ifndef MATH_H
#define MATH_H

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

#define PI 3.1415926
#define ONE_OVER_180 0.005555

real32 Cos(real32 Theta);
real32 Sin(real32 Theta);

struct v2 
{
	real32 x, y;
};

struct v2i
{
	int x, y;
};

union v4i
{
	struct
	{
		int x, y, z, w;
	};
	struct
	{
		int r, g, b, a;
	};
};

struct rect32
{
	v2 Pos;
	v2 Dim;
};

rect32 Rect32(v2 Pos, v2 Dim)
{
	rect32 Result = {Pos, Dim};
	return Result;
}

v2 V2(real32 x = 0, real32 y = 0)
{
	v2 Result = {x, y};
	return Result;
}

v2 V2(v2i A)
{
	v2 Result = {(real32)A.x, (real32)A.y};
	return Result;
}

v2i V2i(int x = 0, int y = 0)
{
	v2i Result = {x, y};
	return Result;
}

v2 operator+(v2 A, v2 B)
{
	v2 Result = {A.x + B.x, A.y + B.y};
	return Result;
}

v2 operator+(v2 A, real32 N)
{
	v2 Result = {A.x + N, A.y + N};
	return Result;
}

v2 operator-(v2 A, v2 B)
{
	v2 Result = {A.x - B.x, A.y - B.y};
	return Result;
}

v2 operator-(v2 A)
{
	v2 Result = {-A.x, -A.y};
	return Result;
}

v2 operator*(v2 A, real32 k)
{
	v2 Result = {k * A.x, k * A.y};
	return Result;
}

v2 operator*(real32 k, v2 A)
{
	v2 Result = {k * A.x, k * A.y};
	return Result;
}

v2 operator/(v2 A, real32 k)
{
	v2 Result = {A.x/k, A.y/k};
	return Result;
}

v2 &operator+=(v2 &A, v2 B)
{
	A = A + B;
	return A;
}

v2 &operator+=(v2 &A, real32 N)
{
	A = A + N;
	return A;
}

v2 &operator-=(v2 &A, v2 B)
{
	A = A - B;
	return A;
}

bool operator>(v2 A, real32 N)
{
	return A.x > N || A.y > N;
}

bool operator<(v2 A, real32 N)
{
	return A.x < N || A.y < N;
}

bool operator<=(v2 A, real32 N)
{
	return A.x <= N || A.y <= N;
}

SDL_Point operator+(SDL_Point P, v2 A)
{
	SDL_Point Result = {P.x + (int)A.x, P.y + (int)A.y};
	return Result;
}

SDL_Point &operator+=(SDL_Point &P, v2 A)
{
	P = P + A;
	return P;
}

real32 Length(v2 A)
{
	real32 Result = (real32)sqrt((A.x * A.x) + (A.y * A.y));
	return Result;
}

v2 Rotate(v2 A, real32 Theta)
{
	v2 Result = V2(Cos(Theta)*A.x - Sin(Theta)*A.y, Sin(Theta)*A.x + Cos(Theta)*A.y);
	return Result;
}

v2 Perp_v2(v2 A)
{
	v2 Result = {-A.y, A.x};
	return Result;
}

v2 Normalize(v2 A)
{
	v2 Result = A / Length(A);
	return Result;
}

real32 Dot(v2 A, v2 B)
{
	real32 Result = {A.x*B.x + A.y*B.y};
	return Result;
}

v2 Project(v2 A, v2 B)
{
	v2 Result = Dot(A, B) * Normalize(B); //Project A onto B;
	return Result;
}

v2 TripleProduct(v2 A, v2 B, v2 C)
{
	v2 Result;
	
	real32 ac = Dot(A, C);
	real32 bc = Dot(B, C);
	
	Result = V2(B.x * ac - A.x * bc, B.y * ac - A.y * bc);
	
	return Result;
}

real32 Min(real32 A, real32 B)
{
	real32 Result = 0;
	if(A < B)
		Result = A;
	else if(B < A)
		Result = B;
	return Result;
}

real32 Max(real32 A, real32 B)
{
	real32 Result = 0; 
	if(A > B)
		Result = A;
	else if(B > A)
		Result = B;
	return Result;
}

int Round32(real32 n)
{
	int Result = (int)(n + 0.5f);
	return Result;
}

real32 Radians(real32 Theta)
{
	real32 Result = (real32)(Theta * (PI/180));
	return Result;
}

real32 Degrees(real32 Theta)
{
	real32 Result = (real32)(Theta * (180/PI));
	return Result;
}

real32 Cos(real32 Theta)
{
	return (real32)cos((double)Theta);
}

real32 Sin(real32 Theta)
{
	return (real32)sin((double)Theta);
}

real32 Rand32(real32 Low, real32 High)
{
	real32 Result = Low + (real32)(rand() / (real32)(RAND_MAX/(High - Low)));
	return Result;
}

void PrintV2(v2 A)
{
	printf("(%.1f, %.1f)\n", A.x, A.y);
}

#endif