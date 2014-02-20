#include "stdafx.h"
#include "ymx2d.h"

YMX2D_API void MessageBoxPrintfA(const char* caption, const char* format, ...)
{
	char szBuf[1024];
	va_list pArgList;
	va_start(pArgList, format);
	vsnprintf(szBuf, sizeof(szBuf) / sizeof(char), format, pArgList);
	va_end(pArgList);
	MessageBoxA(hwnd, szBuf, caption, 0);
}

YMX2D_API void MessageBoxPrintfW(const wchar_t* caption, const wchar_t* format, ...)
{
	wchar_t szBuf[1024];
	va_list pArgList;
	va_start(pArgList, format);
	_vsnwprintf(szBuf, sizeof(szBuf) / sizeof(wchar_t), format, pArgList);
	va_end(pArgList);
	MessageBoxW(hwnd, szBuf, caption, 0);
}

YMX2D_API void RecordErrorA(const char* caption, const char* format, ...)
{
	YmxGame* game = GetGameObject();
	if(game == NULL)
		return;

	char szBuf[1024];
	va_list pArgList;
	va_start(pArgList, format);
	vsnprintf(szBuf, sizeof(szBuf) / sizeof(char), format, pArgList);

	if(!game->m_bFullScreen)
	{
		MessageBoxA(hwnd, szBuf, caption, 0);
	}

	if(game->m_bLogError)
	{
		game->LogA("[ERROR] %s", szBuf);
	}

	va_end(pArgList);
}

YMX2D_API void RecordErrorW(const wchar_t* caption, const wchar_t* format, ...)
{
	YmxGame* game = GetGameObject();
	if(game == NULL)
		return;

	wchar_t szBuf[1024];
	va_list pArgList;
	va_start(pArgList, format);
	_vsnwprintf(szBuf, sizeof(szBuf) / sizeof(char), format, pArgList);

	if(!game->m_bFullScreen)
	{
		MessageBoxW(hwnd, szBuf, caption, 0);
	}

	if(game->m_bLogError)
	{
		game->LogW(L"[ERROR] %s", szBuf);
	}

	va_end(pArgList);
}