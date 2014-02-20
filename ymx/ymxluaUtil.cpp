#include "stdafx.h"
#include "ymxluautil.h"

void ConvertAsciiToPTStrs(const char* s, TCHAR buf[], size_t bufSize)
{
#ifdef _UNICODE
	MultiByteToWideChar(CP_ACP, 0, s, -1, buf, bufSize);
#else
	strncpy_s(buf, bufSize, s, _TRUNCATE);
#endif
}

void ConvertPTStrsToAscii(PCTSTR s, char buf[], size_t bufSize)
{
#ifdef _UNICODE
	WideCharToMultiByte(CP_ACP, 0, s, -1, buf, bufSize, NULL, NULL);
#else
	strncpy_s(buf, bufSize, s, _TRUNCATE);
#endif
}

void CreateMetaTable(lua_State* L, const char* name, const luaL_Reg m[])
{
	luaL_newmetatable(L, name);
	lua_pushvalue(L, -1);
	lua_setfield(L, -2, "__index");
	if(m != NULL)
		luaL_register(L, NULL, m);
}

LPYMXCOMPONENT GetComponentByTblFromLuaArg(lua_State* L, int idx)
{
	if(!lua_istable(L, idx))
		return NULL;

	lua_pushstring(L, "id");
	lua_rawget(L, idx);

	LPYMXCOMPONENT pComponent = NULL;
	if(lua_isnumber(L, -1))
	{
		int componentId = lua_tointeger(L, -1);
		pComponent = GetGameObject()->GetComponentById(componentId);
	}
	lua_pop(L, 1);
	return pComponent;
}

LPYMXCOMPONENT GetComponentByIdOrNameFromLuaArg(lua_State* L, int idx)
{
	LPYMXCOMPONENT pComponent = NULL;
	if(lua_isnumber(L, idx))
	{
		int id = (int)lua_tointeger(L, idx);
		pComponent = GetGameObject()->GetComponentById(id);
	}
	else if(lua_isstring(L, idx)) {
		const char* name = lua_tostring(L, idx);
		pComponent = GetGameObject()->GetComponentByName(name);
	}

	return pComponent;
}

LPYMXCOMPONENT GetComponentFromLuaArg(lua_State* L, int idx)
{
	LPYMXCOMPONENT pComponent = NULL;
	pComponent = GetComponentByIdOrNameFromLuaArg(L, idx);
	if(pComponent != NULL)
		return pComponent;

	pComponent = GetComponentByTblFromLuaArg(L, idx);
	return pComponent;
}


BOOL GetRectFromLuaArg(lua_State* L, int idx, YmxRect* rect)
{
	if(!lua_istable(L, idx)) return FALSE;
	if(lua_objlen(L, idx) < 4) return FALSE;

	lua_rawgeti(L, idx, 1);
	lua_rawgeti(L, idx, 2);
	lua_rawgeti(L, idx, 3);
	lua_rawgeti(L, idx, 4);

	rect->left = (float)lua_tonumber(L, -4);
	rect->top = (float)lua_tonumber(L, -3);
	rect->right = (float)lua_tonumber(L, -2);
	rect->bottom = (float)lua_tonumber(L, -1);

	lua_pop(L, 4);

	return TRUE;
}

BOOL GetColorFromLuaArg(lua_State* L, int idx, YMXCOLOR* color)
{
	if(!lua_istable(L, idx))
		return FALSE;

	int tblLen = lua_objlen(L, idx);

	if(tblLen < 3) 
		return FALSE;

	int a, r, g, b;
	if(tblLen == 4)
	{
		lua_rawgeti(L, idx, 1);
		lua_rawgeti(L, idx, 2);
		lua_rawgeti(L, idx, 3);
		lua_rawgeti(L, idx, 4);

		a = max(0, min(lua_tointeger(L, -4), 255));
		r = max(0, min(lua_tointeger(L, -3), 255));
		g = max(0, min(lua_tointeger(L, -2), 255));
		b = max(0, min(lua_tointeger(L, -1), 255));

		lua_pop(L, 4);
	}
	else {
		lua_rawgeti(L, idx, 1);
		lua_rawgeti(L, idx, 2);
		lua_rawgeti(L, idx, 3);

		a = 255;
		r = max(0, min(lua_tointeger(L, -3), 255));
		g = max(0, min(lua_tointeger(L, -2), 255));
		b = max(0, min(lua_tointeger(L, -1), 255));

		lua_pop(L, 3);
	}

	*color = YMX_ARGB(a, r, g, b);
	return TRUE;
}

BOOL GetVertexFromLuaArg(lua_State* L, int idx, YmxVertex* v)
{
	if(!lua_istable(L, idx))
		return FALSE;

	// Get x; the x value is the first element array, or the element with the key 'x'
	lua_rawgeti(L, idx, 1);
	if(lua_isnumber(L, -1))
	{
		v->x = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);
	}
	else {
		lua_pop(L, 1);
		lua_pushstring(L, "x");
		lua_rawget(L, idx);
		if(lua_isnumber(L, -1))
		{
			v->x = (float)lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
		else {
			lua_pop(L, 1);
			return FALSE;
		}
	}

	// Get y; the y value is the second element array, or the element with the key 'y'
	lua_rawgeti(L, idx, 2);
	if(lua_isnumber(L, -1))
	{
		v->y = (float)lua_tonumber(L, -1);
		lua_pop(L, 1);
	}
	else {
		lua_pop(L, 1);
		lua_pushstring(L, "y");
		lua_rawget(L, idx);
		if(lua_isnumber(L, -1))
		{
			v->y = (float)lua_tonumber(L, -1);
			lua_pop(L, 1);
		}
		else {
			lua_pop(L, 1);
			return FALSE;
		}
	}

	return TRUE;
}

int GetKeyFromLuaArg(lua_State* L, int nd)
{
	int key = -1;
	if(lua_isnumber(L, 1)) 
	{
		key = (int)lua_tointeger(L, 1);
	}
	else if(lua_isstring(L, 1))
	{
		const char* s = lua_tostring(L, 1);
		if(strlen(s) > 0) {
			key = s[0];
		}
	}
	return key;
}