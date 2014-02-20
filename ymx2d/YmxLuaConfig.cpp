#include "stdafx.h"
#include "ymx2d.h"
#include "ymxlua.h"

extern lua_State* g_LuaState;

static const char* DEFAULT_CONF_FILENAME = "lua/conf.lua";

YmxLuaConfig::YmxLuaConfig()
{

}

YmxLuaConfig::~YmxLuaConfig()
{

}

bool YmxLuaConfig::_Initialize()
{
	return LoadLuaFile(DEFAULT_CONF_FILENAME);
}

bool YmxLuaConfig::LoadLuaFile(const char* filename)
{
	if(luaL_loadfile(g_LuaState, filename) || lua_pcall(g_LuaState, 0, 0, 0))
	{
		return false;
	}
	return true;
}

bool YmxLuaConfig::GetFloat(char* key, float* value)
{
	if(floatMap.count(key) == 0)
	{
		if(strchr(key, '.') == NULL)
		{
			lua_getglobal(g_LuaState, key);
		}
		else if(!_GetValueFromTable(key))
		{
			lua_settop(g_LuaState, 0);
			return false;
		}

		if(!lua_isnumber(g_LuaState, -1))
		{
			lua_settop(g_LuaState, 0);
			return false;
		}
		*value = lua_tonumber(g_LuaState, -1);
		lua_settop(g_LuaState, 0);

		floatMap[key] = *value;
		return true;
	}
	*value = floatMap.at(key);
	return true;
}

bool YmxLuaConfig::_GetValueFromTable(char* key)
{
	char* p = strchr(key, '.');
	char str[100];
	memset(str, 0, 100);
	if(p != NULL)
	{
		int tabNameStrSize = p - key;
		strncpy(str, key, tabNameStrSize);
		lua_getglobal(g_LuaState, str);
		if(!lua_istable(g_LuaState, -1))
		{
			lua_pop(g_LuaState, 1);
			return false;
		}

		p++;
		char* p2 = strchr(p, '.');
		while(p2 != NULL)
		{
			tabNameStrSize = p2 - p;
			lua_pushlstring(g_LuaState, p, tabNameStrSize);
			lua_gettable(g_LuaState, -2);
			if(!lua_istable(g_LuaState, -1))
			{
				lua_pop(g_LuaState, 2);
				return false;
			}
			lua_replace(g_LuaState, -2);
			p = p2 + 1;
			p2 = strchr(p, '.');
		}

		if(p == NULL)
		{
			lua_pop(g_LuaState, 1);
			return false;
		}

		lua_pushstring(g_LuaState, p);
		lua_gettable(g_LuaState, -2);
		lua_replace(g_LuaState, -2);
		return true;
	}
	return false;
}

bool YmxLuaConfig::GetBool(char* key, bool* value)
{
	if(boolMap.count(key) == 0)
	{
		if(strchr(key, '.') == NULL)
		{
			lua_getglobal(g_LuaState, key);
		}
		else if(!_GetValueFromTable(key))
		{
			lua_settop(g_LuaState, 0);
			return false;
		}

		if(!lua_isboolean(g_LuaState, -1))
		{
			lua_settop(g_LuaState, 0);
			return false;
		}
		*value = (bool)lua_toboolean(g_LuaState, -1);
		lua_settop(g_LuaState, 0);

		boolMap[key] = *value;
		return true;
	}
	*value = boolMap.at(key);
	return true;
}

bool YmxLuaConfig::GetInt(char* key, int* value)
{
	if(intMap.count(key) == 0)
	{
		if(strchr(key, '.') == NULL)
		{
			lua_getglobal(g_LuaState, key);
		}
		else if(!_GetValueFromTable(key))
		{
			lua_settop(g_LuaState, 0);
			return false;
		}

		if(!lua_isnumber(g_LuaState, -1))
		{
			lua_settop(g_LuaState, 0);
			return false;
		}
		*value = (int)lua_tointeger(g_LuaState, -1);
		lua_settop(g_LuaState, 0);

		intMap[key] = *value;
		return true;
	}
	*value = intMap.at(key);
	return true;
}

bool YmxLuaConfig::GetString(char* key, TCHAR* buf, int bufSize, size_t* len)
{
	if(strchr(key, '.') == NULL)
	{
		lua_getglobal(g_LuaState, key);
	}
	else if(!_GetValueFromTable(key))
	{
		lua_settop(g_LuaState, 0);
		return false;
	}

	if(!lua_isstring(g_LuaState, -1))
	{
		lua_settop(g_LuaState, 0);
		return false;
	}
	const char* s = lua_tostring(g_LuaState, -1);
	if(len != NULL)
		*len = strlen(s);

	if(buf != NULL && bufSize > 0) {
		memset(buf, 0, bufSize * sizeof(TCHAR));
#ifdef _UNICODE
		MultiByteToWideChar(CP_ACP, 0, s, -1, buf, bufSize);
#else
		strncpy_s(buf, bufSize, s, _TRUNCATE);
#endif
	}

	lua_settop(g_LuaState, 0);
	return true;
}

bool YmxLuaConfig::GetStringA(char* key, char* buf, int bufSize, size_t* len)
{
	if(strchr(key, '.') == NULL)
	{
		lua_getglobal(g_LuaState, key);
	}
	else if(!_GetValueFromTable(key))
	{
		lua_settop(g_LuaState, 0);
		return false;
	}

	if(!lua_isstring(g_LuaState, -1))
	{
		lua_settop(g_LuaState, 0);
		return false;
	}
	const char* s = lua_tostring(g_LuaState, -1);
	if(len != NULL)
		*len = strlen(s);

	if(buf != NULL && bufSize > 0) {
		memset(buf, 0, bufSize * sizeof(char));
		strncpy_s(buf, bufSize, s, _TRUNCATE);
	}

	lua_settop(g_LuaState, 0);
	return true;

}

bool YmxLuaConfig::GetStringW(char* key, wchar_t* buf, int bufSize, size_t* len)
{
	if(strchr(key, '.') == NULL)
	{
		lua_getglobal(g_LuaState, key);
	}
	else if(!_GetValueFromTable(key))
	{
		lua_settop(g_LuaState, 0);
		return false;
	}

	if(!lua_isstring(g_LuaState, -1))
	{
		lua_settop(g_LuaState, 0);
		return false;
	}
	const char* s = lua_tostring(g_LuaState, -1);
	if(len != NULL)
		*len = strlen(s);

	if(buf != NULL && bufSize > 0) {
		memset(buf, 0, bufSize * sizeof(TCHAR));
		MultiByteToWideChar(CP_ACP, 0, s, -1, buf, bufSize);
	}

	lua_settop(g_LuaState, 0);
	return true;
}

bool YmxLuaConfig::GetIntArray(char* key, int* buf, size_t* len)
{
	if(strchr(key, '.') == NULL)
	{
		lua_getglobal(g_LuaState, key);
	}
	else if(!_GetValueFromTable(key))
	{
		lua_settop(g_LuaState, 0);
		return false;
	}

	if(!lua_istable(g_LuaState, -1))
	{
		lua_settop(g_LuaState, 0);
		return false;
	}

	int n = lua_objlen(g_LuaState, -1);
	if(len != NULL)
		*len = n;

	if(buf == NULL) {
		lua_settop(g_LuaState, 0);
		return true;
	}

	int i;
	for (i = 1; i <= n; i++)
	{
		lua_rawgeti(g_LuaState, -1, i);
		if(!lua_isnumber(g_LuaState, -1)) {
			lua_settop(g_LuaState, 0);
			return false;
		}

		buf[i-1] = (int)lua_tointeger(g_LuaState, -1);
		lua_pop(g_LuaState, 1);
	}

	lua_settop(g_LuaState, 0);
	return true;
}

bool YmxLuaConfig::GetFloatArray(char* key, float* buf, size_t* len)
{
	if(strchr(key, '.') == NULL)
	{
		lua_getglobal(g_LuaState, key);
	}
	else if(!_GetValueFromTable(key))
	{
		lua_settop(g_LuaState, 0);
		return false;
	}

	if(!lua_istable(g_LuaState, -1))
	{
		lua_settop(g_LuaState, 0);
		return false;
	}

	int n = lua_objlen(g_LuaState, -1);
	if(len != NULL)
		*len = n;

	if(buf == NULL) {
		lua_settop(g_LuaState, 0);
		return true;
	}

	int i;
	for (i = 1; i <= n; i++)
	{
		lua_rawgeti(g_LuaState, -1, i);
		if(!lua_isnumber(g_LuaState, -1)) {
			lua_settop(g_LuaState, 0);
			return false;
		}

		buf[i-1] = (float)lua_tonumber(g_LuaState, -1);
		lua_pop(g_LuaState, 1);
	}

	lua_settop(g_LuaState, 0);
	return true;
}

bool YmxLuaConfig::GetBoolArray(char* key, bool* buf, size_t* len)
{
	if(strchr(key, '.') == NULL)
	{
		lua_getglobal(g_LuaState, key);
	}
	else if(!_GetValueFromTable(key))
	{
		lua_settop(g_LuaState, 0);
		return false;
	}

	if(!lua_istable(g_LuaState, -1))
	{
		lua_settop(g_LuaState, 0);
		return false;
	}

	int n = lua_objlen(g_LuaState, -1);
	if(len != NULL)
		*len = n;

	if(buf == NULL) {
		lua_settop(g_LuaState, 0);
		return true;
	}

	int i;
	for (i = 1; i <= n; i++)
	{
		lua_rawgeti(g_LuaState, -1, i);
		if(!lua_toboolean(g_LuaState, -1)) {
			buf[i - 1] = true;
		}
		else {
			buf[i - 1] = false;
		}
		lua_pop(g_LuaState, 1);
	}

	lua_settop(g_LuaState, 0);
	return true;
}