// ymx2d.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ymx2d.h"

HINSTANCE hInstance = NULL;
HWND hwnd = NULL;

IDirect3DDevice9* device = NULL;
IDirect3DVertexBuffer9* VB = NULL;
IDirect3DIndexBuffer9* IB = NULL;
IDirect3DVertexBuffer9* PVB = NULL;
IDirect3DVertexBuffer9* PVB2 = NULL;//use Vertex instead of ParticleVertex, when hardware not support POINTSIZE



