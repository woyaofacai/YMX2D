// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>
#include <mmsystem.h>
#include <tchar.h>
#include <time.h>
#include <stdlib.h>

#include "ymx2d.h"
#include "ymxform.h"
#include "ymxparticle.h"
#include "ymxsound.h"


// TODO: reference additional headers your program requires here
extern "C"{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
};
