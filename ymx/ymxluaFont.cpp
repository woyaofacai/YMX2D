#include "stdafx.h"
#include "ymxluautil.h"
#include "ymxluacore.h"

const char* FONT_MT_NAME = "ymxlua.font";
const struct luaL_Reg fontLib_m[] = {
	{"setText", fnt_SetText},
	{"getText", fnt_GetText},
	{"render", fnt_Render},
	{"setZ", fnt_SetZ},
	{"getZ", fnt_GetZ},
	{"getColor", fnt_GetColor},
	{"setColor", fnt_SetColor},
	{"setAttribute", fnt_SetAttribute},
	{"getLineSpacing", fnt_GetLineSpacing},
	{"getCharSpacing", fnt_GetCharSpacing},
	{"setLineSpacing", fnt_SetLineSpacing},
	{"setCharSpacing", fnt_SetCharSpacing},
	{"getWidth", fnt_GetWidth},
	{"getHeight", fnt_GetHeight},
	{"release", fnt_Release},
	{0, 0}
};

int fnt_Load(lua_State* L)
{
	const char* fileName = luaL_checkstring(L, 1);
	TCHAR fileNameBuf[1024];
	ConvertAsciiToPTStrs(fileName, fileNameBuf, 1024);

	LPYMXCOMPONENT pComponent = GetComponentFromLuaArg(L, 2);
	YmxFont* pFont = NULL;
	if(pComponent == NULL) 
		pFont = GetGameObject()->CreateFont(fileNameBuf);
	else 
		pFont = pComponent->CreateFont(fileNameBuf);

	if(pFont == NULL) {
		lua_pushnil(L);
		return 1;
	}

	LuaProxy* pFontProxy = (LuaProxy*)lua_newuserdata(L, sizeof(LuaProxy));
	pFontProxy->instance = (void*)pFont;
	pFontProxy->type = FONT_PROXY;

	luaL_getmetatable(L, FONT_MT_NAME);
	lua_setmetatable(L, -2);

	return 1;
}


int fnt_SetText(lua_State* L)
{
	LuaProxy* fontProxy = (LuaProxy*)luaL_checkudata(L, 1, FONT_MT_NAME);
	YmxFont* pFont = (YmxFont*)fontProxy->instance;

	if(!lua_isstring(L, 2)) {
		lua_pushboolean(L, 0);
		return 1;
	}

	const char* s = lua_tostring(L, 2);
	int length = strlen(s);
	TCHAR *buf = (TCHAR*)malloc((length + 1) * sizeof(TCHAR));
	ConvertAsciiToPTStrs(s, buf, length + 1);
	pFont->SetText(buf);
	free(buf);

	lua_pushboolean(L, 1);
	return 1;
}

int fnt_GetText(lua_State* L)
{
	LuaProxy* fontProxy = (LuaProxy*)luaL_checkudata(L, 1, FONT_MT_NAME);
	YmxFont* pFont = (YmxFont*)fontProxy->instance;

	PCTSTR s = pFont->GetText();
	int length = lstrlen(s);

	char *buf = (char*)malloc((length + 1) * sizeof(char));
	ConvertPTStrsToAscii(s, buf, length + 1);

	lua_pushstring(L, buf);
	free(buf);
	return 1;
}



int fnt_Render(lua_State* L)
{
	LuaProxy* fontProxy = (LuaProxy*)luaL_checkudata(L, 1, FONT_MT_NAME);
	YmxFont* pFont = (YmxFont*)fontProxy->instance;

	YmxGraphics* g = (YmxGraphics*)lua_touserdata(L, 2);
	float x = (float)luaL_checknumber(L, 3);
	float y = (float)luaL_checknumber(L, 4);

	pFont->Render(g, x, y);

	return 0;
}

int fnt_SetZ(lua_State* L)
{
	LuaProxy* fontProxy = (LuaProxy*)luaL_checkudata(L, 1, FONT_MT_NAME);
	YmxFont* pFont = (YmxFont*)fontProxy->instance;

	float z = (float)luaL_checknumber(L, 2);
	pFont->SetZ(z);

	return 0;
}

int fnt_GetZ(lua_State* L)
{
	LuaProxy* fontProxy = (LuaProxy*)luaL_checkudata(L, 1, FONT_MT_NAME);
	YmxFont* pFont = (YmxFont*)fontProxy->instance;

	float z = pFont->GetZ();
	lua_pushnumber(L, z);

	return 1;
}

int fnt_GetColor(lua_State* L)
{
	LuaProxy* fontProxy = (LuaProxy*)luaL_checkudata(L, 1, FONT_MT_NAME);
	YmxFont* pFont = (YmxFont*)fontProxy->instance;

	YMXCOLOR color = pFont->GetColor();

	int r = (color & 0x00FF0000) >> 16;
	int g = (color & 0x0000FF00) >> 8;
	int b = color & 0x000000FF;

	lua_pushinteger(L, r);
	lua_pushinteger(L, g);
	lua_pushinteger(L, b);

	return 3;
}

int fnt_SetColor(lua_State* L)
{
	LuaProxy* fontProxy = (LuaProxy*)luaL_checkudata(L, 1, FONT_MT_NAME);
	YmxFont* pFont = (YmxFont*)fontProxy->instance;

	if(lua_istable(L, 2))
	{
		if(lua_objlen(L, 2) >= 3)
		{
			lua_rawgeti(L, 2, 1);
			lua_rawgeti(L, 2, 2);
			lua_rawgeti(L, 2, 3);

			int r = (int)lua_tointeger(L, -3);
			int g = (int)lua_tointeger(L, -2);
			int b = (int)lua_tointeger(L, -1);

			lua_pop(L, 3);
			pFont->SetColor(YMX_RGB(r,g,b));
		}
	}
	else if(lua_gettop(L) >= 4){
		int r = (int)luaL_checkinteger(L, 2);
		int g = (int)luaL_checkinteger(L, 3);
		int b = (int)luaL_checkinteger(L, 4);

		pFont->SetColor(YMX_RGB(r, g, b));
	}

	return 0;
}

struct FontAttributeNameAndType
{
	const char* name;
	YMXFONT_ATTRIBUTE type;
};

static const FontAttributeNameAndType attributeTypes[] = 
{
	{"BUFFER_COUNT", FONT_BUFFER_COUNT},
	{"MULTILINE", FONT_MULTILINE},
	{"AUTO_NEWLINE", FONT_AUTO_NEWLINE},
	{"MAX_CHAR_COUNT",FONT_MAX_CHAR_COUNT},
	{0, FONT_MAX_CHAR_COUNT}
};

int fnt_SetAttribute(lua_State* L)
{
	LuaProxy* fontProxy = (LuaProxy*)luaL_checkudata(L, 1, FONT_MT_NAME);
	YmxFont* pFont = (YmxFont*)fontProxy->instance;

	if(!lua_istable(L, 2)) return 0;

	for(const FontAttributeNameAndType* ant = attributeTypes; ant->name != 0; ant++)
	{
		lua_getfield(L, 2, ant->name);
		if(!lua_isnil(L, -1)) 
		{
			int val;
			if(lua_isboolean(L, -1)) {
				if(lua_toboolean(L, -1)) val = 1;
				else val = 0;
			}
			else {
				val = lua_tointeger(L, -1);
			}
			pFont->SetAttribute(ant->type, val);
		}
		lua_pop(L, 1);
	}
	return 0;
}

int fnt_SetLineSpacing(lua_State* L)
{
	LuaProxy* fontProxy = (LuaProxy*)luaL_checkudata(L, 1, FONT_MT_NAME);
	YmxFont* pFont = (YmxFont*)fontProxy->instance;

	float lineSpacing = (float)luaL_checknumber(L, 2);

	pFont->SetLineSpacing(lineSpacing);
	return 0;
}

int fnt_SetCharSpacing(lua_State* L)
{
	LuaProxy* fontProxy = (LuaProxy*)luaL_checkudata(L, 1, FONT_MT_NAME);
	YmxFont* pFont = (YmxFont*)fontProxy->instance;

	float colSpacing = (float)luaL_checknumber(L, 2);

	pFont->SetCharSpacing(colSpacing);
	return 0;
}

int fnt_GetLineSpacing(lua_State* L)
{
	LuaProxy* fontProxy = (LuaProxy*)luaL_checkudata(L, 1, FONT_MT_NAME);
	YmxFont* pFont = (YmxFont*)fontProxy->instance;

	lua_pushnumber(L, pFont->GetLineSpacing());

	return 1;
}

int fnt_GetCharSpacing(lua_State* L)
{
	LuaProxy* fontProxy = (LuaProxy*)luaL_checkudata(L, 1, FONT_MT_NAME);
	YmxFont* pFont = (YmxFont*)fontProxy->instance;

	lua_pushnumber(L, pFont->GetCharSpacing());
	return 1;
}

int fnt_GetWidth(lua_State* L)
{
	LuaProxy* fontProxy = (LuaProxy*)luaL_checkudata(L, 1, FONT_MT_NAME);
	YmxFont* pFont = (YmxFont*)fontProxy->instance;

	lua_pushnumber(L, pFont->GetWidth());
	return 1;
}

int fnt_GetHeight(lua_State* L)
{
	LuaProxy* fontProxy = (LuaProxy*)luaL_checkudata(L, 1, FONT_MT_NAME);
	YmxFont* pFont = (YmxFont*)fontProxy->instance;

	lua_pushnumber(L, pFont->GetHeight());
	return 1;
}

int fnt_Release(lua_State* L)
{
	LuaProxy* fontProxy = (LuaProxy*)luaL_checkudata(L, 1, FONT_MT_NAME);
	YmxFont* pFont = (YmxFont*)fontProxy->instance;

	pFont->Release();
	fontProxy->instance = NULL;

	return 0;
}