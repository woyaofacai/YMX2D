// stdafx.h : include file for standard ymxtem include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <mmsystem.h>
#include <tchar.h>
#include <d3dx9.h>
#include <time.h>
#include <stdlib.h>
#include <vector>
#include <dsound.h>
#include <dmksctrl.h>
#include <dmusici.h>
#include <dmusicc.h>
#include <dmusicf.h>
#include <direct.h>

//Lua
extern "C"{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
};


extern IDirect3DDevice9* device;
extern IDirect3DVertexBuffer9* VB;
extern IDirect3DIndexBuffer9* IB;
extern IDirect3DVertexBuffer9* PVB;
extern IDirect3DVertexBuffer9* PVB2; //use Vertex instead of ParticleVertex, when hardware not support POINTSIZE
extern HINSTANCE hInstance;
extern HWND hwnd;

//lua
extern lua_State* g_LuaState;

#define VERTEX_FVF D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1

// TODO: reference additional headers your program requires here
