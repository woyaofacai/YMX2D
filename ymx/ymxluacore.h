#ifndef YMXLUA_CORE_FUNCTION_CLASS_FORM_H
#define YMXLUA_CORE_FUNCTION_CLASS_FORM_H



//CORE FUNCTIONS
int input_GetMousePos(lua_State* L);
int input_SetMousePos(lua_State* L);
int input_KeyDown(lua_State* L);
int input_KeyUp(lua_State* L);
int input_GetKeyState(lua_State* L);
int input_GetInputKey(lua_State* L);
int input_GetChar(lua_State* L);
int comp_SendMessage(lua_State* L);
int comp_BroadcastMessage(lua_State* L);
int comp_AddComponent(lua_State* L);
int comp_RemoveComponent(lua_State* L);
int comp_SetEnabled(lua_State* L);
int comp_SetVisible(lua_State* L);

int grap_RenderTriangle(lua_State* L); 
int grap_RenderRectangle(lua_State* L);

int grap_RotateVertex(lua_State* L);
int grap_RenderTexture(lua_State* L);
int grap_RenderPolygon(lua_State* L);

//ERROR 
int err_MessageBox(lua_State* L);
int err_RecordError(lua_State* L);
int err_Log(lua_State* L);

//WINDOWS
int win_GetWindowWidth(lua_State* L);
int win_GetWindowHeight(lua_State* L);

//RECT
int rect_Intersect(lua_State* L);

//TEXTURE
extern const char* TEXTURE_MT_NAME; 
extern const struct luaL_Reg textureLib_m[];
int tex_Load(lua_State* L);
int tex_GetWidth(lua_State* L); 
int tex_GetHeight(lua_State* L);
int tex_SetAlpha(lua_State* L);
int tex_Release(lua_State* L);

//SPRITE
extern const char* SPRITE_MT_NAME;
extern const struct luaL_Reg spriteLib_m[];

int spr_Create(lua_State* L);
int spr_Render(lua_State* L);
int spr_SetTextureRect(lua_State* L);
int spr_GetWidth(lua_State* L);
int spr_GetHeight(lua_State* L);
int spr_SetZ(lua_State* L);
int spr_GetZ(lua_State* L);
int spr_SetHotSpot(lua_State* L);
int spr_Release(lua_State* L);

//FONT
extern const char* FONT_MT_NAME;
extern const struct luaL_Reg fontLib_m[];

int fnt_Load(lua_State* L);
int fnt_SetText(lua_State* L);
int fnt_GetText(lua_State* L);
int fnt_Render(lua_State* L);
int fnt_SetZ(lua_State* L);
int fnt_GetZ(lua_State* L);
int fnt_GetColor(lua_State* L);
int fnt_SetColor(lua_State* L);
int fnt_SetAttribute(lua_State* L);
int fnt_SetLineSpacing(lua_State* L);
int fnt_SetCharSpacing(lua_State* L);
int fnt_GetLineSpacing(lua_State* L);
int fnt_GetCharSpacing(lua_State* L);
int fnt_GetWidth(lua_State* L);
int fnt_GetHeight(lua_State* L);
int fnt_Release(lua_State* L);

//PARTICLE YMXTEM
extern const char* PARTICLE_MT_NAME;
extern const struct luaL_Reg particleLib_m[];

int par_Create(lua_State* L);
int par_Initialize(lua_State* L);
int par_Update(lua_State* L);
int par_Render(lua_State* L);
int par_Release(lua_State* L);
int par_SetAttribute(lua_State* L);
int par_GetAttribute(lua_State* L);
int par_Stop(lua_State* L);
int par_Restart(lua_State* L);
int par_SetTexture(lua_State* L);
int par_GetZ(lua_State* L);
int par_SetZ(lua_State* L);
int par_SetPosition(lua_State* L);


//SOUND
extern const char* SOUND_MT_NAME;
extern const struct luaL_Reg soundLib_m[];

int sound_Create(lua_State* L);
int sound_Play(lua_State* L);
int sound_Release(lua_State* L);
int sound_Pause(lua_State* L);
int sound_Stop(lua_State* L);
int sound_SetFrequency(lua_State* L);
int sound_SetPan(lua_State* L);
int sound_SetVolume(lua_State* L);
int sound_GetFrequency(lua_State* L);
int sound_GetPan(lua_State* L);
int sound_GetVolume(lua_State* L);
int sound_IsPlaying(lua_State* L);
int sound_IsLooping(lua_State* L);


#endif


