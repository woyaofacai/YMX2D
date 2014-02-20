#ifndef YMXLUA_FORM_FUNCTION_CLASS_FORM_H
#define YMXLUA_FORM_FUNCTION_CLASS_FORM_H


//BUTTON
extern const char* BUTTON_MT_NAME; 
extern const struct luaL_Reg buttonLib_m[];

int btn_Create(lua_State* L);
int btn_SetTexture(lua_State* L);
int btn_SetTextureRect(lua_State* L);
int btn_Update(lua_State* L);
int btn_Render(lua_State* L);
int btn_SetFont(lua_State* L);
int btn_SetTextColor(lua_State* L);
int btn_SetText(lua_State* L);
int btn_SetZ(lua_State* L);
int btn_GetZ(lua_State* L);
int btn_SetPosition(lua_State* L);
int btn_GetWidth(lua_State* L);
int btn_GetHeight(lua_State* L);
int btn_SetEnabled(lua_State* L);
int btn_IsEnabled(lua_State* L);
int btn_SetVisible(lua_State* L);
int btn_IsVisible(lua_State* L);

//LISTVIEW
extern const char* LISTVIEW_MT_NAME; 
extern const struct luaL_Reg listViewLib_m[];

int list_Create(lua_State* L);
int list_SetTexture(lua_State* L);
int list_SetTextureRect(lua_State* L);
int list_SetFont(lua_State* L);
int list_SetTextColor(lua_State* L);
int list_SetItemSize(lua_State* L);
int list_GetItemSize(lua_State* L);
int list_SetZ(lua_State* L);
int list_GetZ(lua_State* L);
int list_Update(lua_State* L);
int list_Render(lua_State* L);
int list_SetPosition(lua_State* L);
int list_AddItem(lua_State* L);
int list_RemoveItem(lua_State* L);
int list_SetTextAlignment(lua_State* L);
int list_EnableKeyboard(lua_State* L);
int list_EnableMouse(lua_State* L);
int list_SetFocusItem(lua_State* L);
int list_GetFocusItem(lua_State* L);
int list_SetConfirmKey(lua_State* L);
int list_SetUpKey(lua_State* L);
int list_SetDownKey(lua_State* L);
int list_SetVisible(lua_State* L);
int list_IsVisible(lua_State* L);
int list_Release(lua_State* L);

extern const char* EDITTEXT_MT_NAME; 
extern const struct luaL_Reg editTextLib_m[];

int edi_Create(lua_State* L);
int edi_GetZ(lua_State* L);
int edi_SetZ(lua_State* L);
int edi_SetFont(lua_State* L);
int edi_SetTextColor(lua_State* L);
int edi_Update(lua_State* L);
int edi_Render(lua_State* L);
int edi_SetPosition(lua_State* L);
int edi_SetTextAlignment(lua_State* L);
int edi_SetVisible(lua_State* L);
int edi_SetFocus(lua_State* L);
int edi_IsVisible(lua_State* L);
int edi_IsFocused(lua_State* L);
int edi_Release(lua_State* L);
int edi_GetText(lua_State* L);
int edi_SetText(lua_State* L);
int edi_SetCursor(lua_State* L);
int edi_SetConfirmKey(lua_State* L);

#endif


