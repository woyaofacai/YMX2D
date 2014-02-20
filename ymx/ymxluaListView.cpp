#include "stdafx.h"
#include "ymxluautil.h"
#include "ymxluacore.h"
#include "ymxluaform.h"

const char* LISTVIEW_MT_NAME = "ymxlua.listview"; 
const struct luaL_Reg listViewLib_m[] = {
	{"setTexture", list_SetTexture},
	{"setTextureRect", list_SetTextureRect},
	{"setFont", list_SetFont},
	{"setTextColor", list_SetTextColor},
	{"setItemSize", list_SetItemSize},
	{"getItemSize", list_GetItemSize},
	{"setZ", list_SetZ},
	{"getZ", list_GetZ},
	{"update", list_Update},
	{"render", list_Render},
	{"setPosition", list_SetPosition},
	{"addItem", list_AddItem},
	{"removeItem", list_RemoveItem},
	{"setTextAlignment", list_SetTextAlignment},
	{"enableKeyboard", list_EnableKeyboard},
	{"enableMouse", list_EnableMouse},
	{"setFocusItem", list_SetFocusItem},
	{"setConfirmKey", list_SetConfirmKey},
	{"setUpKey", list_SetUpKey},
	{"setDownKey", list_SetDownKey},
	{"setVisible", list_SetVisible},
	{"isVisible", list_IsVisible},
	{"release", list_Release},
	{0, 0}
};


struct ListStateEnumType
{
	const char* name;
	YMXLISTVIEW_STATE value;
};

static const ListStateEnumType stateNameTypes[] = {
	{"NORMAL", YMXLISTVIEW_NORMAL},
	{"PRESSED", YMXLISTVIEW_PRESSED},
	{"MOUSEOVER", YMXLISTVIEW_MOUSEOVER},
	{0, YMXLISTVIEW_NORMAL}
};

int list_Create(lua_State* L)
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

	YmxListView* listView = pComponent->CreateListView(id, pTexture, pFont, x, y);
	if(listView == NULL) {
		lua_pushnil(L);
		return 1;
	}

	LuaProxy* proxy = (LuaProxy*)lua_newuserdata(L, sizeof(LuaProxy));

	proxy->instance = listView;
	proxy->type = LISTVIEW_PROXY;

	luaL_getmetatable(L, LISTVIEW_MT_NAME);
	lua_setmetatable(L, -2);

	return 1;
}


int list_SetTexture(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, LISTVIEW_MT_NAME);
	YmxListView* listView = (YmxListView*)proxy->instance;

	if(listView == NULL) 
		return 0;

	LPYMXTEXTURE pTexture = NULL;
	if(lua_isuserdata(L, 2)) {
		LuaProxy* pTexProxy = (LuaProxy*)luaL_checkudata(L, 2, TEXTURE_MT_NAME);
		pTexture = (LPYMXTEXTURE)pTexProxy->instance;
	}

	listView->SetTexture(pTexture);
	return 0;
}



int list_SetTextureRect(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, LISTVIEW_MT_NAME);
	YmxListView* listView = (YmxListView*)proxy->instance;

	if(listView == NULL)
		return 0;

	if(lua_istable(L, 2))
	{
		for (const ListStateEnumType* snt = stateNameTypes; snt->name != 0; snt++)
		{
			lua_pushstring(L, snt->name);
			lua_rawget(L, 2);
			if(lua_istable(L, -1))
			{
				YmxRect rect;
				if(GetRectFromLuaArg(L, lua_gettop(L), &rect))
				{
					listView->SetTextureRect(snt->value, rect);
				}
			}
			lua_pop(L, 1);
		}
	}
	else if(lua_isstring(L, 2) && lua_istable(L, 3))
	{
		const char* state = lua_tostring(L, 2);
		for (const ListStateEnumType* snt = stateNameTypes; snt->name != 0; snt++)
		{
			if(stricmp(state, snt->name) == 0)
			{
				YmxRect rect;
				if (GetRectFromLuaArg(L, 3, &rect))
				{
					listView->SetTextureRect(snt->value, rect);
				}
				break;
			}
		}
	}

	return 0;
}


int list_SetFont(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, LISTVIEW_MT_NAME);
	YmxListView* listView = (YmxListView*)proxy->instance;

	if(listView == NULL)
		return 0;

	LPYMXFONT pFont = NULL;
	if(lua_isuserdata(L, 2)) {
		LuaProxy* pFontProxy = (LuaProxy*)luaL_checkudata(L, 2, FONT_MT_NAME);
		pFont = (LPYMXFONT)pFontProxy->instance;
	}

	listView->SetFont(pFont);
	return 0;
}


int list_SetTextColor(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, LISTVIEW_MT_NAME);
	YmxListView* listView = (YmxListView*)proxy->instance;

	if(listView == NULL)
		return 0;

	if(lua_istable(L, 2))
	{
		for (const ListStateEnumType* snt = stateNameTypes; snt->name != 0; snt++)
		{
			lua_pushstring(L, snt->name);
			lua_rawget(L, 2);
			if(lua_istable(L, -1))
			{
				YMXCOLOR color;
				if(GetColorFromLuaArg(L, lua_gettop(L), &color))
				{
					listView->SetTextColor(snt->value, color);
				}
			}
			lua_pop(L, 1);
		}
	}
	else if(lua_isstring(L, 2) && lua_istable(L, 3))
	{
		const char* state = lua_tostring(L, 2);
		for (const ListStateEnumType* snt = stateNameTypes; snt->name != 0; snt++)
		{
			if(stricmp(state, snt->name) == 0)
			{
				YMXCOLOR color;
				if(GetColorFromLuaArg(L, 3, &color))
				{
					listView->SetTextColor(snt->value, color);
				}
				break;
			}
		}
	}

	return 0;
}

int list_SetItemSize(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, LISTVIEW_MT_NAME);
	YmxListView* listView = (YmxListView*)proxy->instance;

	if(listView == NULL)
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	if(!lua_isnumber(L, 2) || !lua_isnumber(L, 3))
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	float width = (float)lua_tonumber(L, 2);
	float height = (float)lua_tonumber(L, 3);

	listView->SetItemSize(width, height);
	lua_pushboolean(L, 1);
	return 1;
}


int list_GetItemSize(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, LISTVIEW_MT_NAME);
	YmxListView* listView = (YmxListView*)proxy->instance;

	if(listView == NULL)
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	float width, height;
	listView->GetItemSize(&width, &height);
	lua_pushnumber(L, width);
	lua_pushnumber(L, height);
	return 2;
}


int list_SetZ(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, LISTVIEW_MT_NAME);
	YmxListView* listView = (YmxListView*)proxy->instance;

	if(listView == NULL )
	{
		return 0;
	}

	if(!lua_isnumber(L, 2))
	{
		return 0;
	}

	float z = (float)lua_tonumber(L, 2);
	listView->SetZ(z);
	return 0;
}

int list_GetZ(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, LISTVIEW_MT_NAME);
	YmxListView* listView = (YmxListView*)proxy->instance;

	if(listView == NULL )
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	float z = listView->GetZ();

	lua_pushnumber(L, z);
	return 1;
}

int list_Update(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, LISTVIEW_MT_NAME);
	YmxListView* listView = (YmxListView*)proxy->instance;

	if(listView == NULL)
		return 0;

	float delta;
	if(lua_isnumber(L, 2)) 
		delta = (float)lua_tonumber(L, 2);
	else 
		delta = GetGameObject()->GetUpdateDelta();

	listView->Update(delta);
	return 0;
}

int list_Render(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, LISTVIEW_MT_NAME);
	YmxListView* listView = (YmxListView*)proxy->instance;

	if(listView == NULL)
		return 0;

	YmxGraphics* g = (YmxGraphics*)lua_touserdata(L, 2);
	if(g == NULL)
		return 0;

	listView->Render(g);
	return 0;
}

int list_SetPosition(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, LISTVIEW_MT_NAME);
	YmxListView* listView = (YmxListView*)proxy->instance;

	if(listView == NULL)
		return 0;

	if(lua_istable(L, 2))
	{
		YmxVertex v;
		if(GetVertexFromLuaArg(L, 2, &v))
		{
			listView->SetPosition(v.x, v.y);
			lua_pushboolean(L, 1);
			return 1;
		}
	}
	else if(lua_isnumber(L, 2) && lua_isnumber(L, 3))
	{
		float x = (float)lua_tonumber(L, 2);
		float y = (float)lua_tonumber(L, 3);
		listView->SetPosition(x, y);
		lua_pushboolean(L, 1);
		return 1;
	}

	lua_pushboolean(L, 0);
	return 1;
}

int list_AddItem(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, LISTVIEW_MT_NAME);
	YmxListView* listView = (YmxListView*)proxy->instance;

	if(listView == NULL)
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	const char* s = lua_tostring(L, 2);

#ifdef _UNICODE
	TCHAR* buf = (TCHAR*)malloc((strlen(s) + 1) * sizeof(TCHAR));
	ConvertAsciiToPTStrs(s, buf, strlen(s) + 1);
	listView->AddItem(buf);
	free(buf);
#else
	listView->AddItem(s);
#endif

	lua_pushboolean(L, 1);
	return 1;
}
int list_RemoveItem(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, LISTVIEW_MT_NAME);
	YmxListView* listView = (YmxListView*)proxy->instance;

	if(listView == NULL)
	{
		return 0;
	}

	if(!lua_isnumber(L, 2))
	{
		return 0;
	}
	int index = (int)lua_tointeger(L, 2);
	listView->RemoveItem(index);
	return 0;
}

int list_SetTextAlignment(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, LISTVIEW_MT_NAME);
	YmxListView* listView = (YmxListView*)proxy->instance;

	if(listView == NULL || !lua_isstring(L, 2))
	{
		return 0;
	}

	const char* align = lua_tostring(L, 2);
	if(_stricmp(align, "left") == 0)
		listView->SetTextAlignment(TEXT_ALIGNMENT_LEFT);
	else if(_stricmp(align, "center") == 0)
		listView->SetTextAlignment(TEXT_ALIGNMENT_CENTER);
	else if(_stricmp(align, "right") == 0)
		listView->SetTextAlignment(TEXT_ALIGNMENT_RIGHT);

	return 0;
}


int list_EnableKeyboard(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, LISTVIEW_MT_NAME);
	YmxListView* listView = (YmxListView*)proxy->instance;

	if(listView == NULL)
	{
		return 0;
	}

	if(lua_toboolean(L, 2))
		listView->EnableKeyboard(TRUE);
	else 
		listView->EnableKeyboard(FALSE);

	return 0;
}

int list_EnableMouse(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, LISTVIEW_MT_NAME);
	YmxListView* listView = (YmxListView*)proxy->instance;

	if(listView == NULL)
	{
		return 0;
	}

	if(lua_toboolean(L, 2))
		listView->EnableMouse(TRUE);
	else 
		listView->EnableMouse(FALSE);

	return 0;
}

int list_SetFocusItem(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, LISTVIEW_MT_NAME);
	YmxListView* listView = (YmxListView*)proxy->instance;

	if(listView == NULL || !lua_isnumber(L, 2))
	{
		return 0;
	}

	int index = (int)lua_tointeger(L, 2);
	listView->SetFocusItem(index);
	return 0;
}

int list_GetFocusItem(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, LISTVIEW_MT_NAME);
	YmxListView* listView = (YmxListView*)proxy->instance;

	if(listView == NULL)
	{
		lua_pushboolean(L, 0);
		return 1;
	}

	int index = listView->GetFocusItem();
	if(index < 0) {
		lua_pushboolean(L, 0);
	}
	else {
		lua_pushnumber(L, index);
	}
	return 1;
}

int list_SetConfirmKey(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, LISTVIEW_MT_NAME);
	YmxListView* listView = (YmxListView*)proxy->instance;

	if(listView == NULL)
	{
		return 0;
	}

	int key = GetKeyFromLuaArg(L, 2);
	if(key != -1)
	{
		listView->SetConfirmKey(key);
	}
	return 0;
}


int list_SetUpKey(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, LISTVIEW_MT_NAME);
	YmxListView* listView = (YmxListView*)proxy->instance;

	if(listView == NULL)
	{
		return 0;
	}

	int key = GetKeyFromLuaArg(L, 2);
	if(key != -1)
	{
		listView->SetUpKey(key);
	}
	return 0;	
}

int list_SetDownKey(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, LISTVIEW_MT_NAME);
	YmxListView* listView = (YmxListView*)proxy->instance;

	if(listView == NULL)
	{
		return 0;
	}

	int key = GetKeyFromLuaArg(L, 2);
	if(key != -1)
	{
		listView->SetDownKey(key);
	}
	return 0;
}

int list_SetVisible(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, LISTVIEW_MT_NAME);
	YmxListView* listView = (YmxListView*)proxy->instance;

	if(listView == NULL) {
		lua_pushboolean(L, 0);
		return 1;
	}

	if(lua_toboolean(L, 2))
		listView->SetVisible(TRUE);
	else 
		listView->SetVisible(FALSE);

	lua_pushboolean(L, 1);
	return 1;
}

int list_IsVisible(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, LISTVIEW_MT_NAME);
	YmxListView* listView = (YmxListView*)proxy->instance;

	if(listView == NULL) {
		lua_pushnil(L);
		return 1;
	}

	lua_pushboolean(L, listView->IsVisible());
	return 1;
}


int list_Release(lua_State* L)
{
	LuaProxy* proxy = (LuaProxy*)luaL_checkudata(L, 1, LISTVIEW_MT_NAME);
	YmxListView* listView = (YmxListView*)proxy->instance;

	if(listView == NULL) {
		return 0;
	}

	listView->Release();
	return 0;
}