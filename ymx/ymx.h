#ifdef YMX_EXPORTS
#define YMX_API __declspec(dllexport)
#else
#define YMX_API __declspec(dllimport)
#endif

YMX_API int luaopen_ymx(lua_State* L);