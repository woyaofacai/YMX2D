#include "stdafx.h"
#include "ymx2d.h"
#include "ymxlua.h"

lua_State* g_LuaState;

// Call when createGame
bool Initialize_LuaState()
{
	g_LuaState = luaL_newstate();
	if(g_LuaState == NULL)
		return false;

	luaL_openlibs(g_LuaState);

	//if the configuration lua file not exists, create it
	CreateDirectoryA("lua/", NULL);
	HANDLE hFile = CreateFileA("lua/conf.lua", GENERIC_READ, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	CloseHandle(hFile);

	lua_getglobal(g_LuaState, "package");
	lua_getfield(g_LuaState, -1, "path");
	//lua_pushvalue(g_LuaState, -1);
	lua_pushstring(g_LuaState, ";.\\lua\\?.lua;");
	lua_concat(g_LuaState, 2);
	lua_setfield(g_LuaState, -2, "path");
	lua_pop(g_LuaState, 1);
	return true;
}

void Release_LuaState()
{
	lua_close(g_LuaState);
}

bool GetColorFromLuaArg(lua_State* L, int idx, YMXCOLOR* color)
{
	if(!lua_istable(L, idx))
		return false;

	int tblLen = lua_objlen(L, idx);

	if(tblLen < 3) 
		return false;

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
	return true;
}


bool GetRectFromLuaArg(lua_State* L, int idx, YmxRect* rect)
{
	if(!lua_istable(L, idx)) return false;
	if(lua_objlen(L, idx) < 4) return false;

	lua_rawgeti(L, idx, 1);
	lua_rawgeti(L, idx, 2);
	lua_rawgeti(L, idx, 3);
	lua_rawgeti(L, idx, 4);

	rect->left = (float)lua_tonumber(L, -4);
	rect->top = (float)lua_tonumber(L, -3);
	rect->right = (float)lua_tonumber(L, -2);
	rect->bottom = (float)lua_tonumber(L, -1);

	lua_pop(L, 4);

	return true;
}


YmxLuaComponent* CreateLuaComponentFromFile(const char* fileName, bool bInit /*=false*/)
{ 
	int id, order;
	float z;

	char compoTblName[MAX_PATH];
	strncpy_s(compoTblName, MAX_PATH, fileName, strlen(fileName) - 4);
	char compoName[MAX_PATH];
	strncpy_s(compoName, MAX_PATH, fileName, strlen(fileName) - 13);
	char filePath[MAX_PATH];
	strcpy(filePath, "lua\\");
	strcat(filePath, fileName);

	lua_getglobal(g_LuaState, compoTblName);
	if(!lua_istable(g_LuaState, -1))
	{
		lua_pop(g_LuaState, 1);
		if(luaL_loadfile(g_LuaState, filePath) || lua_pcall(g_LuaState, 0, 0, 0))
		{
			RecordErrorA("Lua Script Error", "Cannot translate script file : %s", fileName);
			return NULL;
		}
		lua_getglobal(g_LuaState, compoTblName);
		if(!lua_istable(g_LuaState, -1))
		{
			lua_pop(g_LuaState, 1);
			return NULL;
		}
	}

	// Get id attribute
	lua_getfield(g_LuaState, -1, "id");
	if(!lua_isnumber(g_LuaState, -1))
	{
		RecordErrorA("Lua Script Error", "%s does not have an 'id' attribute", fileName);
		lua_pop(g_LuaState, 2);
		return NULL;
	}
	id = (int)lua_tointeger(g_LuaState, -1);
	lua_pop(g_LuaState, 1);

	// Get Order attribute
	lua_getfield(g_LuaState, -1, "order");
	order = 0;
	if(lua_isnumber(g_LuaState, -1))
	{
		order = (int)lua_tointeger(g_LuaState, -1);	
	}
	lua_pop(g_LuaState, 1);

	LPYMXCOMPONENT pParent = NULL;
	lua_getfield(g_LuaState, -1, "parent");
	if(lua_isstring(g_LuaState, -1))
	{
		const char* parentName = lua_tostring(g_LuaState, -1);
		pParent = GetGameObject()->GetComponentByName(parentName);

		//if parent is null, maybe it is not loaded, try to load it from lua file
		if(pParent == NULL)
		{
			char parentFileName[MAX_PATH];
			strcpy(parentFileName, parentName);
			strcat(parentFileName, "Component.lua");
			pParent = CreateLuaComponentFromFile(parentFileName, bInit);
		}
	}
	//pop the parent name
	lua_pop(g_LuaState, 1);

	YmxLuaComponent* pComponent = new YmxLuaComponent(id, pParent, compoName, order);

	if(pParent == NULL) {
		GetGameObject()->AddComponent(pComponent, bInit);
	}

	// Get Visible attribute
	lua_getfield(g_LuaState, -1, "visible");
	if(!lua_isnil(g_LuaState, -1) && !lua_toboolean(g_LuaState, -1))
	{
		pComponent->SetVisible(false);
	}
	lua_pop(g_LuaState, 1);

	// Get Enabled attribute
	lua_getfield(g_LuaState, -1, "enabled");
	if(!lua_isnil(g_LuaState, -1) && !lua_toboolean(g_LuaState, -1))
	{
		pComponent->SetEnabled(false);
	}
	lua_pop(g_LuaState, 1);

	// Get rect attribute
	lua_getfield(g_LuaState, -1, "rect");
	YMXRECT rect;
	if(GetRectFromLuaArg(g_LuaState, lua_gettop(g_LuaState), &rect))
	{
		pComponent->SetBoundingBox(rect);
	}
	lua_pop(g_LuaState, 1);

	//check if need clip
	lua_getfield(g_LuaState, -1, "clip");
	if(!lua_isnil(g_LuaState, -1) && lua_toboolean(g_LuaState, -1))
	{
		pComponent->SetAttribute(COMPONENT_REQUIRE_CLIP, true);
	}
	lua_pop(g_LuaState, 1);

	// Get z attribute
	lua_getfield(g_LuaState, -1, "z");
	if(lua_isnumber(g_LuaState, -1))
	{
		z = lua_tonumber(g_LuaState, -1);
		pComponent->SetZ(z);
	}
	lua_pop(g_LuaState, 1);

	lua_getfield(g_LuaState, -1, "color");
	YMXCOLOR backgroundColor;
	if(GetColorFromLuaArg(g_LuaState, lua_gettop(g_LuaState), &backgroundColor))
	{
		pComponent->SetAttribute(COMPONENT_BACKGROUND_TRANSPARENT, false);
		pComponent->SetBackgroundColor(backgroundColor);
	}
	lua_pop(g_LuaState, 1);

	lua_getfield(g_LuaState, -1, "Update");
	if(lua_isfunction(g_LuaState, -1))
		pComponent->m_bExistUpdateFunc = true;
	else 
		pComponent->m_bExistUpdateFunc = false;
	lua_pop(g_LuaState, 1);

	lua_getfield(g_LuaState, -1, "Render");
	if(lua_isfunction(g_LuaState, -1))
		pComponent->m_bExistRenderFunc = true;
	else 
		pComponent->m_bExistRenderFunc = false;
	lua_pop(g_LuaState, 1);

	lua_getfield(g_LuaState, -1, "OnMessage");
	if(lua_isfunction(g_LuaState, -1))
		pComponent->m_bExistOnMessageFunc = true;
	else 
		pComponent->m_bExistOnMessageFunc = false;
	lua_pop(g_LuaState, 1);

	// Pop the Component table
	lua_pop(g_LuaState, 1);
	return pComponent;
}

bool LoadAllLuaComponentsFromFile()
{
	WIN32_FIND_DATA ffd;
	HANDLE hFind = INVALID_HANDLE_VALUE;

	PCTSTR szDir = TEXT("lua\\*");

	hFind = FindFirstFile(szDir, &ffd);
	if (INVALID_HANDLE_VALUE == hFind) 
	{
		return false;
	} 

	do {
		PTSTR fileName = ffd.cFileName;
		char fileNameBuf[MAX_PATH];
#ifdef _UNICODE
		WideCharToMultiByte(CP_ACP, 0, fileName, -1, fileNameBuf, MAX_PATH, NULL, NULL);
#else
		strncpy_s(fileNameBuf, MAX_PATH, fileName, _TRUNCATE);
#endif

		char* p = strstr(fileNameBuf, "Component.lua");
		if(p != NULL && *(p + 13) == '\0') {

			char compoName[MAX_PATH];
			strncpy_s(compoName, MAX_PATH, fileNameBuf, strlen(fileNameBuf) - 13);

			// if the component is not loaded, load it now
			if(GetGameObject()->GetComponentByName(compoName) == NULL) {
				CreateLuaComponentFromFile(fileNameBuf);
			}
		}
	}
	while (FindNextFile(hFind, &ffd) != 0);
	FindClose(hFind);

	return true;
}
