#include "stdafx.h"
#include "ymxluautil.h"
#include "ymxluacore.h"
#include "ymxluaform.h"

const char* BUTTON_MT_NAME = "ymxlua.button"; 
const struct luaL_Reg buttonLib_m[] = {
	{"setTexture", btn_SetTexture},
	{"setTextureRect", btn_SetTextureRect},
	{"update", btn_Update},
	{"render", btn_Render},
	{"setZ", btn_SetZ},
	{"getZ", btn_GetZ},
	{"setFont", btn_SetFont},
	{"setText", btn_SetText},
	{"setTextColor", btn_SetTextColor},
	{"setPosition", btn_SetPosition},
	{"getWidth", btn_GetWidth},
	{"getHeight", btn_GetHeight},
	{"setEnabled", btn_SetEnabled},
	{"isEnabled", btn_IsEnabled},
	{"setVisible", btn_SetVisible},
	{"isVisible", btn_IsVisible},
	{0, 0}
};

struct ButtonStateEnumType
{
	const char* name;
	YMXBUTTON_STATE value;
};

static const ButtonStateEnumType stateNameTypes[] = {
	{"NORMAL", YMXBUTTON_NORMAL},
	{"PRESSED", YMXBUTTON_PRESSED},
	{"MOUSEOVER", YMXBUTTON_MOUSEOVER},
	{"DISABLED", YMXBUTTON_DISABLED},
	{0, YMXBUTTON_NORMAL}
};

int btn_Create(lua_State* L)
{
	if(!lua_isnumber(L, 1))
	{
		lua_pushnil(L);
		return 1;
	}

	int id = (int)lua_tointeger(L, 1);

	LPYMXCOMPONENT pComponent = GetComponentFromLuaArg(L, 2);
	if(pComponent == NULL)
	{
		lua_pushnil(L);
		return 1;
	}

	LPYMXTEXTURE pTexture = NULL;
	LPYMXFONT pFont = NULL;

	if(lua_isuserdata(L, 3)) {
		LuaProxy* pTexProxy = (LuaProxy*)luaL_checkudata(L, 3, TEXTURE_MT_NAME);
		if(pTexProxy != NULL) 
			pTexture = (LPYMXTEXTURE)pTexProxy->instance;
	}

	if(lua_isuserdata(L, 4)) {
		LuaProxy* pFontProxy = (LuaProxy*)luaL_checkudata(L, 4, FONT_MT_NAME);
		if(pFontProxy != NULL)
			pFont = (LPYMXFONT)pFontProxy->instance;
	}

	if(!lua_isnumber(L, 5) || !lua_isnumber(L, 6))
	{
		lua_pushnil(L);
		return 1;
	}

	float x = (float)lua_tonumber(L, 5);
	float y = (float)lua_tonumber(L, 6);

	YmxButton* pButton = pComponent->CreateButton(id, pTexture, pFont, x, y);

	if(pButton == NULL) {
		lua_pushnil(L);
		return 1;
	}

	LuaProxy* proxy = (LuaProxy*)lua_newuserdata(L, sizeof(LuaProxy));
	proxy->instance = pButton;
	proxy->type = BUTTON_PROXY;

	luaL_getmetatable(L, BUTTON_MT_NAME);
	lua_setmetatable(L, -2);

	return 1;
}

int btn_SetTexture(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, BUTTON_MT_NAME);
	YmxButton* button = (YmxButton*)proxy->instance;

	if(button == NULL) 
		return 0;

	LPYMXTEXTURE pTexture = NULL;
	if(lua_isuserdata(L, 2)) {
		LuaProxy* pTexProxy = (LuaProxy*)luaL_checkudata(L, 2, TEXTURE_MT_NAME);
		pTexture = (LPYMXTEXTURE)pTexProxy->instance;
	}

	button->SetTexture(pTexture);
	return 0;
}


int btn_SetTextureRect(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, BUTTON_MT_NAME);
	YmxButton* button = (YmxButton*)proxy->instance;

	if(button == NULL)
		return 0;

	if(lua_istable(L, 2))
	{
		for (const ButtonStateEnumType* snt = stateNameTypes; snt->name != 0; snt++)
		{
			lua_pushstring(L, snt->name);
			lua_rawget(L, 2);
			if(lua_istable(L, -1))
			{
				YmxRect rect;
				if(GetRectFromLuaArg(L, lua_gettop(L), &rect))
				{
					button->SetTextureRect(snt->value, rect);
				}
			}
			lua_pop(L, 1);
		}
	}
	else if(lua_isstring(L, 2) && lua_istable(L, 3))
	{
		const char* state = lua_tostring(L, 2);
		for (const ButtonStateEnumType* snt = stateNameTypes; snt->name != 0; snt++)
		{
			if(stricmp(state, snt->name) == 0)
			{
				YmxRect rect;
				if (GetRectFromLuaArg(L, 3, &rect))
				{
					button->SetTextureRect(snt->value, rect);
				}
				break;
			}
		}
	}

	return 0;
}

int btn_Update(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, BUTTON_MT_NAME);
	YmxButton* button = (YmxButton*)proxy->instance;

	if(button == NULL)
		return 0;

	float delta;
	if(lua_isnumber(L, 2)) 
		delta = (float)lua_tonumber(L, 2);
	else 
		delta = GetGameObject()->GetUpdateDelta();

	button->Update(delta);
	return 0;
}

int btn_Render(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, BUTTON_MT_NAME);
	YmxButton* button = (YmxButton*)proxy->instance;

	if(button == NULL)
		return 0;

	YmxGraphics* g = (YmxGraphics*)lua_touserdata(L, 2);
	if(g == NULL)
		return 0;

	button->Render(g);
	return 0;
}

int btn_SetFont(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, BUTTON_MT_NAME);
	YmxButton* button = (YmxButton*)proxy->instance;

	if(button == NULL) 
		return 0;

	LPYMXFONT pFont = NULL;
	if(lua_isuserdata(L, 2)) {
		LuaProxy* pFontProxy = (LuaProxy*)luaL_checkudata(L, 2, FONT_MT_NAME);
		pFont = (LPYMXFONT)pFontProxy->instance;
	}

	button->SetFont(pFont);
	return 0;
}

int btn_SetText(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, BUTTON_MT_NAME);
	YmxButton* button = (YmxButton*)proxy->instance;

	if(button == NULL || !lua_isstring(L, 2)) 
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	const char* s = lua_tostring(L, 2);

#ifdef _UNICODE
	TCHAR* buf = (TCHAR*)malloc((strlen(s) + 1) * sizeof(TCHAR));
	ConvertAsciiToPTStrs(s, buf, strlen(s) + 1);
	button->SetText(buf);
	free(buf);
#else
	button->SetText(s);
#endif

	lua_pushboolean(L, 1);
	return 1;
}

int btn_SetTextColor(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, BUTTON_MT_NAME);
	YmxButton* button = (YmxButton*)proxy->instance;

	if(button == NULL || !lua_isstring(L, 2)) 
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	YMXCOLOR color;
	if(GetColorFromLuaArg(L, 3, &color))
	{
		button->SetTextColor(color);
		lua_pushboolean(L, 1);
	}
	else {
		lua_pushboolean(L, 0);
	}
	return 1;
}

int btn_SetZ(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, BUTTON_MT_NAME);
	YmxButton* button = (YmxButton*)proxy->instance;

	if(button == NULL) {
		return 0;
	}

	if(!lua_isnumber(L, 2))
	{
		return 0;
	}

	float z = (float)lua_tonumber(L, 2);
	button->SetZ(z);
	return 0;
}

int btn_GetZ(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, BUTTON_MT_NAME);
	YmxButton* button = (YmxButton*)proxy->instance;

	if(button == NULL) {
		lua_pushboolean(L, 0);
		return 1;
	}

	float z = button->GetZ();

	lua_pushnumber(L, z);
	return 1;
}

int btn_SetPosition(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, BUTTON_MT_NAME);
	YmxButton* button = (YmxButton*)proxy->instance;

	if(button == NULL) {
		lua_pushboolean(L, 0);
		return 1;
	}

	if(lua_istable(L, 2))
	{
		YmxVertex v;
		if(GetVertexFromLuaArg(L, 2, &v))
		{
			button->SetPosition(v.x, v.y);
			lua_pushboolean(L, 1);
			return 1;
		}
	}
	else if(lua_isnumber(L, 2) && lua_isnumber(L, 3))
	{
		float x = (float)lua_tonumber(L, 2);
		float y = (float)lua_tonumber(L, 3);
		button->SetPosition(x, y);
		lua_pushboolean(L, 1);
		return 1;
	}

	lua_pushboolean(L, 0);
	return 1;
}

int btn_GetWidth(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, BUTTON_MT_NAME);
	YmxButton* button = (YmxButton*)proxy->instance;

	if(button == NULL) {
		lua_pushboolean(L, 0);
		return 1;
	}

	float w = button->GetWidth();
	lua_pushnumber(L, w);
	return 1;
}

int btn_GetHeight(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, BUTTON_MT_NAME);
	YmxButton* button = (YmxButton*)proxy->instance;

	if(button == NULL) {
		lua_pushboolean(L, 0);
		return 1;
	}

	float h = button->GetHeight();
	lua_pushnumber(L, h);
	return 1;
}

int btn_SetEnabled(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, BUTTON_MT_NAME);
	YmxButton* button = (YmxButton*)proxy->instance;

	if(button == NULL) {
		lua_pushboolean(L, 0);
		return 1;
	}

	if(lua_toboolean(L, 2))
		button->SetEnabled(TRUE);
	else 
		button->SetEnabled(FALSE);

	lua_pushboolean(L, 1);
	return 1;
}

int btn_IsEnabled(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, BUTTON_MT_NAME);
	YmxButton* button = (YmxButton*)proxy->instance;

	if(button == NULL) {
		lua_pushnil(L);
		return 1;
	}

	lua_pushboolean(L, button->IsEnabled());
	return 1;
}

int btn_SetVisible(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, BUTTON_MT_NAME);
	YmxButton* button = (YmxButton*)proxy->instance;

	if(button == NULL) {
		lua_pushboolean(L, 0);
		return 1;
	}

	if(lua_toboolean(L, 2))
		button->SetVisible(TRUE);
	else 
		button->SetVisible(FALSE);

	lua_pushboolean(L, 1);
	return 1;
}

int btn_IsVisible(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, BUTTON_MT_NAME);
	YmxButton* button = (YmxButton*)proxy->instance;

	if(button == NULL) {
		lua_pushnil(L);
		return 1;
	}

	lua_pushboolean(L, button->IsVisible());
	return 1;
}
