#include "stdafx.h"
#include "ymx2d.h"
#include "ymxlua.h"

extern lua_State* g_LuaState;

YmxLuaComponent::YmxLuaComponent(int id, LPYMXCOMPONENT pParentComponent, char* name, UINT order)
	:YmxComponent(id, pParentComponent, name, order)
{
	m_CompoName = (char*)malloc(strlen(name) + 10);
	strcpy(m_CompoName, name);
	strcat(m_CompoName, "Component");

	this->m_bValid = true;

	/*
	if(_ExistFunc("Update")) 
		m_bExistUpdateFunc = true;
	else
		m_bExistUpdateFunc = false;

	if(_ExistFunc("Render")) 
		m_bExistRenderFunc = true;
	else 
		m_bExistRenderFunc = false;

	if(_ExistFunc("OnMessage"))
		m_bExistOnMessageFunc = true;
	else 
		m_bExistOnMessageFunc = false;

	// Set Z Attribute if the lua script have
	lua_getglobal(g_LuaState, m_CompoName);
	
	//lua_pop(g_LuaState, 2);
	lua_settop(g_LuaState, 0);
	*/
}

void YmxLuaComponent::LoadContent()
{
	if(m_bValid)
	{
		lua_getglobal(g_LuaState, m_CompoName);
		lua_pushstring(g_LuaState, "LoadContent");
		lua_rawget(g_LuaState, -2);
		lua_getglobal(g_LuaState, m_CompoName);

		if(lua_isfunction(g_LuaState, -2))
		{
			if(lua_pcall(g_LuaState, 1, 0, 0))
			{
				RecordErrorA("Lua Script Error", "Cannot translate %s's LoadContent function", m_CompoName);
				m_bValid = false;
			}
		}
		lua_pop(g_LuaState, 1);
	}
}

void YmxLuaComponent::Initialize()
{
	if(m_bValid)
	{
		lua_getglobal(g_LuaState, m_CompoName);
		lua_pushstring(g_LuaState, "Initialize");
		lua_rawget(g_LuaState, -2);
		lua_getglobal(g_LuaState, m_CompoName);

		if(lua_isfunction(g_LuaState, -2))
		{
			if(lua_pcall(g_LuaState, 1, 0, 0))
			{
				RecordErrorA("Lua Script Error", "Cannot translate %s's Initialize function", m_CompoName);
				m_bValid = false;
			}
		}
		lua_pop(g_LuaState, 1);
	}
}

void YmxLuaComponent::Render(YmxGraphics* g)
{
	if(m_bValid && m_bExistRenderFunc)
	{
		lua_getglobal(g_LuaState, m_CompoName);
		lua_pushstring(g_LuaState, "Render");
		lua_rawget(g_LuaState, -2);
		lua_getglobal(g_LuaState, m_CompoName);
		lua_pushlightuserdata(g_LuaState, (void*)g);

		if(lua_pcall(g_LuaState, 2, 0, 0))
		{
			RecordErrorA("Lua Script Error", "Cannot translate %s's Render function", m_CompoName);
			m_bValid = false;
		}
		lua_settop(g_LuaState, 0);
	}
}

bool YmxLuaComponent::Update(float delta)
{
	bool result = true;
	if(m_bValid && m_bExistUpdateFunc)
	{
		lua_getglobal(g_LuaState, m_CompoName);
		lua_pushstring(g_LuaState, "Update");
		lua_rawget(g_LuaState, -2);
		lua_getglobal(g_LuaState, m_CompoName);
		lua_pushnumber(g_LuaState, delta);

		if(lua_pcall(g_LuaState, 2, 1, 0))
		{
			RecordErrorA("Lua Script Error", "Cannot translate %s's Update function", m_CompoName);
			m_bValid = false;
		}

		if(lua_isboolean(g_LuaState, -1) && !lua_toboolean(g_LuaState, -1))
			result = false;
		
		lua_settop(g_LuaState, 0);
	}
	return result;
}

void YmxLuaComponent::UnloadContent()
{
	if(m_bValid)
	{
		lua_getglobal(g_LuaState, m_CompoName);
		lua_pushstring(g_LuaState, "UnloadContent");
		lua_rawget(g_LuaState, -2);
		lua_getglobal(g_LuaState, m_CompoName);

		if(lua_isfunction(g_LuaState, -2))
		{
			if(lua_pcall(g_LuaState, 1, 0, 0))
			{
				RecordErrorA("Lua Script Error", "Cannot translate %s's UnloadContent function", m_CompoName);
				m_bValid = false;
			}
		}
		lua_settop(g_LuaState, 0);
	}
}

YmxLuaComponent::~YmxLuaComponent()
{
	free(m_CompoName);
}

bool YmxLuaComponent::_ExistFunc(const char* funcName)
{
	lua_getglobal(g_LuaState, m_CompoName);
	lua_pushstring(g_LuaState, funcName);
	lua_rawget(g_LuaState, -2);

	if(lua_isfunction(g_LuaState, -1))
	{
		lua_settop(g_LuaState, 0);
		return true;
	}

	lua_settop(g_LuaState, 0);
	return false;
}

bool YmxLuaComponent::OnMessage(int msgid, DWORD notifyCode, PVOID param)
{
	bool result = false;
	int argNum = 3;
	if(m_bValid && m_bExistOnMessageFunc)
	{
		//save the context of g_LuaState now
		int top = lua_gettop(g_LuaState);
		lua_getglobal(g_LuaState, m_CompoName);
		lua_pushstring(g_LuaState, "OnMessage");
		lua_rawget(g_LuaState, -2);
		lua_getglobal(g_LuaState, m_CompoName);

		if(!lua_isfunction(g_LuaState, -2))
		{
			lua_settop(g_LuaState, top);
			return false;
		}

		lua_pushinteger(g_LuaState, msgid);
		lua_pushinteger(g_LuaState, notifyCode);
		if(lua_istable(g_LuaState, 4)) {
			lua_pushvalue(g_LuaState, 4);
			argNum = 4;
		}
		else if(msgid == GCM_LIST_CLICK || msgid == GCM_LIST_MOUSEOUT || msgid == GCM_LIST_MOUSEOVER)
		{
			// if the listView event, should set the params, set params['index'] = item_index
			lua_newtable(g_LuaState);
			lua_pushinteger(g_LuaState, (int)param);
			lua_setfield(g_LuaState, -2, "index");
			argNum = 4;
		}

		if(lua_pcall(g_LuaState, argNum, 1, 0))
		{
			RecordErrorA("Lua Script Error", "Cannot translate %s's OnMessage function", m_CompoName);
			m_bValid = false;
		}

		if(lua_toboolean(g_LuaState, -1)) result = true;
		
		//recover the context of lua_State
		lua_settop(g_LuaState, top);

	}
	return result;
}

