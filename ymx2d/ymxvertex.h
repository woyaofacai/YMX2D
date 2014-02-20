#include "ymx2d.h"

#ifndef YMXVERTEX_FUNCTION_CLASS_FORM_H
#define YMXVERTEX_FUNCTION_CLASS_FORM_H

struct Vertex 
{
	float x, y, z;
	union{
		YMXCOLOR color;
		struct{
			byte blue;
			byte green;
			byte red;
			byte alpha;
		};
	};
	float u, v; 
	static const DWORD FVF;
	Vertex();
	Vertex(float x, float y, float z, YMXCOLOR color, float u = 0.0f, float v=0.0f);
};

struct ConciseVertex
{
	float x, y, z;
	union{
		YMXCOLOR color;
		struct{
			byte blue;
			byte green;
			byte red;
			byte alpha;
		};
	};
	static const DWORD FVF;
};

typedef struct ParticleVertex
{
	float x, y, z;
	float size;
	union{
		YMXCOLOR color;
		struct{
			byte blue;
			byte green;
			byte red;
			byte alpha;
		};
	};
	static const DWORD FVF;
};

struct YMX2D_API YmxParticle
{
	float x, y;
	float size;
	union {
		YMXCOLOR color;
		struct{
			byte blue;			
			byte green;
			byte red;
			byte alpha;
		};
	};
	YMXVECTOR velocity;
	YMXVECTOR acceleration;
	float life;
	YmxParticle* next;
};

#endif