#include "stdafx.h"
#include "ymxluautil.h"
#include "ymxluacore.h"

const char* SOUND_MT_NAME = "ymxlua.sound";
const struct luaL_Reg soundLib_m[] = {
	{"play", sound_Play},
	{"pause", sound_Pause},
	{"stop", sound_Stop},
	{"setFrequency", sound_SetFrequency},
	{"getFrequency", sound_GetFrequency},
	{"setPan", sound_SetPan},
	{"getPan", sound_GetPan},
	{"setVolume", sound_SetVolume},
	{"getVolume", sound_GetVolume},
	{"isPlaying", sound_IsPlaying},
	{"isLooping", sound_IsLooping},
	{"release", sound_Release},
	{0, 0}
};


int sound_Create(lua_State* L)
{
	const char* filename = luaL_checkstring(L, 1);
#ifdef _UNICODE
	TCHAR wfilename[MAX_PATH];
	ConvertAsciiToPTStrs(filename, wfilename, MAX_PATH);
	YmxSound* sound = GetGameObject()->CreateSound(wfilename);
#else
	YmxSound* sound = GetGameObject()->CreateSound(filename);
#endif
	if(sound == NULL)
	{
		lua_pushnil(L);
		return 1;
	}

	LuaProxy* soundProxy = (LuaProxy*)lua_newuserdata(L, sizeof(LuaProxy));
	soundProxy->instance = (void*)sound;
	soundProxy->type = SOUND_PROXY;

	luaL_getmetatable(L, SOUND_MT_NAME);
	lua_setmetatable(L, -2);

	return 1;
}

int sound_Play(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, SOUND_MT_NAME);
	YmxSound* sound = (YmxSound*)proxy->instance;

	BOOL looping = FALSE;
	if(lua_gettop(L) == 2 && lua_toboolean(L, 2))
	{
		looping = TRUE;
	}

	if(sound == NULL)
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	if(sound->Play(looping))
	{
		lua_pushboolean(L, 1);
	}
	else {
		lua_pushboolean(L, 0);
	}
	return 1;
}


int sound_Pause(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, SOUND_MT_NAME);
	YmxSound* sound = (YmxSound*)proxy->instance;

	if(sound == NULL)
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	if(sound->Pause())
	{
		lua_pushboolean(L, 1);
	}
	else {
		lua_pushboolean(L, 0);
	}
	return 1;
}


int sound_Stop(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, SOUND_MT_NAME);
	YmxSound* sound = (YmxSound*)proxy->instance;

	if(sound == NULL)
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	if(sound->Stop())
	{
		lua_pushboolean(L, 1);
	}
	else {
		lua_pushboolean(L, 0);
	}
	return 1;
}


int sound_SetFrequency(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, SOUND_MT_NAME);
	YmxSound* sound = (YmxSound*)proxy->instance;

	if(sound == NULL || !lua_isnumber(L, 2))
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	int freq = (int)lua_tointeger(L, 2);
	if(sound->SetFrequency(freq))
	{
		lua_pushboolean(L, 1);
	}
	else {
		lua_pushboolean(L, 0);
	}
	return 1;
}


int sound_SetPan(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, SOUND_MT_NAME);
	YmxSound* sound = (YmxSound*)proxy->instance;

	if(sound == NULL || !lua_isnumber(L, 2))
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	int pan = (int)lua_tointeger(L, 2);
	if(sound->SetPan(pan))
	{
		lua_pushboolean(L, 1);
	}
	else {
		lua_pushboolean(L, 0);
	}
	return 1;
}

int sound_SetVolume(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, SOUND_MT_NAME);
	YmxSound* sound = (YmxSound*)proxy->instance;

	if(sound == NULL || !lua_isnumber(L, 2))
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	int volume = (int)lua_tointeger(L, 2);
	if(sound->SetVolume(volume))
	{
		lua_pushboolean(L, 1);
	}
	else {
		lua_pushboolean(L, 0);
	}
	return 1;
}

int sound_GetFrequency(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, SOUND_MT_NAME);
	YmxSound* sound = (YmxSound*)proxy->instance;

	if(sound == NULL)
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	int freq;
	if(sound->GetFrequency(&freq))
	{
		lua_pushinteger(L, freq);
	}
	else {
		lua_pushboolean(L, 0);
	}
	return 1;
}

int sound_GetPan(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, SOUND_MT_NAME);
	YmxSound* sound = (YmxSound*)proxy->instance;

	if(sound == NULL)
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	int pan;
	if(sound->GetPan(&pan))
	{
		lua_pushinteger(L, pan);
	}
	else {
		lua_pushboolean(L, 0);
	}
	return 1;
}

int sound_GetVolume(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, SOUND_MT_NAME);
	YmxSound* sound = (YmxSound*)proxy->instance;

	if(sound == NULL)
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	int volume;
	if(sound->GetVolume(&volume))
	{
		lua_pushinteger(L, volume);
	}
	else {
		lua_pushboolean(L, 0);
	}
	return 1;
}

int sound_IsPlaying(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, SOUND_MT_NAME);
	YmxSound* sound = (YmxSound*)proxy->instance;

	if(sound == NULL)
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	if(sound->IsPlaying())
	{
		lua_pushboolean(L, 1);	
	}
	else {
		lua_pushboolean(L, 0);
	}
	return 1;
}

int sound_IsLooping(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, SOUND_MT_NAME);
	YmxSound* sound = (YmxSound*)proxy->instance;

	if(sound == NULL)
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	if(sound->IsLooping())
	{
		lua_pushboolean(L, 1);	
	}
	else {
		lua_pushboolean(L, 0);
	}
	return 1;
}

int sound_Release(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, SOUND_MT_NAME);
	YmxSound* sound = (YmxSound*)proxy->instance;

	if(sound == NULL)
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	sound->Release();
	proxy->instance = NULL;
	return 0;
}
