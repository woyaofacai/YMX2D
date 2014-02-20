#include "stdafx.h"
#include "ymxluautil.h"
#include "ymxluacore.h"

const char* SPRITE_MT_NAME = "ymxlua.sprite";
const struct luaL_Reg spriteLib_m[] = {
	{"render", spr_Render},
	{"setTextureRect", spr_SetTextureRect},
	{"getWidth", spr_GetWidth},
	{"getHeight", spr_GetHeight},
	{"getZ", spr_GetZ},
	{"setZ", spr_SetZ},
	{"setHotSpot", spr_SetHotSpot},
	{"release", spr_Release},
	{0, 0}
};

int spr_Create(lua_State* L)
{
	LPYMXCOMPONENT pComponent = NULL;
	pComponent = GetComponentFromLuaArg(L, 1);

	LuaProxy* pTexProxy = (LuaProxy*)luaL_checkudata(L, 2, TEXTURE_MT_NAME);
	LPYMXTEXTURE pTexture = (LPYMXTEXTURE)pTexProxy->instance;
	LPYMXSPRITE pSprite = NULL;

	if(lua_istable(L, 3))
	{
		YmxRect srcRect;
		lua_rawgeti(L, 3, 1);
		lua_rawgeti(L, 3, 2);
		lua_rawgeti(L, 3, 3);
		lua_rawgeti(L, 3, 4);

		srcRect.left = lua_tonumber(L, -4);
		srcRect.top  = lua_tonumber(L, -3);
		srcRect.right = lua_tonumber(L, -2);
		srcRect.bottom = lua_tonumber(L, -1);

		lua_pop(L, 4);

		if(pComponent != NULL)
			pSprite = pComponent->CreateSprite(pTexture, srcRect);
		else 
			pSprite = GetGameObject()->CreateSprite(pTexture, srcRect);
	}
	else {
		if(pComponent != NULL)
			pSprite = pComponent->CreateSprite(pTexture);
		else 
			pSprite = GetGameObject()->CreateSprite(pTexture);
	}

	if(pSprite == NULL) {
		lua_pushnil(L);
		return 1;
	}

	LuaProxy* pSpriteProxy = (LuaProxy*)lua_newuserdata(L, sizeof(LuaProxy));
	pSpriteProxy->instance = pSprite;
	pSpriteProxy->type = SPRITE_PROXY;

	luaL_getmetatable(L, SPRITE_MT_NAME);
	lua_setmetatable(L, -2);

	return 1;
}

int spr_Render(lua_State* L)
{
	LuaProxy* spriteProxy = (LuaProxy*)luaL_checkudata(L, 1, SPRITE_MT_NAME);
	YmxSprite* sprite = (YmxSprite*)spriteProxy->instance;

	YmxGraphics* g = (YmxGraphics*)lua_touserdata(L, 2);
	float x = (float)luaL_checknumber(L, 3);
	float y = (float)luaL_checknumber(L, 4);

	if(lua_gettop(L) <= 4)
	{
		sprite->Render(g, x, y);
	}
	else 
	{
		float rot = (float)luaL_checknumber(L, 5);
		float hScale = 1.0f, vScale = 1.0f;
		if(lua_isnumber(L, 6)) hScale = (float)lua_tonumber(L, 6);
		if(lua_isnumber(L, 7)) vScale = (float)lua_tonumber(L, 7);
		sprite->Render(g, x, y, rot, hScale, vScale);
	}

	return 0;
}

int spr_SetTextureRect(lua_State* L)
{
	LuaProxy* spriteProxy = (LuaProxy*)luaL_checkudata(L, 1, SPRITE_MT_NAME);
	YmxSprite* sprite = (YmxSprite*)spriteProxy->instance;

	YmxRect texRect;
	if(lua_istable(L, 2)) 
	{
		lua_rawgeti(L, 2, 1);
		lua_rawgeti(L, 2, 2);
		lua_rawgeti(L, 2, 3);
		lua_rawgeti(L, 2, 4);

		texRect.left = (float)lua_tonumber(L, -4);
		texRect.top  = (float)lua_tonumber(L, -3);
		texRect.right = (float)lua_tonumber(L, -2);
		texRect.bottom = (float)lua_tonumber(L, -1);
		lua_pop(L, 4);
	}
	else {
		texRect.left = (float)luaL_checknumber(L, 2);
		texRect.top = (float)luaL_checknumber(L, 3);
		texRect.right = (float)luaL_checknumber(L, 4);
		texRect.bottom = (float)luaL_checknumber(L, 5);
	}

	sprite->SetTextureRect(texRect);
	return 0;
}

int spr_GetWidth(lua_State* L)
{
	LuaProxy* spriteProxy = (LuaProxy*)luaL_checkudata(L, 1, SPRITE_MT_NAME);
	YmxSprite* sprite = (YmxSprite*)spriteProxy->instance;

	float width = sprite->GetWidth();
	lua_pushnumber(L, width);
	return 1;
}

int spr_GetHeight(lua_State* L)
{
	LuaProxy* spriteProxy = (LuaProxy*)luaL_checkudata(L, 1, SPRITE_MT_NAME);
	YmxSprite* sprite = (YmxSprite*)spriteProxy->instance;

	float height = sprite->GetHeight();
	lua_pushnumber(L, height);
	return 1;
}

int spr_SetZ(lua_State* L)
{
	LuaProxy* spriteProxy = (LuaProxy*)luaL_checkudata(L, 1, SPRITE_MT_NAME);
	YmxSprite* sprite = (YmxSprite*)spriteProxy->instance;

	float z = (float)luaL_checknumber(L, 2);
	sprite->SetZ(z);
	return 0;
}

int spr_GetZ(lua_State* L)
{
	LuaProxy* spriteProxy = (LuaProxy*)luaL_checkudata(L, 1, SPRITE_MT_NAME);
	YmxSprite* sprite = (YmxSprite*)spriteProxy->instance;

	float z = sprite->GetZ();
	lua_pushnumber(L, z);
	return 1;
}

int spr_SetHotSpot(lua_State* L)
{
	LuaProxy* spriteProxy = (LuaProxy*)luaL_checkudata(L, 1, SPRITE_MT_NAME);
	YmxSprite* sprite = (YmxSprite*)spriteProxy->instance;

	float x = (float)luaL_checknumber(L, 2);
	float y = (float)luaL_checknumber(L, 3);

	sprite->SetHotSpot(x, y);
	return 1;
}

int spr_Release(lua_State* L)
{
	LuaProxy* spriteProxy = (LuaProxy*)luaL_checkudata(L, 1, SPRITE_MT_NAME);
	YmxSprite* sprite = (YmxSprite*)spriteProxy->instance;

	if(sprite == NULL) {
		return 0;
	}

	sprite->Release();
	return 0;	
}