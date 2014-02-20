#include "stdafx.h"
#include "ymxluautil.h"
#include "ymxluacore.h"

int grap_RenderTriangle(lua_State* L)
{
	YmxGraphics* g = (YmxGraphics*)lua_touserdata(L, 1);
	YmxVertex vs[3];
	if(!GetVertexFromLuaArg(L, 2, &vs[0]) || !GetVertexFromLuaArg(L, 3, &vs[1]) || !GetVertexFromLuaArg(L, 4, &vs[2]))
	{
		lua_pushboolean(L, 0);
		return 1;
	}
	YMXCOLOR color;
	if(!GetColorFromLuaArg(L, 5, &color))
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	float z = 0.0f;
	if(lua_isnumber(L, 6))
	{
		z = (float)lua_tonumber(L, 6);
	}

	g->RenderTriangle(vs, color, z);
	lua_pushboolean(L, 1);
	return 1;
}

int grap_RenderRectangle(lua_State* L)
{
	YmxGraphics* g = (YmxGraphics*)lua_touserdata(L, 1);
	YmxRect rect;
	if(!GetRectFromLuaArg(L, 2, &rect))
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	YMXCOLOR color;
	if(!GetColorFromLuaArg(L, 3, &color))
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	float z = 0.0f;
	if(lua_isnumber(L, 4))
	{
		z = (float)lua_tonumber(L, 4);
	}

	g->RenderRectangle(&rect, color, z);

	lua_pushboolean(L, 1);
	return 1;
}

int grap_RotateVertex(lua_State* L)
{
	YmxVertex v, pivot;
	if(!GetVertexFromLuaArg(L, 1, &v) || !GetVertexFromLuaArg(L, 2, &pivot))
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	float theta = 0.0f;
	if(lua_isnumber(L, 3))
	{
		theta = (float)lua_tonumber(L, 3);
	}

	RotateVertex(&v, &pivot, theta);
	lua_pushnumber(L, v.x);
	lua_pushnumber(L, v.y);
	return 2;
}


int grap_RenderTexture(lua_State* L)
{
	YmxGraphics* g = (YmxGraphics*)lua_touserdata(L, 1);
	LuaProxy* pTexProxy = (LuaProxy*)luaL_checkudata(L, 2, TEXTURE_MT_NAME);
	LPYMXTEXTURE pTexture = (LPYMXTEXTURE)pTexProxy->instance;
	if(pTexture == NULL)
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	if(!lua_isnumber(L, 3) || !lua_isnumber(L, 4))
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	float x = (float)lua_tonumber(L, 3);
	float y = (float)lua_tonumber(L, 4);

	float z = 0.0f;
	if(lua_gettop(L) > 4 && lua_isnumber(L, 5))
	{
		z = (float)lua_tonumber(L, 5);
	}

	if(lua_gettop(L) <= 5)
	{
		g->RenderTexture(pTexture, x, y, z);
	}
	else {
		YmxRect srcRect;
		if(lua_istable(L, 6) && GetRectFromLuaArg(L, 6, &srcRect))
		{
			g->RenderTexture(pTexture, x, y, &srcRect, z);
		}
		else {
			g->RenderTexture(pTexture, x, y, z);
		}
	}

	lua_pushboolean(L, 1);
	return 1;
}

int grap_RenderPolygon(lua_State* L)
{
	YmxGraphics* g = (YmxGraphics*)lua_touserdata(L, 1);

	if(!lua_istable(L, 2))
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	int vcount = lua_objlen(L, 2);
	YmxVertex* vertexs = new YmxVertex[vcount];
	int i;	
	for (i = 0; i < vcount; i++)
	{
		lua_rawgeti(L, 2, i + 1);
		if(!GetVertexFromLuaArg(L, lua_gettop(L), &vertexs[i]))
		{
			lua_pop(L, 1);
			delete [] vertexs;
			lua_pushboolean(L, 0);
			return 1;
		}
		lua_pop(L, 1);
	}

	YMXCOLOR color;
	if(!GetColorFromLuaArg(L, 3, &color))
	{
		delete [] vertexs;
		lua_pushboolean(L, 0);
		return 1;
	}

	float z = 0.0f;
	if(lua_isnumber(L, 4))
	{
		z = (float)lua_tonumber(L, 4);
	}

	g->RenderPolygon(vertexs, vcount, color, z);

	delete [] vertexs;
	lua_pushboolean(L, 1);
	return 1;
}