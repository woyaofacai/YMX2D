#include "stdafx.h"
#include "ymx2d.h"
#include "ymxvertex.h"

Vertex::Vertex()
	:x(0), y(0),z(0),color(0), u(0), v(0)
{

}

Vertex::Vertex(float x, float y, float z, D3DCOLOR color, float u /* = 0.0f */, float v/* =0.0f */)
	:x(x), y(y), z(z), color(color), u(u), v(v)
{

}
