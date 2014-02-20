#include "stdafx.h"
#include "png.h"

#pragma once

typedef struct SysChar
{
	float x;
	float y; 
	float width;
	float height;
	TCHAR c;
} SYSCHAR, *LPSYSCHAR;

typedef struct SysCharGroup
{
	DWORD charCount;
	DWORD countPerRow;
	DWORD width;
	DWORD height;
	DWORD nRow;
	DWORD nCol;
	DWORD tmHeight;
	DWORD tmExternalLeading;
	DWORD tmAveCharWidth;
	COLORREF color;
	LOGFONT lf;
	SysChar chars[1];

} CHARGROUP, SYSCHARGROUP, *LPSYSCHARGROUP, *LPCHARGROUP;

extern OPENFILENAME ofn;
extern CHOOSEFONT cf;
extern CHOOSECOLOR cc;
extern COLORREF color;
extern LOGFONT logFont;

LPCHARGROUP CreateCharGroup(TCHAR *content, HWND);
TCHAR* RemoveRedundantChars(TCHAR* pContent);
bool SavePNG(PCTSTR fileName, LPCHARGROUP lpChargrp, HWND hwnd);
void OpenFileNameInitalize(HWND hwnd);
void ChooseFontInitalize(HWND hwnd);
void ChooseColorInitalize(HWND hwnd);
BOOL GetFileSaveDlg(HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName);
BOOL GetFileOpenDlg(HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName);
BOOL SaveCharGroup(PCTSTR pstrFileName, LPCHARGROUP lpChargrp);
LPCHARGROUP OpenCharGroup(PCTSTR pstrFileName);
void ShowCharsInEditArea(HWND hEditArea, LPCHARGROUP lpChargrp);
