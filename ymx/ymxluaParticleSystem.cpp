#include "stdafx.h"
#include "ymxluautil.h"
#include "ymxluacore.h"


const char* PARTICLE_MT_NAME = "ymxlua.particle";
const struct luaL_Reg particleLib_m[] = {
	{"initialize", par_Initialize},
	{"update", par_Update},
	{"render", par_Render},
	{"release", par_Release},
	{"setAttribute", par_SetAttribute},
	{"getAttribute", par_GetAttribute},
	{"stop", par_Stop},
	{"restart", par_Restart},
	{"setTexture", par_SetTexture},
	{"getZ", par_GetZ},
	{"setZ", par_SetZ},
	{"setPosition", par_SetPosition},
	{0, 0}
};

struct ParAttrNameAndType
{
	const char* name;
	PARTICLE_ATTRIBUTE type;
};

static const ParAttrNameAndType parAttrBoolTypes[] = {
	{"FROM_NOTING", PAR_FROM_NOTING},
	{"HIGHLIGHT", PAR_HIGHLIGHT},
	{"COLOR_GRADIENT", PAR_COLOR_GRADIENT},
	{"SIZE_GRADIENT", PAR_SIZE_GRADIENT},
	{"EXPLODE_CONTINUOUS", PAR_EXPLODE_CONTINUOUS},
	{"COMIT_BURN_ONLY_ONMOVING", PAR_COMIT_BURN_ONLY_ONMOVING},
	{0, PAR_FROM_NOTING}
};

static const ParAttrNameAndType parAttrIntTypes[] = 
{
	{"MAX_COUNT", PAR_MAX_COUNT},
	{"START_COLOR_A", PAR_START_COLOR_A},
	{"START_COLOR_R", PAR_START_COLOR_R},
	{"START_COLOR_G", PAR_START_COLOR_G},
	{"START_COLOR_B", PAR_START_COLOR_B},
	{"END_COLOR_A", PAR_END_COLOR_A},
	{"END_COLOR_R", PAR_END_COLOR_R},
	{"END_COLOR_G", PAR_END_COLOR_G},
	{"END_COLOR_B", PAR_END_COLOR_B},
	{"COLOR_MIN_R", PAR_COLOR_MIN_R},
	{"COLOR_MIN_G", PAR_COLOR_MIN_G},
	{"COLOR_MIN_B", PAR_COLOR_MIN_B},
	{"COLOR_MIN_A", PAR_COLOR_MIN_A},
	{"COLOR_MAX_R", PAR_COLOR_MAX_R},
	{"COLOR_MAX_G", PAR_COLOR_MAX_G},
	{"COLOR_MAX_B", PAR_COLOR_MAX_B},
	{"COLOR_MAX_A", PAR_COLOR_MAX_A},
	{"COLOR_R", PAR_COLOR_R},
	{"COLOR_G", PAR_COLOR_G},
	{"COLOR_B", PAR_COLOR_B},
	{"COLOR_A", PAR_COLOR_A},
	{"COMET_DENSITY", PAR_COMET_DENSITY},
	{0, PAR_MAX_SIZE}
};

static const ParAttrNameAndType parAttrFloatTypes[] = 
{
	{"MIN_SIZE", PAR_MIN_SIZE},
	{"MAX_SIZE", PAR_MAX_SIZE},
	{"SIZE", PAR_SIZE},
	{"MIN_VELOCITY_X", PAR_MIN_VELOCITY_X},
	{"MIN_VELOCITY_Y", PAR_MIN_VELOCITY_Y},
	{"MAX_VELOCITY_X", PAR_MAX_VELOCITY_X},
	{"MAX_VELOCITY_Y", PAR_MAX_VELOCITY_Y},
	{"VELOCITY_X", PAR_VELOCITY_X},
	{"VELOCITY_Y", PAR_VELOCITY_Y},
	{"Z", PAR_Z},
	{"POS_X", PAR_POS_X},
	{"POS_Y", PAR_POS_Y},
	{"LIFE", PAR_LIFE},
	{"LIFE_VARIANT", PAR_LIFE_VARIANT},
	{"VELOCITY", PAR_VELOCITY},
	{"VELOCITY_VARIANT", PAR_VELOCITY_VARIANT},
	{"ACCELERATION", PAR_ACCELERATION},
	{"ACCELERATION_VARIANT", PAR_ACCELERATION_VARIANT},
	{"START_SIZE", PAR_START_SIZE},
	{"END_SIZE", PAR_END_SIZE},
	{"SIZE_VARIANT", PAR_SIZE_VARIANT},
	{"FIELD_ACCELERATION_X", PAR_FIELD_ACCELERATION_X},
	{"FIELD_ACCELERATION_Y", PAR_FIELD_ACCELERATION_Y},
	{"EXPLODE_START_ANGLE", PAR_EXPLODE_START_ANGLE},
	{"EXPLODE_END_ANGLE", PAR_EXPLODE_END_ANGLE},
	{"VELOCITY_X_VARIANT", PAR_VELOCITY_X_VARIANT},
	{"VELOCITY_Y_VARIANT", PAR_VELOCITY_Y_VARIANT},
	{"FLAME_HEIGHT", PAR_FLAME_HEIGHT},
	{"ACCELERATION_X_VARIANT", PAR_ACCELERATION_X_VARIANT},
	{"ACCELERATION_Y_VARIANT", PAR_ACCELERATION_Y_VARIANT},
	{"FLAME_START_X", PAR_FLAME_START_X},
	{"FLAME_START_Y", PAR_FLAME_START_Y},
	{"FLAME_END_X", PAR_FLAME_END_X},
	{"FLAME_END_Y", PAR_FLAME_END_Y},
	{0, PAR_MAX_SIZE}
};

static void SetAttributesFromLuaArg(YmxParticleSystem* parsys, lua_State* L, int idx)
{
	if(!lua_istable(L, idx)) return;

	for (const ParAttrNameAndType *pnt = parAttrIntTypes; pnt->name != 0; pnt++)
	{
		lua_getfield(L, idx, pnt->name);
		if(lua_isnumber(L, -1))
		{
			int val = (int)lua_tointeger(L, -1);
			parsys->SetAttribute(pnt->type, val);
		}
		lua_pop(L, 1);
	}

	for (const ParAttrNameAndType *pnt = parAttrFloatTypes; pnt->name != 0; pnt++)
	{
		lua_getfield(L, idx, pnt->name);
		if(lua_isnumber(L, -1))
		{
			float val = (float)lua_tonumber(L, -1);
			parsys->SetAttribute(pnt->type, val);
		}
		lua_pop(L, 1);
	}

	for (const ParAttrNameAndType* pnt = parAttrBoolTypes; pnt->name != 0; pnt++)
	{
		lua_getfield(L, idx, pnt->name);
		if(!lua_isnil(L, -1))
		{
			bool val = (bool)lua_toboolean(L, -1);
			parsys->SetAttribute(pnt->type, val);
		}
		lua_pop(L, 1);
	}
}

int par_Create(lua_State* L)
{
	const char* parType = luaL_checkstring(L, 1);
	LuaProxy* pTexProxy = (LuaProxy*)luaL_checkudata(L, 2, TEXTURE_MT_NAME);

	LPYMXTEXTURE pTex = NULL;
	if(pTexProxy != NULL) 
		pTex = (LPYMXTEXTURE)pTexProxy->instance;

	LPYMXCOMPONENT pComponent = GetComponentFromLuaArg(L, 4);

	YmxParticleSystem* parsys = NULL;

	if(pComponent == NULL)
	{
		if(strcmp(parType, "snow")==0)
			parsys = GetGameObject()->CreateParticleSystem(PAR_TYPE_SNOW, pTex, 100);
		else if(strcmp(parType, "flame")==0)
			parsys = GetGameObject()->CreateParticleSystem(PAR_TYPE_FLAME, pTex, 100);
		else if(strcmp(parType, "explode")==0)
			parsys = GetGameObject()->CreateParticleSystem(PAR_TYPE_EXPLODE, pTex, 100);
		else if(strcmp(parType, "comet")==0)
			parsys = GetGameObject()->CreateParticleSystem(PAR_TYPE_COMET, pTex, 100);
	}
	else 
	{
		if(strcmp(parType, "snow")==0)
			parsys = pComponent->CreateParticleSystem(PAR_TYPE_SNOW, pTex, 100);
		else if(strcmp(parType, "flame")==0)
			parsys = pComponent->CreateParticleSystem(PAR_TYPE_FLAME, pTex, 100);
		else if(strcmp(parType, "explode")==0)
			parsys = pComponent->CreateParticleSystem(PAR_TYPE_EXPLODE, pTex, 100);
		else if(strcmp(parType, "comet")==0)
			parsys = pComponent->CreateParticleSystem(PAR_TYPE_COMET, pTex, 100);
	}

	if(parsys == NULL) {
		lua_pushnil(L);
		return 1;
	}

	SetAttributesFromLuaArg(parsys, L, 3);

	LuaProxy* pParsysProxy = (LuaProxy*)lua_newuserdata(L, sizeof(LuaProxy));
	pParsysProxy->instance = parsys;
	pParsysProxy->type = PARTICLE_PROXY;

	luaL_getmetatable(L, PARTICLE_MT_NAME);
	lua_setmetatable(L, -2);

	return 1;
}

int par_Initialize(lua_State* L)
{
	LuaProxy* pParsysProxy = (LuaProxy*)luaL_checkudata(L, 1, PARTICLE_MT_NAME);
	YmxParticleSystem* parsys = (YmxParticleSystem*)pParsysProxy->instance;
	if(parsys != NULL && parsys->Initialize())
	{
		lua_pushboolean(L, 1);
	}
	else {
		lua_pushboolean(L, 0);
	}
	return 1;
}

int par_Update(lua_State* L)
{
	LuaProxy* pParsysProxy = (LuaProxy*)luaL_checkudata(L, 1, PARTICLE_MT_NAME);
	YmxParticleSystem* parsys = (YmxParticleSystem*)pParsysProxy->instance;

	float delta = (float)luaL_checknumber(L, 2);
	parsys->Update(delta);

	return 0;
}

int par_Render(lua_State* L)
{
	LuaProxy* pParsysProxy = (LuaProxy*)luaL_checkudata(L, 1, PARTICLE_MT_NAME);
	YmxParticleSystem* parsys = (YmxParticleSystem*)pParsysProxy->instance;

	YmxGraphics* g = (YmxGraphics*)lua_touserdata(L, 2);

	if(g != NULL)
		parsys->Render(g);

	return 0;
}

int par_Release(lua_State* L)
{
	LuaProxy* pParsysProxy = (LuaProxy*)luaL_checkudata(L, 1, PARTICLE_MT_NAME);
	YmxParticleSystem* parsys = (YmxParticleSystem*)pParsysProxy->instance;

	parsys->Release();

	pParsysProxy->instance = NULL;
	return 0;
}

int par_SetAttribute(lua_State* L)
{
	LuaProxy* pParsysProxy = (LuaProxy*)luaL_checkudata(L, 1, PARTICLE_MT_NAME);
	YmxParticleSystem* parsys = (YmxParticleSystem*)pParsysProxy->instance;

	SetAttributesFromLuaArg(parsys, L, 2);

	return 0;
}

int par_GetAttribute(lua_State* L)
{
	LuaProxy* pParsysProxy = (LuaProxy*)luaL_checkudata(L, 1, PARTICLE_MT_NAME);
	YmxParticleSystem* parsys = (YmxParticleSystem*)pParsysProxy->instance;
	
	if(!lua_isstring(L, 2))
	{
		lua_pushnil(L);
		return 1;
	}

	const char* attr = lua_tostring(L, 2);

	for (const ParAttrNameAndType *pnt = parAttrFloatTypes; pnt->name != 0; pnt++)
	{
		if(_stricmp(pnt->name, attr) == 0)
		{
			float val;
			if(parsys->GetAttribute(pnt->type, &val))
			{
				lua_pushnumber(L, val);
				return 1;
			}
			else break;
		}
	}

	for (const ParAttrNameAndType *pnt = parAttrIntTypes; pnt->name != 0; pnt++)
	{
		if(_stricmp(pnt->name, attr) == 0)
		{
			int val;
			if(parsys->GetAttribute(pnt->type, &val))
			{
				lua_pushinteger(L, val);
				return 1;
			}
			else break;
		}
	}

	for (const ParAttrNameAndType* pnt = parAttrBoolTypes; pnt->name != 0; pnt++)
	{
		if(_stricmp(pnt->name, attr) == 0)
		{
			bool val;
			if(parsys->GetAttribute(pnt->type, &val))
			{
				if(val) 
					lua_pushboolean(L, 1);
				else 
					lua_pushboolean(L, 0);
				return 1;
			}
			else break;
		}
	}
	
	lua_pushnil(L);
	return 1;
}

int par_Stop(lua_State* L)
{
	LuaProxy* pParsysProxy = (LuaProxy*)luaL_checkudata(L, 1, PARTICLE_MT_NAME);
	YmxParticleSystem* parsys = (YmxParticleSystem*)pParsysProxy->instance;
	parsys->Stop();
	return 0;
}

int par_Restart(lua_State* L)
{
	LuaProxy* pParsysProxy = (LuaProxy*)luaL_checkudata(L, 1, PARTICLE_MT_NAME);
	YmxParticleSystem* parsys = (YmxParticleSystem*)pParsysProxy->instance;
	parsys->Restart();
	return 0;
}

int par_SetTexture(lua_State* L)
{
	if(lua_gettop(L) < 2) return 0;

	LuaProxy* pParsysProxy = (LuaProxy*)luaL_checkudata(L, 1, PARTICLE_MT_NAME);
	YmxParticleSystem* parsys = (YmxParticleSystem*)pParsysProxy->instance;

	if(lua_isnil(L, 2))
	{
		parsys->SetTexture(NULL);
	}
	else {
		LuaProxy* pTexProxy = (LuaProxy*)luaL_checkudata(L, 2, TEXTURE_MT_NAME);
		LPYMXTEXTURE pTexture = (LPYMXTEXTURE)pTexProxy->instance;
		parsys->SetTexture(pTexture);
	}
	return 0;
}

int par_SetZ(lua_State* L)
{
	LuaProxy* pParsysProxy = (LuaProxy*)luaL_checkudata(L, 1, PARTICLE_MT_NAME);
	YmxParticleSystem* parsys = (YmxParticleSystem*)pParsysProxy->instance;

	if(!lua_isnumber(L, 2))
		return 0;

	float z = (float)lua_tonumber(L, 2);
	parsys->SetZ(z);
	return 0;
}

int par_GetZ(lua_State* L)
{
	LuaProxy* pParsysProxy = (LuaProxy*)luaL_checkudata(L, 1, PARTICLE_MT_NAME);
	YmxParticleSystem* parsys = (YmxParticleSystem*)pParsysProxy->instance;

	lua_pushnumber(L, parsys->GetZ());
	return 1;
}

int par_SetPosition(lua_State* L)
{
	LuaProxy* pParsysProxy = (LuaProxy*)luaL_checkudata(L, 1, PARTICLE_MT_NAME);
	YmxParticleSystem* parsys = (YmxParticleSystem*)pParsysProxy->instance;

	if(!lua_tonumber(L, 2) || !lua_tonumber(L, 3))
		return 0;

	float x = (float)lua_tonumber(L, 2);
	float y = (float)lua_tonumber(L, 3);

	parsys->SetPosition(x, y);

	return 0;
}




