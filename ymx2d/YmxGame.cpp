#include "stdafx.h"
#include "ymx2d.h"
#include "ymxparticle.h"
#include "ymxvertex.h"
#include "ymxlua.h"
#include "ymxsound.h"

YMX2D_API YmxGame* CreateGame()
{
	YmxGame* game = YmxGame::_GetInstance();
	game->SetAttribute(GAME_WINDOW_TITLE, TEXT("YMX2D GAME"));
	game->SetAttribute(GAME_LOG_FILE, TEXT("log.txt"));
	game->SetAttribute(GAME_WINDOW_WIDTH, 800);
	game->SetAttribute(GAME_WINDOW_HEIGHT, 600);
	//game->SetAttribute(SUPPORT_LUA_CONFIG, false);

	//Create Lua State
	if(!Initialize_LuaState())
		game->m_bSupportLuaConfig = false;

	return game;
}

YMX2D_API YmxGame* GetGameObject()
{
	return YmxGame::_GetInstance();
}

YmxGraphics* YmxGame::graphics = NULL;

YmxGame::YmxGame()
{
	hInstance = (HINSTANCE)GetModuleHandle(NULL);

	graphics = new YmxGraphics;
	graphics->m_game = this;

	renderFunction = NULL;
	updateFunction = NULL;

	m_pRootComponent = NULL;

	m_hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	m_hCursor = LoadCursor(NULL, IDC_ARROW);

	m_Delta = 0.02f;

	m_bFullScreen = false;
	m_bActive = true;

	m_LuaConfig = NULL;
	m_bSupportLuaConfig = true;
	m_bLoadLuaComponents = true;
	m_bShowCursor = true;
	m_bLogError = true;

	srand(time(NULL));
}

YmxGame::YmxGame(YmxGame&)
{

}

YmxGame::~YmxGame()
{
	
}

YmxGame* YmxGame::_GetInstance()
{
	static YmxGame ymxGame;
	return &ymxGame;
}

bool YmxGame::Start()
{
	// call the Initalize method of all components
	m_pRootComponent->_LoadContent();
	m_pRootComponent->_Initialize();

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	float currentTime, delta;

	static float lastTime = (float)timeGetTime();

	while(true) {
		if(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
			if(msg.message == WM_QUIT) {
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			currentTime = (float)timeGetTime();
			delta = (currentTime - lastTime) * 0.001f;

			if(delta > m_Delta) {

				if(m_bActive)
				{
					if(updateFunction != NULL)
					{
						if(!updateFunction(delta))
							return true;
					}
					if(!m_pRootComponent->_Update(delta))
						return true;

					_Render();

					_Input_Clear();
				}
				
				lastTime = currentTime;
			}
			else if(m_Delta - delta > 0.001f){
				Sleep(1);
			}
		}
	}
	return true;
}

/*
	YmxGame::Initialize's objective:
	1, Create Windows
	2, Initialize the DirectX(DirectSound And D3D)
	3, Add Default Root Component And Load All Lua Component if needed.
*/
bool YmxGame::Initialize()
{
	HWND hwnd;               /* This is the handle for our window */
	//MSG msg;            /* Here messages to the application are saved */
	WNDCLASSEX wincl;        /* Data structure for the windowclass */

	TCHAR szClassName[] = TEXT("WindowsApp");
	/* The Window structure */
	wincl.hInstance = hInstance;
	wincl.lpszClassName = szClassName;
	wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
	wincl.style = CS_HREDRAW | CS_VREDRAW;                 /* Catch double-clicks */
	wincl.cbSize = sizeof (WNDCLASSEX);

	/* Use default icon and mouse-pointer */

	wincl.hIcon = m_hIcon;
	wincl.hIconSm = m_hIcon;
	wincl.hCursor = m_hCursor;
	wincl.lpszMenuName = NULL;                 /* No menu */
	wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
	wincl.cbWndExtra = 0;                      /* structure or the window instance */
	/* Use Windows's default color as the background of the window */
	wincl.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);

	/* Register the window class, and if it fails quit the program */
	if (!RegisterClassEx (&wincl))
		return 0;

	/* The class is registered, let's create the program*/

	int windowWidth = (int)graphics->m_screenWidth + GetSystemMetrics(SM_CXSIZEFRAME) + GetSystemMetrics(SM_CXSIZEFRAME);
	int windowHeight = (int)graphics->m_screenHeight + GetSystemMetrics(SM_CYCAPTION) + 2* GetSystemMetrics(SM_CYSIZEFRAME);

	if(m_bFullScreen)
	{
		hwnd = CreateWindowEx(WS_EX_TOPMOST, szClassName, m_szWindowTitle, WS_POPUP|WS_VISIBLE,
			0, 0, 0, 0,
			NULL, NULL, hInstance, NULL);
	}
	else {
		hwnd = CreateWindowEx (
			0,                   /* Extended possibilites for variation */
			szClassName,         /* Classname */
			m_szWindowTitle,       /* Title Text */
			WS_POPUP|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_VISIBLE, /* default window */
			(GetSystemMetrics(SM_CXSCREEN)-windowWidth)/2,       /* Windows decides the position */
			(GetSystemMetrics(SM_CYSCREEN)-windowHeight)/2,       /* where the window ends up on the screen */
			windowWidth,                 /* The programs width */
			windowHeight,                 /* and height in pixels */
			HWND_DESKTOP,        /* The window is a child-window to desktop */
			NULL,                /* No menu */
			hInstance,			 /* Program Instance handler */
			NULL                 /* No Window Creation data */
			);
	}

	/* Make the window visible on the screen */
	ShowWindow (hwnd, SW_SHOW);
	UpdateWindow(hwnd);

	::hwnd = hwnd;

	// Initialize The DirectSound Component.
	if(!Initialize_DSound())
	{
		RecordError(TEXT("DirectX Error"), TEXT("Cannot Initialize DirectSound!"));
		return false;
	}

	// Initialize Direct3D
	if(!graphics->_D3D_Initialize())
	{
		return false;
	}

	//Add The Root Component
	m_pRootComponent = new YmxComponent(ROOT_COMPONENT_ID, NULL, ROOT_COMPONENT_NAME);
	m_pRootComponent->SetBoundingBox(YMXRECT(0, 0, GetWindowWidth(), GetWindowHeight()));

	// load and all the luaComponents from file, if need load automatically
	if(m_bLoadLuaComponents)
		LoadAllLuaComponentsFromFile();

	return true;
}

LRESULT CALLBACK YmxGame::WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT rect;
	YmxGame* game = GetGameObject();
	switch( msg )
	{
	case WM_DESTROY:
		::PostQuitMessage(0);
		break;
	case WM_ACTIVATE:
		if(LOWORD(wParam) == WA_INACTIVE)
			game->m_bActive = false;
		else 
			game->m_bActive = true;
		break;
	case WM_SIZE:
		break;
	case WM_MOUSEMOVE:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_KEYUP:
	case WM_KEYDOWN:
	case WM_CHAR:
		if(game->_Handle_Input(msg, wParam, lParam))
			return 0;
		break;
	}
	return ::DefWindowProc(hwnd, msg, wParam, lParam);
}

void YmxGame::SetRenderFunction(RENDER_FUNCTION renderFunc)
{
	renderFunction = renderFunc;
}

void YmxGame::SetUpdateFunction(UPDATE_FUNCTION updateFunc)
{
	updateFunction = updateFunc;
}

void YmxGame::_Render()
{
	graphics->_Render_Begin();
	
	if(renderFunction != NULL)
	{
		renderFunction(graphics);
	}
	
	m_pRootComponent->_Render(graphics);
	graphics->_Render_End();
}


void YmxGame::SetAttribute(YMXGAME_ATTRIBUTE attr, PCTSTR value)
{
	switch(attr)
	{
	case GAME_WINDOW_TITLE:
		lstrcpyn(m_szWindowTitle, value, 200);
		if(hwnd != NULL)
		{
			SetWindowText(hwnd, m_szWindowTitle);
		}
		break;
	case GAME_LOG_FILE:
		lstrcpyn(m_szLogFile, value, 200);
		break;
	case GAME_ICON_RESOURCE:
		m_hIcon = LoadIcon(hInstance, value);
		break;
	case GAME_CURSOR_RESOURCE:
		m_hCursor = LoadCursor(hInstance, value);
		if(hwnd != NULL)
		{
			DestroyCursor(SetCursor(m_hCursor));
		}
		break;
	default:
		break;
	}
}

void YmxGame::SetAttribute(YMXGAME_ATTRIBUTE attr, int value)
{
	switch(attr)
	{
	case GAME_WINDOW_WIDTH:
		graphics->m_screenWidth = (float)value;
		break;
	case GAME_WINDOW_HEIGHT:
		graphics->m_screenHeight = (float)value;
		break;
	case GAME_UPDATE_FPS:
		value = max(1, min(value, 300));
		m_Delta = 1.0f / value;
		break;
	default:
		break;
	}
}

void YmxGame::SetAttribute(YMXGAME_ATTRIBUTE attr, bool value)
{
	switch(attr)
	{
	case GAME_PARTICLE_ACCELERATE:
		graphics->m_bFastParticle = value;
		break;
	case GAME_FULL_SCREEN:
		m_bFullScreen = value;
		break;
	case GAME_LOAD_LUACOMPONENTS:
		m_bLoadLuaComponents = value;
		break;
	case GAME_SHOW_CURSOR:
		if(m_bShowCursor && !value)
			ShowCursor(false);
		else if(!m_bShowCursor && value)
			ShowCursor(true);
		m_bShowCursor = value;
		break;
	case GAME_LOG_ERROR:
		m_bLogError = value;
		break;
	}
}

LPYMXTEXTURE YmxGame::LoadTexture(PCTSTR fileName)
{
	return YmxTexture::LoadTexture(fileName);
}

void YmxGame::_Input_Clear()
{
	memset(m_keys, 0, sizeof(int) * 256);
	m_lastChar = 0;
	m_lastKey = 0;
}

bool YmxGame::_Handle_Input(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_MOUSEMOVE:
		m_mousePosX = LOWORD(lParam);
		m_mousePosY = HIWORD(lParam);
		return true;
	case WM_LBUTTONDOWN:
		m_keys[VK_LBUTTON] |= 0x01;
		return true;
	case WM_LBUTTONUP:
		m_keys[VK_LBUTTON] |= 0x02;
		return true;
	case WM_RBUTTONDOWN:
		m_keys[VK_RBUTTON] |= 0x01;
		return true;
	case WM_RBUTTONUP:
		m_keys[VK_RBUTTON] |= 0x02;
		return true;
	case WM_KEYDOWN:
		m_lastKey = wParam;
		m_keys[wParam] |= 0x01;
		return true;
	case WM_KEYUP:
		m_keys[wParam] |= 0x02;
		return true;
	case WM_CHAR:
		m_lastChar = (TCHAR)wParam;
		return true;
	default:
		break;
	}
	return false;
}

void YmxGame::GetMousePos(float* x, float *y)
{
	*x = m_mousePosX;
	*y = m_mousePosY;
}

void YmxGame::SetMousePos(float x, float y)
{
	POINT pt;
	pt.x = x;
	pt.y = y;
	ClientToScreen(hwnd, (LPPOINT)&pt);
	SetCursorPos(pt.x, pt.y);
	
	m_mousePosX = x;
	m_mousePosY = y;
}

bool YmxGame::KeyDown(int key)
{
	return ( m_keys[key] & 0x01) != 0 ;
}

bool YmxGame::KeyUp(int key)
{
	return ( m_keys[key] & 0x02 ) != 0;
}

int YmxGame::GetInputKey()
{
	return m_lastKey;
}

TCHAR YmxGame::GetInputChar()
{
	return m_lastChar;
}

bool YmxGame::GetKeyState(int key)
{
	return ((::GetKeyState(key) & 0x8000) != 0);
}

float YmxGame::GetWindowWidth()
{
	return graphics->GetWidth();
}

float YmxGame::GetWindowHeight()
{
	return graphics->GetHeight();
}

LPYMXFONT YmxGame::CreateFont(PCTSTR szFntFileName)
{
	return YmxFont::Load_From_File(szFntFileName, NULL);
}

LPYMXSPRITE YmxGame::CreateSprite(LPYMXTEXTURE pTexture)
{
	YMXRECT textureRect(0, 0, pTexture->GetWidth(), pTexture->GetHeight());
	return CreateSprite(pTexture, textureRect);
}

LPYMXSPRITE YmxGame::CreateSprite(LPYMXTEXTURE pTexture, YMXRECT& textureRect)
{
	LPYMXSPRITE sprite = new YmxSprite(pTexture, textureRect, NULL);
	return sprite;
}

bool YmxGame::AddComponent(LPYMXCOMPONENT pComponent, bool bInit /* = false */)
{
	return m_pRootComponent->AddComponent(pComponent, bInit);
}

void YmxGame::Release()
{
	m_pRootComponent->_Release();

	// release DirectSound
	Release_DSound();

	// release lua
	delete m_LuaConfig;
	Release_LuaState();

	// release graphics
	delete graphics;
}

LPYMXCOMPONENT YmxGame::GetComponentById(int id)
{
	return m_pRootComponent->GetComponentById(id);
}

LPYMXCOMPONENT YmxGame::GetComponentByName(const char* name)
{
	return m_pRootComponent->GetComponentByName(name);
}

bool YmxGame::_IsSupporFVFPSize()
{
	return graphics->m_bSupportPSize;
}

float YmxGame::GetUpdateDelta()
{
	return m_Delta;
}

LPPARTICLEYMXTEM YmxGame::CreateParticleSystem(PARTICLESYSTEM_TYPE parType, LPYMXTEXTURE pTexture, int maxCount)
{
	return YmxParticleSystem::CreateParticleSystem(NULL, parType, pTexture, maxCount);
}

LPYMXSOUND YmxGame::CreateSound(PTSTR filename)
{
	TCHAR* ext = _tcsrchr(filename, TEXT('.'));
	if(ext == NULL)
		return NULL;
	
	LPYMXSOUND pSound = NULL;

	// check the extension of sound file, only wave can be loaded
	if(lstrcmpi(ext, TEXT(".wav")) == 0)
	{
		pSound = CreateWaveSoundFromFile(filename);
	}
	else
	{
		pSound = NULL;
		RecordError(TEXT("Sound Error"), TEXT("%s cannot be loaded. Only wav format is supported"), filename);
	}
	return pSound;
}

YmxLuaConfig* YmxGame::GetLuaConfig()
{
	if(!m_bSupportLuaConfig)
		return NULL;

	if(m_LuaConfig != NULL)
		return m_LuaConfig;

	m_LuaConfig = new YmxLuaConfig;
	
	//Initialize the config, load the 'lua/conf.lua' file
	m_LuaConfig->_Initialize();

	return m_LuaConfig;
}

bool YmxGame::RemoveComponent(LPYMXCOMPONENT pComponent, bool bRelease /* = true */)
{
	return m_pRootComponent->RemoveComponent(pComponent, bRelease);
}

bool YmxGame::RemoveComponent(int id, bool bRelease /* = true */)
{
	return m_pRootComponent->RemoveComponent(id, bRelease);
}

bool YmxGame::RemoveComponent(const char* name, bool bRelease /* = true */)
{
	return m_pRootComponent->RemoveComponent(name, bRelease);
}

bool YmxGame::AddLuaComponent(const char* name, bool bInit)
{
	char filename[MAX_PATH + 1];
	strcpy(filename, name);
	strcat(filename, "Component.lua");
	YmxLuaComponent* pComponent = CreateLuaComponentFromFile(filename, bInit);
	if(pComponent == NULL)
		return false;
	return true;
}

// write log to file
void YmxGame::Log(PCTSTR szFormat, ...)
{
	FILE *fp = NULL;
	va_list argList;

	if(!m_szLogFile[0]) return;

	fp = _tfopen(m_szLogFile, TEXT("a"));

	if(fp == NULL) 
		return;

	va_start(argList, szFormat);
	_vftprintf(fp, szFormat, argList);
	va_end(argList);
	fprintf(fp, "\n");
	fclose(fp);
}

// Write Log to file, use Ascii
void YmxGame::LogA(const char* szFormat, ...)
{
	FILE *fp = NULL;
	va_list argList;

	if(!m_szLogFile[0]) return;

	fp = _tfopen(m_szLogFile, TEXT("a"));

	if(fp == NULL) 
		return;

	va_start(argList, szFormat);
	vfprintf(fp, szFormat, argList);
	va_end(argList);
	fprintf(fp, "\n");
	fclose(fp);
}

// Write Log to file, use Unicode
void YmxGame::LogW(const wchar_t* szFormat, ...)
{
	FILE *fp = NULL;
	va_list argList;

	if(!m_szLogFile[0]) return;

	fp = _tfopen(m_szLogFile, TEXT("a"));

	if(fp == NULL) 
		return;

	va_start(argList, szFormat);
	vfwprintf(fp, szFormat, argList);
	va_end(argList);
	fprintf(fp, "\n");
	fclose(fp);
}