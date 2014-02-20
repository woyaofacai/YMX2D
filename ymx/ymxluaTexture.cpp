#include "stdafx.h"
#include "ymxluautil.h"
#include "ymxluacore.h"

const char* TEXTURE_MT_NAME = "ymxlua.texture";

const struct luaL_Reg textureLib_m[] = 
{
	{"getWidth", tex_GetWidth},
	{"getHeight", tex_GetHeight},
	{"setAlpha", tex_SetAlpha},
	{"release", tex_Release},
	{0, 0}
};

int tex_Load(lua_State* L)
{
	const char* arg = luaL_checkstring(L, -1);
	TCHAR fileName[1024];
	ConvertAsciiToPTStrs(arg, fileName, 1024);

	LPYMXTEXTURE pTexture = GetGameObject()->LoadTexture(fileName);
	if(pTexture == NULL) {
		lua_pushnil(L);
		return 1;
	}
	LuaProxy* proxy = (LuaProxy *)lua_newuserdata(L, sizeof(LuaProxy));

	proxy->instance = pTexture;
	proxy->type = TEXTURE_PROXY;

	luaL_getmetatable(L, TEXTURE_MT_NAME);
	lua_setmetatable(L, -2);

	return 1;
}

int tex_GetWidth(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, TEXTURE_MT_NAME);
	LPYMXTEXTURE pTexture = (LPYMXTEXTURE)proxy->instance;

	float width;
	if(lua_toboolean(L, 2))
		width = pTexture->GetMemWidth();
	else 
		width = pTexture->GetWidth();
	lua_pushnumber(L, width);
	return 1;
}

int tex_GetHeight(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, TEXTURE_MT_NAME);
	LPYMXTEXTURE pTexture = (LPYMXTEXTURE)proxy->instance;

	float height;
	if(lua_toboolean(L, 2))
		height = pTexture->GetMemHeight();
	else 
		height = pTexture->GetHeight();
	lua_pushnumber(L, height);
	return 1;
}

int tex_SetAlpha(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, TEXTURE_MT_NAME);
	LPYMXTEXTURE pTexture = (LPYMXTEXTURE)proxy->instance;

	float alpha = (float)luaL_checknumber(L, 2);
	pTexture->SetAlpha(alpha);
	return 0;
}

int tex_Release(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, TEXTURE_MT_NAME);
	LPYMXTEXTURE pTexture = (LPYMXTEXTURE)proxy->instance;

	if(pTexture == NULL) {
		return 0;
	}

	pTexture->Release();
	return 0;
}