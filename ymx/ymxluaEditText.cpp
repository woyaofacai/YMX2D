#include "stdafx.h"
#include "ymxluautil.h"
#include "ymxluacore.h"
#include "ymxluaform.h"

const char* EDITTEXT_MT_NAME = "ymxlua.edittext"; 
const struct luaL_Reg editTextLib_m[] = {
	{"getZ", edi_GetZ},
	{"setZ", edi_SetZ},
	{"setFont", edi_SetFont},
	{"setTextColor", edi_SetTextColor},
	{"update", edi_Update},
	{"render", edi_Render},
	{"setPosition", edi_SetPosition},
	{"setTextAlignment", edi_SetTextAlignment},
	{"setVisible", edi_SetVisible},
	{"setFocus", edi_SetFocus},
	{"isVisible", edi_IsVisible},
	{"isFocused", edi_IsFocused},
	{"release", edi_Release},
	{"getText", edi_GetText},
	{"setText", edi_SetText},
	{"setCursor", edi_SetCursor},
	{"setConfirmKey", edi_SetConfirmKey},
	{0, 0}
};

int edi_Create(lua_State* L)
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

	LPYMXFONT pFont = NULL;
	if(lua_isuserdata(L, 3)) {
		LuaProxy* pFontProxy = (LuaProxy*)luaL_checkudata(L, 3, FONT_MT_NAME);
		if(pFontProxy != NULL)
			pFont = (LPYMXFONT)pFontProxy->instance;
	}

	if(!lua_isnumber(L, 4) || !lua_isnumber(L, 5))
	{
		lua_pushnil(L);
		return 1;
	}

	float x = (float)lua_tonumber(L, 4);
	float y = (float)lua_tonumber(L, 5);

	YmxEditText* editText = pComponent->CreateEditText(id, pFont, x, y);

	if(editText == NULL) {
		lua_pushnil(L);
		return 1;
	}

	LuaProxy* proxy = (LuaProxy*)lua_newuserdata(L, sizeof(LuaProxy));

	proxy->instance = editText;
	proxy->type = EDITTEXT_PROXY;

	luaL_getmetatable(L, EDITTEXT_MT_NAME);
	lua_setmetatable(L, -2);

	return 1;
}

int edi_GetZ(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, EDITTEXT_MT_NAME);
	YmxEditText* editText = (YmxEditText*)proxy->instance;

	if(editText == NULL )
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	float z = editText->GetZ();

	lua_pushnumber(L, z);
	return 1;
}

int edi_SetZ(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, EDITTEXT_MT_NAME);
	YmxEditText* editText = (YmxEditText*)proxy->instance;

	if(editText == NULL )
	{
		return 0;
	}

	if(!lua_isnumber(L, 2))
	{
		return 0;
	}

	float z = (float)lua_tonumber(L, 2);
	editText->SetZ(z);
	return 0;
}

int edi_SetFont(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, EDITTEXT_MT_NAME);
	YmxEditText* editText = (YmxEditText*)proxy->instance;

	if(editText == NULL )
		return 0;

	LPYMXFONT pFont = NULL;
	if(lua_isuserdata(L, 2)) {
		LuaProxy* pFontProxy = (LuaProxy*)luaL_checkudata(L, 2, FONT_MT_NAME);
		pFont = (LPYMXFONT)pFontProxy->instance;
	}

	editText->SetFont(pFont);
	return 0;
}

int edi_SetTextColor(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, EDITTEXT_MT_NAME);
	YmxEditText* editText = (YmxEditText*)proxy->instance;

	if(editText == NULL )
		return 0;

	YMXCOLOR color;
	if(GetColorFromLuaArg(L, 2, &color))
	{
		editText->SetTextColor(color);
	}

	return 0;
}

int edi_Update(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, EDITTEXT_MT_NAME);
	YmxEditText* editText = (YmxEditText*)proxy->instance;

	if(editText == NULL )
		return 0;

	float delta;
	if(lua_isnumber(L, 2)) 
		delta = (float)lua_tonumber(L, 2);
	else 
		delta = GetGameObject()->GetUpdateDelta();

	editText->Update(delta);
	return 0;
}

int edi_Render(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, EDITTEXT_MT_NAME);
	YmxEditText* editText = (YmxEditText*)proxy->instance;

	if(editText == NULL )
		return 0;

	YmxGraphics* g = (YmxGraphics*)lua_touserdata(L, 2);
	if(g == NULL)
		return 0;

	editText->Render(g);
	return 0;
}

int edi_SetPosition(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, EDITTEXT_MT_NAME);
	YmxEditText* editText = (YmxEditText*)proxy->instance;

	if(editText == NULL )
		return 0;

	if(lua_istable(L, 2))
	{
		YmxVertex v;
		if(GetVertexFromLuaArg(L, 2, &v))
		{
			editText->SetPosition(v.x, v.y);
			lua_pushboolean(L, 1);
			return 1;
		}
	}
	else if(lua_isnumber(L, 2) && lua_isnumber(L, 3))
	{
		float x = (float)lua_tonumber(L, 2);
		float y = (float)lua_tonumber(L, 3);
		editText->SetPosition(x, y);
		lua_pushboolean(L, 1);
		return 1;
	}

	lua_pushboolean(L, 0);
	return 1;
}

int edi_SetTextAlignment(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, EDITTEXT_MT_NAME);
	YmxEditText* editText = (YmxEditText*)proxy->instance;

	if(editText == NULL )
		return 0;

	const char* align = lua_tostring(L, 2);
	if(_stricmp(align, "left") == 0)
		editText->SetTextAlignment(TEXT_ALIGNMENT_LEFT);
	else if(_stricmp(align, "center") == 0)
		editText->SetTextAlignment(TEXT_ALIGNMENT_CENTER);
	else if(_stricmp(align, "right") == 0)
		editText->SetTextAlignment(TEXT_ALIGNMENT_RIGHT);

	return 0;
}

int edi_SetVisible(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, EDITTEXT_MT_NAME);
	YmxEditText* editText = (YmxEditText*)proxy->instance;

	if(editText == NULL) {
		lua_pushboolean(L, 0);
		return 1;
	}

	if(lua_toboolean(L, 2))
		editText->SetVisible(TRUE);
	else 
		editText->SetVisible(FALSE);

	lua_pushboolean(L, 1);
	return 1;
}

int edi_SetFocus(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, EDITTEXT_MT_NAME);
	YmxEditText* editText = (YmxEditText*)proxy->instance;

	if(editText == NULL) {
		lua_pushboolean(L, 0);
		return 1;
	}

	if(lua_toboolean(L, 2))
		editText->SetFocus(TRUE);
	else 
		editText->SetFocus(FALSE);

	lua_pushboolean(L, 1);
	return 1;
}

int edi_IsVisible(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, EDITTEXT_MT_NAME);
	YmxEditText* editText = (YmxEditText*)proxy->instance;

	if(editText == NULL) {
		lua_pushnil(L);
		return 1;
	}

	lua_pushboolean(L, editText->IsVisible());
	return 1;
}


int edi_IsFocused(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, EDITTEXT_MT_NAME);
	YmxEditText* editText = (YmxEditText*)proxy->instance;

	if(editText == NULL) {
		lua_pushnil(L);
		return 1;
	}

	lua_pushboolean(L, editText->IsFocused());
	return 1;
}

int edi_Release(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, EDITTEXT_MT_NAME);
	YmxEditText* editText = (YmxEditText*)proxy->instance;

	if(editText == NULL) {
		return 0;
	}

	editText->Release();
	return 0;	
}


int edi_SetText(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, EDITTEXT_MT_NAME);
	YmxEditText* editText = (YmxEditText*)proxy->instance;

	if(editText == NULL) {
		lua_pushboolean(L, 0);
		return 1;
	}

	const char* s = lua_tostring(L, 2);

#ifdef _UNICODE
	TCHAR* buf = (TCHAR*)malloc((strlen(s) + 1) * sizeof(TCHAR));
	ConvertAsciiToPTStrs(s, buf, strlen(s) + 1);
	editText->SetText(buf);
	free(buf);
#else
	editText->SetText(s);
#endif

	lua_pushboolean(L, 1);
	return 1;
}

int edi_GetText(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, EDITTEXT_MT_NAME);
	YmxEditText* editText = (YmxEditText*)proxy->instance;

	if(editText == NULL) {
		lua_pushboolean(L, 0);
		return 1;
	}

	PCTSTR text = editText->GetText();
#ifdef _UNICODE
	char* buf = (char*)malloc((lstrlen(text) + 1) * sizeof(char));
	ConvertPTStrsToAscii(text, buf, lstrlen(text) + 1);
	lua_pushstring(L, buf);
	free(buf);
#else
	lua_pushstring(L, text);
#endif

	return 1;
}

int edi_SetCursor(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, EDITTEXT_MT_NAME);
	YmxEditText* editText = (YmxEditText*)proxy->instance;

	if(editText == NULL || !lua_istable(L, 2)) {
		lua_pushboolean(L, 0);
		return 1;
	}

	lua_getfield(L, 2, "ORIENTATION");
	if(lua_isstring(L, -1)){
		const char* orientation = lua_tostring(L, -1);
		if(_stricmp(orientation, "vertical") == 0)
			editText->SetCursorStyle(YMXEDITTEXT_CURSOR_VERTICAL);
		else if(_stricmp(orientation, "horizontal") == 0)
			editText->SetCursorStyle(YMXEDITTEXT_CURSOR_HORIZONTAL);
	}
	lua_pop(L, 1);

	lua_getfield(L, 2, "SHOW");
	if(!lua_isnil(L, -1))
	{
		if(!lua_toboolean(L, -1))
			editText->SetCursorStyle(YMXEDITTEXT_CURSOR_NONE);
	}
	lua_pop(L, 1);

	lua_getfield(L, 2, "THICKNESS");
	if(lua_isnumber(L, -1))
	{
		float thickness = (float)lua_tonumber(L, -1);
		editText->SetCursorThickness(thickness);
	}
	lua_pop(L, 1);

	lua_getfield(L, 2, "BLINK");
	if(!lua_isnil(L, -1))
	{
		if(lua_toboolean(L, -1))
			editText->SetCursorBlink(TRUE);
		else
			editText->SetCursorBlink(FALSE);
	}
	lua_pop(L, 1);

	lua_getfield(L, 2, "BLINKFREQ");
	if(lua_isnumber(L, -1))
	{
		float freq = (float)lua_tonumber(L, -1);
		editText->SetCursorBlinkFreq(freq);
	}
	lua_pop(L, 1);

	lua_getfield(L, 2, "COLOR");
	YMXCOLOR color;
	if(GetColorFromLuaArg(L, lua_gettop(L), &color))
	{
		editText->SetCursorColor(color);
	}
	lua_pop(L, 1);

	lua_pushboolean(L, 1);
	return 1;
}

int edi_SetConfirmKey(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, EDITTEXT_MT_NAME);
	YmxEditText* editText = (YmxEditText*)proxy->instance;

	if(editText == NULL) {
		lua_pushboolean(L, 0);
		return 1;
	}

	int key = GetKeyFromLuaArg(L, 2);
	if(key == -1) {
		lua_pushboolean(L, 0);
		return 1;
	}

	editText->SetConfirmKey(key);
	lua_pushboolean(L, 1);
	return 1;
}


