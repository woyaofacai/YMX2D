// yxm.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "ymx.h"
#include "ymxluautil.h"
#include "ymxluacore.h"
#include "ymxluaform.h"

static const struct luaL_Reg modules[] = 
{
	{"log", err_Log},
	{"getWindowWidth",win_GetWindowWidth},
	{"getWindowHeight",win_GetWindowHeight},
	{"recordError",err_RecordError},
	{"messageBox", err_MessageBox},
	{"loadTexture", tex_Load},
	{"createSprite", spr_Create},
	{"getMousePos",input_GetMousePos},
	{"setMousePos", input_SetMousePos},
	{"keyDown", input_KeyDown},
	{"keyUp", input_KeyUp},
	{"getKeyState", input_GetKeyState},
	{"getInputKey", input_GetInputKey},
	{"getChar", input_GetChar},
	{"createFont", fnt_Load},
	{"createParticleSystem", par_Create},
	{"sendMessage", comp_SendMessage},
	{"broadcastMessage",comp_BroadcastMessage},
	{"renderTriangle", grap_RenderTriangle},
	{"renderRectangle", grap_RenderRectangle},
	{"rotateVertex", grap_RotateVertex},
	{"renderTexture", grap_RenderTexture},
	{"renderPolygon", grap_RenderPolygon},
	{"createButton", btn_Create},
	{"createListView", list_Create},
	{"createEditText", edi_Create},
	{"createSound", sound_Create},
	{"addComponent", comp_AddComponent},
	{"removeComponent", comp_RemoveComponent},
	{"intersect", rect_Intersect},
	{"setEnabled", comp_SetEnabled},
	{"setVisible", comp_SetVisible},
	{0, 0}
};

struct KeyNameCode{
	const char* name;
	int code;
};


static const KeyNameCode keyConsts[] = 
{
	{"A", 65},{"B", 66},{"C", 67},{"D", 68},{"E", 69},
	{"F", 70},{"G", 71},{"H", 72},{"I", 73},{"J", 74},
	{"K", 75},{"L", 76},{"M", 77},{"N", 78},{"O", 79},
	{"P", 80},{"Q", 81},{"R", 82},{"S", 83},{"T", 84},
	{"U", 85},{"V", 86},{"W", 87},{"X", 88},{"Y", 89},
	{"Z", 90},

	{"LBUTTON", VK_LBUTTON},{"MBUTTON", VK_MBUTTON},{"RBUTTON", VK_RBUTTON},

	{"ESCAPE", VK_ESCAPE},{"TAB", VK_TAB},{"ENTER", VK_RETURN},
	{"BACKSPACE", VK_BACK},{"SPACE", VK_SPACE},
	{"SHIFT", VK_SHIFT},{"CTRL", VK_CONTROL},{"ALT", VK_MENU},
	{"PAUSE", VK_PAUSE},{"CAPSLOCK", VK_CAPITAL},

	{"PGUP",VK_PRIOR},{"PGDN", VK_NEXT},{"HOME", VK_HOME},
	{"END", VK_END},{"INSERT", VK_INSERT},{"DELETE", VK_DELETE},
	{"UP", VK_UP},{"DOWN", VK_DOWN},{"LEFT", VK_LEFT},{"RIGHT", VK_RIGHT},

	{"VK_0", 0x30},{"VK_1", 0x31},{"VK_2", 0x32},{"VK_3", 0x33},{"VK_4", 0x34},
	{"VK_5", 0x35},{"VK_6", 0x36},{"VK_7", 0x37},{"VK_8", 0x38},{"VK_9", 0x39},

	{"GRAVE", 0xC0},{"MINUS", 0xBD},{"EQUALS", 0xBB},{"BACKSLASH", 0xDC},{"LBRACKET", 0xDB},
	{"RBRACKET", 0xDD},{"SEMICOLON", 0xBA},{"APOSTROPHE", 0xDE},{"COMMA", 0xBC},{"PERIOD", 0xBE},
	{"SLASH", 0xBF},{"NUMLOCK", VK_NUMLOCK},{"SCROLL", VK_SCROLL},

	{"NUMPAD0", 0x60},{"NUMPAD1", 0x61},{"NUMPAD2", 0x62},{"NUMPAD3", 0x63},{"NUMPAD4", 0x64},
	{"NUMPAD5", 0x65},{"NUMPAD6", 0x66},{"NUMPAD7", 0x67},{"NUMPAD8", 0x68},{"NUMPAD9", 0x69},
	{"MULTIPLY", 0x6A},{"DIVIDE", 0x6F},{"ADD", 0x6B},{"SUBTRACT", 0x6D},{"DECIMAL", 0x6E},

	{"F1", 0x70},{"F2", 0x71},{"F3", 0x72},{"F4", 0x73},{"F5", 0x74},{"F6", 0x75},
	{"F7", 0x76},{"F8", 0x77},{"F9", 0x78},{"F10", 0x79},{"F11", 0x7A},{"F12", 0x7B},
	{0, 0}
};

static const KeyNameCode msgConsts[] = {
	{"SETVISIBLE", GCM_SETVISIBLE},
	{"SETENABLED", GCM_SETENABLED},
	{"BUTTON_MOUSEOVER", GCM_BUTTON_MOUSEOVER},
	{"BUTTON_MOUSEOUT", GCM_BUTTON_MOUSEOUT},
	{"BUTTON_CLICK", GCM_BUTTON_CLICK},
	{"LIST_MOUSEOVER", GCM_LIST_MOUSEOVER},
	{"LIST_MOUSEOUT", GCM_LIST_MOUSEOUT},
	{"LIST_CLICK", GCM_LIST_CLICK},
	{"EDITTEXT_CONFIRM", GCM_EDITTEXT_CONFIRM},
	{"EDITTEXT_CHANGE", GCM_EDITTEXT_CHANGE},
	{0, 0}
};

static void RegisterConstantValues(lua_State* L)
{
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "loaded");
	lua_getfield(L, -1, "ymx");

	lua_newtable(L);
	for (const KeyNameCode* knc = keyConsts; knc->name != 0; knc++)
	{
		lua_pushinteger(L, knc->code);
		lua_setfield(L, -2, knc->name);
	}
	lua_setfield(L, -2, "Keys");

	lua_newtable(L);
	for (const KeyNameCode* knc = msgConsts; knc->name != 0; knc++)
	{
		lua_pushinteger(L, knc->code);
		lua_setfield(L, -2, knc->name);
	}
	lua_setfield(L, -2, "Msg");


	lua_newtable(L);
	lua_pushinteger(L, ROOT_COMPONENT_ID);
	lua_setfield(L, -2, "ROOT_ID");
	lua_pushstring(L, ROOT_COMPONENT_NAME);
	lua_setfield(L, -2, "ROOT_NAME");
	lua_setfield(L, -2, "Comp");

	lua_pop(L, 3);
}

YMX_API int luaopen_ymx(lua_State* L)
{
	CreateMetaTable(L, TEXTURE_MT_NAME, textureLib_m);
	CreateMetaTable(L, SPRITE_MT_NAME, spriteLib_m);
	CreateMetaTable(L, FONT_MT_NAME, fontLib_m);
	CreateMetaTable(L, PARTICLE_MT_NAME, particleLib_m);
	CreateMetaTable(L, BUTTON_MT_NAME, buttonLib_m);
	CreateMetaTable(L, LISTVIEW_MT_NAME, listViewLib_m);
	CreateMetaTable(L, EDITTEXT_MT_NAME, editTextLib_m);
	CreateMetaTable(L, SOUND_MT_NAME, soundLib_m);

	luaL_register(L, "ymx", modules);
	RegisterConstantValues(L);
	return 1;
}