#include "stdafx.h"
#include "ymxluautil.h"
#include "ymxluacore.h"


int input_GetMousePos(lua_State* L)
{
	LPYMXCOMPONENT pComponent = GetComponentFromLuaArg(L, 1);
	float x, y;
	if(pComponent == NULL) 
		GetGameObject()->GetMousePos(&x, &y);
	else 
		pComponent->GetMousePos(&x, &y);

	lua_pushnumber(L, x);
	lua_pushnumber(L, y);
	return 2;
}

int input_SetMousePos(lua_State* L)
{
	float x = (float)luaL_checknumber(L, 1);
	float y = (float)luaL_checknumber(L, 2);
	LPYMXCOMPONENT pComponent = GetComponentFromLuaArg(L, 3);

	if(pComponent == NULL)
		GetGameObject()->SetMousePos(x, y);
	else 
		pComponent->SetMousePos(x, y);
	return 0;
}

int input_KeyDown(lua_State* L)
{
	int key = GetKeyFromLuaArg(L, 1);

	if(key >= 0 && GetGameObject()->KeyDown(key))
		lua_pushboolean(L, 1);
	else 
		lua_pushboolean(L, 0);
	return 1;
}

int input_KeyUp(lua_State* L)
{
	int key = GetKeyFromLuaArg(L, 1);

	if(key >= 0 && GetGameObject()->KeyUp(key))
		lua_pushboolean(L, 1);
	else 
		lua_pushboolean(L, 0);
	return 1;
}

int input_GetKeyState(lua_State* L)
{
	int key = GetKeyFromLuaArg(L, 1);

	if(key >= 0 && GetGameObject()->GetKeyState(key))
		lua_pushboolean(L, 1);
	else 
		lua_pushboolean(L, 0);

	return 1;
}

int input_GetInputKey(lua_State* L)
{
	int key = GetGameObject()->GetInputKey();
	lua_pushinteger(L, key);
	return 1;
}

//TODO:
int input_GetChar(lua_State* L)
{
	TCHAR c = GetGameObject()->GetInputChar();
	TCHAR str[2];
	str[0] = c;
	str[1] = '\0';

#ifdef _UNICODE
	char buf[4];

#else

#endif
	return 1;
}

int comp_SendMessage(lua_State* L)
{
	BOOL result;
	int msgId = (UINT)luaL_checkinteger(L, 1);
	LPYMXCOMPONENT pComponent = GetComponentFromLuaArg(L, 2);

	if(pComponent == NULL) 
		return 0;

	int notifyCode = 0;
	if(lua_isnumber(L, 3))
	{
		notifyCode = (int)lua_tointeger(L, 3);
	}

	result = YmxComponent::SendMessage(msgId, pComponent, notifyCode, NULL);
	lua_pushboolean(L, result);
	return 1;
}

int comp_BroadcastMessage(lua_State* L)
{
	BOOL result;
	int msgId = (UINT)luaL_checkinteger(L, 1);
	LPYMXCOMPONENT pComponent = GetComponentFromLuaArg(L, 2);

	if(pComponent == NULL) 
		return 0;

	int notifyCode = 0;
	if(lua_isnumber(L, 3))
	{
		notifyCode = (int)lua_tointeger(L, 3);
	}

	int flag = GMF_CAPTURE;

	// if the fifth arg is true, use GMF_BUBBLE, if false, use GMF_CAPTURE
	if(lua_gettop(L) > 4)
	{
		if(lua_toboolean(L, 5))
			flag |= GMF_BUBBLE;

		// if the sixth argument is true, use GMF_THROWABLE
		if(lua_gettop(L) > 5 && lua_toboolean(L, 6))
			flag |= GMF_THROWABLE;
	}

	result = YmxComponent::BroadcastMessage(msgId, pComponent, notifyCode, NULL, flag);
	lua_pushboolean(L, result);
	return 1;
}

int comp_AddComponent(lua_State* L)
{
	if(!lua_isstring(L, 1))
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	const char* name = lua_tostring(L, 1);

	if(GetGameObject()->AddLuaComponent(name, TRUE))
	{
		lua_pushboolean(L, 1);
	}
	else {
		lua_pushboolean(L, 0);
	}

	return 1;
}

int comp_RemoveComponent(lua_State* L)
{
	if(!lua_isstring(L, 1))
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	const char* name = lua_tostring(L, 1);
	if(GetGameObject()->RemoveComponent(name, TRUE))
	{
		lua_pushboolean(L, 1);
	}
	else {
		lua_pushboolean(L, 0);
	}
	return 1;
}

int err_MessageBox(lua_State* L)
{
	if(lua_gettop(L) == 1) {
		if(!lua_isstring(L, 1)) {
			return 0;
		}

		const char* s = lua_tostring(L, 1);
		MessageBoxA(NULL, s, NULL, 0);
		return 0;
	}
	else if(lua_gettop(L) > 1) {
		if(!lua_isstring(L, 1) || !lua_isstring(L, 2))
			return 0;

		const char* caption = lua_tostring(L, 1);
		const char* s = lua_tostring(L, 2);
		MessageBoxA(NULL, s, caption, 0);
		return 0;
	}
	return 0;
}

int err_RecordError(lua_State* L)
{
	if(lua_gettop(L) == 1) {
		if(!lua_isstring(L, 1)) {
			return 0;
		}

		const char* s = lua_tostring(L, 1);
		RecordErrorA(NULL, s, NULL, 0);
		return 0;
	}
	else if(lua_gettop(L) > 1) {
		if(!lua_isstring(L, 1) || !lua_isstring(L, 2))
			return 0;

		const char* caption = lua_tostring(L, 1);
		const char* s = lua_tostring(L, 2);
		RecordErrorA(NULL, s, caption, 0);
		return 0;
	}
	return 0;
}

int err_Log(lua_State* L)
{
	if(!lua_isstring(L, 1))
		return 0;

	const char* s = lua_tostring(L, 1);
	GetGameObject()->LogA(s);
	return 0;
}

int win_GetWindowWidth(lua_State* L)
{
	float width = GetGameObject()->GetWindowWidth();
	lua_pushnumber(L, width);
	return 1;
}

int win_GetWindowHeight(lua_State* L)
{
	float height = GetGameObject()->GetWindowHeight();
	lua_pushnumber(L, height);
	return 1;
}

int rect_Intersect(lua_State* L)
{
	if(lua_gettop(L) < 2)
		return 0;

	YmxRect rect1, rect2;
	if(GetRectFromLuaArg(L, 1, &rect1) && GetRectFromLuaArg(L, 2, &rect2))
	{
		if(rect1.Intersect(rect2))
		{
			lua_pushboolean(L, 1);
		}
		else {
			lua_pushboolean(L, 0);
		}
		return 1;
	}
	return 0;
}

int comp_SetEnabled(lua_State* L)
{
	LPYMXCOMPONENT pComponent = GetComponentFromLuaArg(L, 1);
	if(pComponent == NULL)
		return 0;

	bool bEnabled = true;
	if(lua_gettop(L) > 1)
		bEnabled = (bool)lua_toboolean(L, 2);

	pComponent->SetEnabled(bEnabled);
	return 0;
}

int comp_SetVisible(lua_State* L)
{
	LPYMXCOMPONENT pComponent = GetComponentFromLuaArg(L, 1);
	if(pComponent == NULL)
		return 0;

	bool bVisible = true;
	if(lua_gettop(L) > 1)
		bVisible = (bool)lua_toboolean(L, 2);

	pComponent->SetVisible(bVisible);
	return 0;
}