#ifndef YMXLUA_UTIL_FUNCTION_CLASS_FORM_H
#define YMXLUA_UTIL_FUNCTION_CLASS_FORM_H

enum LUA_PROXY_TYPE
{
	TEXTURE_PROXY = 0,
	SPRITE_PROXY,
	FONT_PROXY,
	PARTICLE_PROXY,
	BUTTON_PROXY,
	LISTVIEW_PROXY,
	EDITTEXT_PROXY,
	SOUND_PROXY
};

struct LuaProxy
{ 
	void* instance; 
	LUA_PROXY_TYPE type;
};

void ConvertAsciiToPTStrs(const char* s, TCHAR buf[], size_t bufSize);
void ConvertPTStrsToAscii(PCTSTR s, char buf[], size_t bufSize);

void CreateMetaTable(lua_State* L, const char* name, const luaL_Reg m[]);
LPYMXCOMPONENT GetComponentByTblFromLuaArg(lua_State* L, int idx);
LPYMXCOMPONENT GetComponentByIdOrNameFromLuaArg(lua_State* L, int idx);
LPYMXCOMPONENT GetComponentFromLuaArg(lua_State* L, int idx);
BOOL GetRectFromLuaArg(lua_State* L, int idx, YmxRect* rect);
BOOL GetColorFromLuaArg(lua_State* L, int idx, YMXCOLOR* color);
BOOL GetVertexFromLuaArg(lua_State* L, int idx, YmxVertex* v);
int GetKeyFromLuaArg(lua_State* L, int idx);

#endif 