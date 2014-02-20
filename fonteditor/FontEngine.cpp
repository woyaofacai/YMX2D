#include "StdAfx.h"
#include "FontEngine.h"
#include <math.h>
#include <stdio.h>

#define SIMILAR_RGB_ERROR 100

int compareChar( const void *c1, const void *c2 )
{
	return *(TCHAR*)c1 > *(TCHAR*)c2 ? 1 : -1;
}

bool SavePNG(PCTSTR szFilename, LPCHARGROUP lpChargrp, HWND hwnd)
{
	TCHAR szPngName[MAX_PATH];
	lstrcpy(szPngName, szFilename);
	lstrcat(szPngName, TEXT(".png"));

	FILE *fp = _tfopen(szPngName, TEXT("wb"));
	if(!fp) return false;

	BITMAPINFO bmi;
	memset(&bmi, 0, sizeof(BITMAPINFO));
	bmi.bmiHeader.biWidth = lpChargrp->width;
	bmi.bmiHeader.biHeight = -lpChargrp->height;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);

	PVOID pBits;
	HBITMAP hbm = CreateDIBSection(NULL, &bmi, DIB_RGB_COLORS, &pBits, NULL, NULL);

	HDC hdc = GetDC(hwnd);
	HDC hmdc = CreateCompatibleDC(hdc);
	SetTextColor(hmdc, RGB(255, 255, 255));

	SelectObject(hmdc, GetStockObject(NULL_PEN));
	SetBkMode(hmdc, TRANSPARENT);
	SetMapMode(hmdc, MM_TEXT);
	SetTextAlign(hmdc, TA_TOP);

	SelectObject(hmdc, hbm);

	LOGFONT lf = logFont;
	lf.lfItalic = 0;
	HFONT hFont = CreateFontIndirect(&lf);
	hFont = (HFONT)SelectObject(hmdc, hFont);

	TCHAR cstr[2] = {0};
	LPSYSCHAR pChars = lpChargrp->chars;
	for (int i = 0; i < lpChargrp->charCount; i++)
	{
		cstr[0] = pChars[i].c;
		TextOut(hmdc, pChars[i].x, pChars[i].y, cstr, 1);
	}

	GdiFlush();

	png_structp png_ptr = 0;
	png_infop info_ptr = 0;

	//	create png structs
	png_ptr = png_create_write_struct
		(PNG_LIBPNG_VER_STRING, 0,
		NULL, NULL);
	if (png_ptr == 0) return false;

	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == 0) return false;

	//	init i/o
	png_init_io(png_ptr, fp);
	png_set_IHDR( png_ptr, info_ptr, lpChargrp->width, lpChargrp->height, 
		8, 
		PNG_COLOR_TYPE_RGB_ALPHA, 
		PNG_INTERLACE_NONE,
		PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
	png_write_info(png_ptr, info_ptr);
	png_set_bgr(png_ptr);	//	switch to little-endian notation

	unsigned char* pSrc = (unsigned char*)pBits;

	DWORD* pDws = (DWORD* )pBits;

	//DWORD rgb = cc.rgbResult;
	//DWORD r = rgb & 0x000000FF, g = (rgb >> 8) & 0x000000FF, b = (rgb >> 16) & 0x000000FF;
	//rgb = 0xFF000000 | (r << 16) | (g << 8) | b;
	DWORD rgb = 0xFFFFFFFF;

	for (int i = 0; i < lpChargrp->width * lpChargrp->height; i++)
	{
		if(pDws[i] != 0)
			//if(isSimilarColor(pDws[i], r, g, b))
		{
			pDws[i] = rgb;
		}
	}
	for ( int i = 0; i < lpChargrp->height; i++, pSrc+= lpChargrp->width * 4 )
	{
		png_write_row( png_ptr, pSrc );	
	}

	png_write_end(png_ptr, info_ptr);
	//	cleanup
	png_destroy_write_struct( &png_ptr, (info_ptr == 0) ? NULL : &info_ptr);

	DeleteObject(SelectObject(hmdc, hFont));
	ReleaseDC(hwnd, hmdc);
	ReleaseDC(hwnd, hdc);
	DeleteObject(hbm);
	fclose(fp);
	return true;
}

LPCHARGROUP CreateCharGroup(TCHAR *pContent, HWND hwnd)
{
	qsort((void*)pContent, lstrlen(pContent), sizeof(TCHAR), compareChar);
	TCHAR *pReducedContent = RemoveRedundantChars(pContent);
	free(pContent);

	int length = lstrlen(pReducedContent);

	LPCHARGROUP pChargrp = (LPCHARGROUP)malloc(sizeof(CHARGROUP) + (length - 1) * sizeof(SYSCHAR));

	pChargrp->charCount = length;
	pChargrp->countPerRow = ceil(sqrt((double)length));
	LPSYSCHAR pChars = pChargrp->chars;

	HDC hdc = GetDC(hwnd);
	HFONT hFont = CreateFontIndirect(cf.lpLogFont);
	hFont = (HFONT)SelectObject(hdc, hFont);

	TCHAR oneCharStr[2];
	SIZE sz;
	int char_y = 0;

	TEXTMETRIC tm;
	GetTextMetrics(hdc,&tm);
	//int cbData = GetOutlineTextMetrics(hdc, 0, NULL);
	//OUTLINETEXTMETRIC otm;
	//GetOutlineTextMetrics(hdc, cbData, &otm);


	int cHeight = tm.tmHeight + tm.tmExternalLeading;
	pChargrp->tmHeight = tm.tmHeight;
	pChargrp->tmExternalLeading = tm.tmExternalLeading;
	pChargrp->tmAveCharWidth = tm.tmAveCharWidth;

	int row = 0, col = 0;
	int currRowWidth = 0;
	int maxRowWidth = 0;

	for (int i = 0; i < length; i++)
	{
		pChars[i].c = pReducedContent[i];
		oneCharStr[0] = pChars[i].c;
		oneCharStr[1] = 0;
		GetTextExtentPoint32(hdc, oneCharStr, 1, &sz);
		pChars[i].x = currRowWidth;
		pChars[i].y = char_y;
		pChars[i].width = sz.cx;
		pChars[i].height = sz.cy;

		currRowWidth += sz.cx + 2;

		if(++col >= pChargrp->countPerRow)
		{
			col = 0;
			row++;
			char_y += cHeight + 5;
			if(currRowWidth > maxRowWidth) maxRowWidth = currRowWidth;
			currRowWidth = 0;
		}
	}

	pChargrp->width = maxRowWidth;
	pChargrp->height = char_y + cHeight;

	DeleteObject(SelectObject(hdc, hFont));

	ReleaseDC(hwnd, hdc);

	pChargrp->lf = *(cf.lpLogFont);
	pChargrp->color = (255 << 24) | ((cc.rgbResult & 0x000000FF) << 16) 
		| (cc.rgbResult & 0x0000FF00) | ((cc.rgbResult & 0x00FF0000) >> 16);

	return pChargrp;
}

TCHAR* RemoveRedundantChars(TCHAR* pContent)
{
	int length = lstrlen(pContent);
	TCHAR *pReducedContent = (TCHAR *)malloc((length + 1) * sizeof(TCHAR));
	memset(pReducedContent, 0, (length + 1) * sizeof(TCHAR));

	TCHAR *pSrc = pContent, *pDst = pReducedContent;

	TCHAR c = 0;
	for (; *pSrc != 0; pSrc++)
	{
		if(*pSrc != c)
		{
			c = *pSrc;
			*(pDst++) = c;
		}
	}

	return pReducedContent;
}


void OpenFileNameInitalize(HWND hwnd)
{
	static TCHAR szFilter[] = TEXT ("SYS2D Font Files (*.FNT)\0*.fnt\0")  \
		TEXT ("All Files (*.*)\0*.*\0\0") ;

	ofn.lStructSize       = sizeof (OPENFILENAME) ;
	ofn.hwndOwner         = hwnd ;
	ofn.hInstance         = NULL ;
	ofn.lpstrFilter       = szFilter ;
	ofn.lpstrCustomFilter = NULL ;
	ofn.nMaxCustFilter    = 0 ;
	ofn.nFilterIndex      = 0 ;
	ofn.lpstrFile         = NULL ;          // Set in Open and Close functions
	ofn.nMaxFile          = MAX_PATH ;
	ofn.lpstrFileTitle    = NULL ;          // Set in Open and Close functions
	ofn.nMaxFileTitle     = MAX_PATH ;
	ofn.lpstrInitialDir   = NULL ;
	ofn.lpstrTitle        = NULL ;
	ofn.Flags             = 0 ;             // Set in Open and Close functions
	ofn.nFileOffset       = 0 ;
	ofn.nFileExtension    = 0 ;
	ofn.lpstrDefExt       = TEXT ("fnt") ;
	ofn.lCustData         = 0 ;
	ofn.lpfnHook          = NULL ;
	ofn.lpTemplateName    = NULL ;

}

BOOL GetFileSaveDlg(HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{
	ofn.hwndOwner         = hwnd ;
	ofn.lpstrFile         = pstrFileName ;
	ofn.lpstrFileTitle    = pstrTitleName ;
	ofn.Flags             = OFN_OVERWRITEPROMPT ;
	return GetSaveFileName(&ofn);
}

BOOL GetFileOpenDlg(HWND hwnd, PTSTR pstrFileName, PTSTR pstrTitleName)
{
	ofn.hwndOwner         = hwnd ;
	ofn.lpstrFile         = pstrFileName ;
	ofn.lpstrFileTitle    = pstrTitleName ;
	ofn.Flags             = 0 ;

	return GetOpenFileName(&ofn) ;
}

BOOL SaveCharGroup(PCTSTR pstrFileName,LPCHARGROUP lpChargrp)
{
	HANDLE hFile = CreateFile(pstrFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) {
		return FALSE;
	}
	DWORD fileSize = sizeof(SYSCHARGROUP) + sizeof(SYSCHAR) * (lpChargrp->charCount - 1);
	DWORD numberOfBytesWritten;

	if(!WriteFile(hFile, (void*)lpChargrp, fileSize, &numberOfBytesWritten, NULL)) 
		return FALSE;

	if(fileSize != numberOfBytesWritten) 
		return FALSE;

	CloseHandle(hFile);
	return TRUE;
}

LPCHARGROUP OpenCharGroup(PCTSTR pstrFileName)
{
	HANDLE hFile = CreateFile(pstrFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if(hFile == INVALID_HANDLE_VALUE) return NULL;

	LPCHARGROUP lpChargrp = (LPCHARGROUP)malloc(sizeof(CHARGROUP));

	DWORD numberOfBytesRead = 0;
	if(!ReadFile(hFile, (void*)lpChargrp, sizeof(CHARGROUP), &numberOfBytesRead, NULL)){
		free(lpChargrp);
		CloseHandle(hFile);
		return NULL;
	}

	if(numberOfBytesRead != sizeof(CHARGROUP)){
		free(lpChargrp);
		CloseHandle(hFile);
		return NULL;
	}

	DWORD fileSizeHigh;
	DWORD fileSize = GetFileSize(hFile,  &fileSizeHigh);
	if(fileSizeHigh != 0)
	{
		free(lpChargrp);
		CloseHandle(hFile);
		return NULL;
	}

	DWORD charCount = lpChargrp->charCount;
	lpChargrp = (LPCHARGROUP)realloc((void*)lpChargrp, sizeof(CHARGROUP) + (charCount - 1) * sizeof(SYSCHAR));
	if(!lpChargrp) return NULL;

	LPSYSCHAR lpStartCharToRead = &lpChargrp->chars[1];

	if(!ReadFile(hFile, (void*)lpStartCharToRead, sizeof(SYSCHAR) * (charCount - 1), &numberOfBytesRead, NULL))
	{
		free(lpChargrp);
		CloseHandle(hFile);
		return NULL;
	}

	if(numberOfBytesRead != sizeof(SYSCHAR) * (charCount - 1))
	{
		free(lpChargrp);
		CloseHandle(hFile);
		return NULL;
	}

	CloseHandle(hFile);
	return lpChargrp;
}

void ShowCharsInEditArea(HWND hEditArea, LPCHARGROUP lpChargrp)
{
	TCHAR* str = (TCHAR*)malloc((lpChargrp->charCount + 1) * sizeof(TCHAR));
	int i;
	for (i = 0; i < lpChargrp->charCount; i++)
	{
		str[i] = lpChargrp->chars[i].c;		
	}
	str[i] = 0;
	SetWindowText(hEditArea, str);
	free(str);
}

void ChooseFontInitalize(HWND hwnd)
{
	cf.lStructSize    = sizeof (CHOOSEFONT) ;
	cf.hwndOwner      = hwnd ; 
	cf.hDC            = NULL ;
	cf.lpLogFont      = &logFont;
	cf.iPointSize     = 0 ;
	cf.Flags          = CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS | CF_NOVERTFONTS;
	cf.rgbColors      = 0 ;
	cf.lCustData      = 0 ;
	cf.lpfnHook       = NULL ;
	cf.lpTemplateName = NULL ;
	cf.hInstance      = NULL ;
	cf.lpszStyle      = NULL ;
	cf.nFontType      = 0 ;      
	cf.nSizeMin       = 0 ;
	cf.nSizeMax       = 0 ;	
}

void ChooseColorInitalize(HWND hwnd)
{
	static COLORREF crCustColors[16];

	cc.lStructSize = sizeof(CHOOSECOLOR);
	cc.hInstance = NULL;
	cc.hwndOwner = hwnd;
	cc.lpTemplateName = NULL;
	cc.lpfnHook = NULL;
	cc.lCustData = 0;
	cc.rgbResult = 0;
	cc.lpCustColors = crCustColors;
	cc.Flags = CC_RGBINIT | CC_ANYCOLOR;
}

