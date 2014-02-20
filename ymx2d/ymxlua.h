#include "ymx2d.h"
#include <map>

#ifndef YMX2D_FUNCTION_CLASS_LUA_H
#define YMX2D_FUNCTION_CLASS_LUA_H

class YMX2D_API YmxLuaConfig;
class YMX2D_API YmxLuaComponent;

bool Initialize_LuaState();
void Release_LuaState();
//YMX2D_API YmxLuaConfig* getLuaConfig();
YMX2D_API YmxLuaComponent* CreateLuaComponentFromFile(const char* fileName, bool bInit = false);
YMX2D_API bool LoadAllLuaComponentsFromFile();


class YMX2D_API YmxLuaConfig
{
	friend class YmxGame;
public:
	~YmxLuaConfig();
	//static YmxLuaConfig* GetInstance();
	
	bool LoadLuaFile(const char* filename);
	bool GetBool(char* key, bool* value);
	bool GetFloat(char* key, float* value);
	bool GetInt(char* key, int* value);
	bool GetString(char* key, TCHAR* buf, int bufSize, size_t* len);
	bool GetStringA(char* key, char* buf, int bufSize, size_t* len);
	bool GetStringW(char* key, wchar_t* buf, int bufSize, size_t* len);
	bool GetIntArray(char* key, int* buf, size_t* len);
	bool GetBoolArray(char* key, bool* buf, size_t* len);
	bool GetFloatArray(char* key, float* buf, size_t* len);
private:
	YmxLuaConfig();
	YmxLuaConfig(YmxLuaConfig&);
	bool _GetValueFromTable(char* key);
	static YmxLuaConfig* _GetInstance();
	bool _Initialize();
	std::map<char*, float> floatMap;
	std::map<char*, int> intMap;
	std::map<char*, bool> boolMap;
};


class YmxLuaComponent : public YmxComponent
{
	friend YmxLuaComponent* CreateLuaComponentFromFile(const char* fileName, bool bInit);
public:
	YmxLuaComponent(int id, LPYMXCOMPONENT pParentComponent, char* name, UINT order);
	~YmxLuaComponent(); 
	virtual bool OnMessage(int msgid, DWORD notifyCode, PVOID param);
protected:
	virtual void Initialize(); 
	virtual void LoadContent();
	virtual void Render(YmxGraphics* g);
	virtual bool Update(float delta);
	virtual void UnloadContent();
	bool _ExistFunc(const char* funcName);
private:
	char* m_CompoName;
	bool m_bValid;
	bool m_bExistUpdateFunc;
	bool m_bExistRenderFunc;
	bool m_bExistOnMessageFunc;
};

#endif