// fonteditor.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "fonteditor.h"
#include <winuser.h>
#include "FontEngine.h"


#define MAX_LOADSTRING 100

#define IDC_EDITAREA 200
#define IDC_SAVEBTN 201
#define IDC_OPENBTN 202
#define IDC_NEWBTN 203
#define IDC_SAVEASBTN 204
#define IDC_FONTBTN 205
#define IDC_COLORBTN 206
#define IDC_LISTBOX 207

//control positions:
#define EDITAREA_X 20
#define EDITAREA_Y 20
#define EDITAREA_HEIGHT 200
#define TOOLS_AREA_WIDTH 150
#define LISTBOX_ITEMS_NUM 5
#define COMMON_CH_WORDS_NUM 2002

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
TCHAR szSaveBtnText[MAX_LOADSTRING];
TCHAR szOpenBtnText[MAX_LOADSTRING];
TCHAR szNewBtnText[MAX_LOADSTRING];
TCHAR szSaveAsBtnText[MAX_LOADSTRING];
TCHAR szDefaultFileTitle[MAX_LOADSTRING];
TCHAR szSaveModifyMsg[MAX_LOADSTRING];
TCHAR szFontBtnText[MAX_LOADSTRING];
TCHAR szColorBtnText[MAX_LOADSTRING];
TCHAR szCommonCharsTexts[4][MAX_LOADSTRING];
TCHAR szDemoText[MAX_LOADSTRING];
TCHAR szCommonChWords[COMMON_CH_WORDS_NUM];


HFONT hWinFont;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

OPENFILENAME ofn;
CHOOSEFONT cf;
CHOOSECOLOR cc;
COLORREF color;
LOGFONT logFont;
BOOL bModified = FALSE;
BOOL bEditExistFile = FALSE;

//if return false, do nothing
BOOL AlertSaveBeforeAbandon(HWND hWnd, PCTSTR szFileTitle);
void ResetWindowCaption(HWND hWnd, PCTSTR szFileTitle);
void InitWindowFont(HWND hWnd);
void FillListBox(HWND hListBox);
BOOL CALLBACK SetChildFont(HWND hWnd, LPARAM lParam);
void AddCharsInListBox(HWND hwnd, int index);

int APIENTRY _tWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	//InitCommonControls();
	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_FONTEDITOR, szWindowClass, MAX_LOADSTRING);
	LoadString(hInstance, IDS_SAVEBUTTON, szSaveBtnText, MAX_LOADSTRING);
	LoadString(hInstance, IDS_OPENBUTTON, szOpenBtnText, MAX_LOADSTRING);
	LoadString(hInstance, IDS_NEWBUTTON, szNewBtnText, MAX_LOADSTRING);
	LoadString(hInstance, IDS_SAVEASBUTTON, szSaveAsBtnText, MAX_LOADSTRING);
	LoadString(hInstance, IDS_SAVE_MODIFY_MSG, szSaveModifyMsg, MAX_LOADSTRING);
	LoadString(hInstance, IDS_DEFAULT_FILE_TITLE, szDefaultFileTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDS_FONTBUTTON, szFontBtnText, MAX_LOADSTRING);
	LoadString(hInstance, IDS_COLORBTN, szColorBtnText, MAX_LOADSTRING);
	LoadString(hInstance, IDS_EN_LETTERS, szCommonCharsTexts[0], MAX_LOADSTRING);
	LoadString(hInstance, IDS_NUMBERS, szCommonCharsTexts[1], MAX_LOADSTRING);
	LoadString(hInstance, IDS_EN_SYMBOLS, szCommonCharsTexts[2], MAX_LOADSTRING);
	LoadString(hInstance, IDS_CH_SYMBOLS, szCommonCharsTexts[3], MAX_LOADSTRING);
	LoadString(hInstance, IDS_CH_WORDS, szCommonChWords, COMMON_CH_WORDS_NUM);
	LoadString(hInstance, IDS_DEMO_TEXT, szDemoText, MAX_LOADSTRING);

	color = RGB(0, 0, 0);

	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FONTEDITOR));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FONTEDITOR));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW);
	//wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_FONTEDITOR);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance; // Store instance handle in our global variable

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 500, 500, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	static HWND hEditArea, hNewBtn, hSaveBtn, hSaveAsBtn, hOpenBtn, hFontBtn, hColorBtn, hListBox;
	TCHAR* pContent;
	int length;
	LPCHARGROUP lpChargrp;

	SYSCHAR chars[100];
	int width, height;

	static TCHAR szFileName[MAX_PATH], szFileTitle[MAX_PATH]; 
	static HFONT hFont;
	static RECT demoTextRect;

	switch (message)
	{
	case WM_CREATE:
		hEditArea = CreateWindow(TEXT("edit"), 
			NULL, WS_VISIBLE | WS_CHILD | ES_AUTOVSCROLL | ES_MULTILINE | WS_VSCROLL , 
			0, 0, 0, 0, hWnd, 
			(HMENU)IDC_EDITAREA, hInst, NULL);
		hNewBtn = CreateWindow(TEXT("button"), szNewBtnText,
			WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWnd,
			(HMENU)IDC_NEWBTN, hInst, NULL);
		hSaveBtn = CreateWindow(TEXT("button"), szSaveBtnText, 
			WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, 
			(HMENU)IDC_SAVEBTN, hInst, NULL);
		hSaveAsBtn = CreateWindow(TEXT("button"), szSaveAsBtnText,
			WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWnd,
			(HMENU)IDC_SAVEASBTN, hInst, NULL);
		hOpenBtn = CreateWindow(TEXT("button"), szOpenBtnText,
			WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd,
			(HMENU)IDC_OPENBTN, hInst, NULL);
		hFontBtn = CreateWindow(TEXT("button"),szFontBtnText,
			WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd,
			(HMENU)IDC_FONTBTN, hInst, NULL);
		hColorBtn = CreateWindow(TEXT("button"), szColorBtnText,
			WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd,
			(HMENU)IDC_COLORBTN, hInst, NULL);
		hListBox = CreateWindow(TEXT("listbox"), NULL, 
			WS_CHILD | WS_VISIBLE | LBS_STANDARD & ~LBS_SORT , 0, 0, 0, 0, hWnd, 
			(HMENU)IDC_LISTBOX, hInst, NULL);

		OpenFileNameInitalize(hWnd);
		ChooseFontInitalize(hWnd);
		ChooseColorInitalize(hWnd);
		EnableWindow(hSaveBtn, FALSE);
		EnableWindow(hSaveAsBtn, FALSE);
		ResetWindowCaption(hWnd, szFileTitle);
		InitWindowFont(hWnd);
		EnumChildWindows(hWnd, SetChildFont, (LPARAM)hWinFont); 
		FillListBox(hListBox);
		return 0;
	case WM_SIZE:
		width = LOWORD(lParam);
		height = HIWORD(lParam);
		MoveWindow(hEditArea, EDITAREA_X, EDITAREA_Y, width - TOOLS_AREA_WIDTH, EDITAREA_HEIGHT, TRUE);
		MoveWindow(hNewBtn, width - TOOLS_AREA_WIDTH + 30, 20, TOOLS_AREA_WIDTH - 35, 35, TRUE);
		MoveWindow(hSaveBtn, width - TOOLS_AREA_WIDTH + 30, 60, TOOLS_AREA_WIDTH - 35, 35, TRUE);
		MoveWindow(hSaveAsBtn, width - TOOLS_AREA_WIDTH + 30, 100, TOOLS_AREA_WIDTH - 35, 35, TRUE);
		MoveWindow(hOpenBtn, width - TOOLS_AREA_WIDTH + 30, 140, TOOLS_AREA_WIDTH - 35, 35, TRUE);
		MoveWindow(hFontBtn, width - TOOLS_AREA_WIDTH + 30, 180, TOOLS_AREA_WIDTH - 35, 35, TRUE);
		MoveWindow(hColorBtn, width - TOOLS_AREA_WIDTH + 30, 220, TOOLS_AREA_WIDTH - 35, 35, TRUE);
		MoveWindow(hListBox, width - TOOLS_AREA_WIDTH + 30, 275, TOOLS_AREA_WIDTH - 35, 100, TRUE);

		demoTextRect.left = EDITAREA_X;
		demoTextRect.top = EDITAREA_Y + EDITAREA_HEIGHT + 30;
		demoTextRect.bottom = height;
		demoTextRect.right = width - TOOLS_AREA_WIDTH;

		return 0;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDC_NEWBTN:
			{
				if(!AlertSaveBeforeAbandon(hWnd, szFileTitle)) break;
				bModified = FALSE;
				bEditExistFile = FALSE;
				SetWindowText(hEditArea, TEXT(""));
				EnableWindow(hSaveBtn, FALSE);
				EnableWindow(hSaveAsBtn, FALSE);
				ResetWindowCaption(hWnd, szFileTitle);
			}
			break;
		case IDC_SAVEBTN:
			if(!bEditExistFile && !GetFileSaveDlg(hWnd, szFileName, szFileTitle)) 
				return 0;
			length = GetWindowTextLength(hEditArea);
			pContent = (TCHAR*)malloc((length + 1) * sizeof(TCHAR));
			GetWindowText(hEditArea, pContent, length + 1);
			lpChargrp = CreateCharGroup(pContent, hWnd);

			if(SaveCharGroup(szFileName, lpChargrp) && SavePNG(szFileName, lpChargrp, hWnd))
			{
				bModified = FALSE;
				bEditExistFile = TRUE;
				EnableWindow(hSaveBtn, FALSE);
				ResetWindowCaption(hWnd, szFileTitle);			
			}
			else {
				MessageBox(hWnd, TEXT("字体保存失败"), 
					TEXT("提示"),0);
			}

			free(pContent);
			free(lpChargrp);
			break;
		case IDC_SAVEASBTN:
			if(!GetFileSaveDlg(hWnd, szFileName, szFileTitle)) return 0;
			length = GetWindowTextLength(hEditArea);
			pContent = (TCHAR*)malloc((length + 1) * sizeof(TCHAR));
			GetWindowText(hEditArea, pContent, length + 1);
			lpChargrp = CreateCharGroup(pContent, hWnd);

			if(SaveCharGroup(szFileName, lpChargrp) &&SavePNG(szFileName, lpChargrp, hWnd))
			{
				bModified = FALSE;
				bEditExistFile = TRUE;
				EnableWindow(hSaveBtn, FALSE);
				ResetWindowCaption(hWnd, szFileTitle);
			}
			else {
				MessageBox(hWnd, TEXT("字体保存失败"), 
					TEXT("提示"),0);
			}
			free(pContent);
			free(lpChargrp);
			break;

		case IDC_OPENBTN:
			if(!AlertSaveBeforeAbandon(hWnd, szFileTitle)) 
				return 0;
			if(!GetFileOpenDlg(hWnd, szFileName, szFileTitle)) 
				return 0;
			lpChargrp = OpenCharGroup(szFileName);
			ShowCharsInEditArea(GetDlgItem(hWnd, IDC_EDITAREA), lpChargrp);

			logFont = lpChargrp->lf;
			cf.lpLogFont = &logFont;
			cc.rgbResult = lpChargrp->color;

			free(lpChargrp);

			bModified = FALSE;
			bEditExistFile = TRUE;
			EnableWindow(hSaveAsBtn, TRUE);
			EnableWindow(hSaveBtn, FALSE);
			ResetWindowCaption(hWnd, szFileTitle);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDC_EDITAREA:
			if(wmEvent == EN_CHANGE)
			{
				length = GetWindowTextLength(hEditArea);
				if(length > 0) {
					bModified = TRUE;
					EnableWindow(hSaveBtn, TRUE);
					EnableWindow(hSaveAsBtn, TRUE);
				}
				else {
					bModified = FALSE;
					EnableWindow(hSaveBtn, FALSE);
					EnableWindow(hSaveAsBtn, FALSE);
				}
				ResetWindowCaption(hWnd, szFileTitle);
			}
			break;
		case IDC_FONTBTN:
			ChooseFont(&cf);
			if(GetWindowTextLength(hEditArea) > 0)
			{
				bModified = TRUE;
				EnableWindow(hSaveBtn, TRUE);
				EnableWindow(hSaveAsBtn, TRUE);
				ResetWindowCaption(hWnd, szFileTitle);
			}
			InvalidateRect(hWnd, &demoTextRect, TRUE);
			break;
		case IDC_COLORBTN:
			ChooseColor(&cc);
			if(GetWindowTextLength(hEditArea) > 0)
			{
				bModified = TRUE;
				EnableWindow(hSaveBtn, TRUE);
				EnableWindow(hSaveAsBtn, TRUE);
				ResetWindowCaption(hWnd, szFileTitle);
			}
			InvalidateRect(hWnd, &demoTextRect, TRUE);
			break;
		case IDC_LISTBOX:
			if(wmEvent == LBN_DBLCLK)
			{
				int iIndex = SendMessage(hListBox, LB_GETCURSEL, 0, 0);
				AddCharsInListBox(hWnd, iIndex);
				bModified = TRUE;
				EnableWindow(hSaveAsBtn, TRUE);
				EnableWindow(hSaveBtn, TRUE);
				ResetWindowCaption(hWnd, szFileTitle);
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...

		hFont = CreateFontIndirect(cf.lpLogFont);
		hFont = (HFONT)SelectObject(hdc, hFont);
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, cc.rgbResult);

		DrawText(hdc, szDemoText, lstrlen(szDemoText), &demoTextRect, DT_CENTER);

		DeleteObject(SelectObject(hdc, hFont));

		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

BOOL AlertSaveBeforeAbandon(HWND hWnd, PCTSTR szFileTitle)
{
	if(bModified)
	{
		TCHAR szMsg[MAX_PATH];
		if(bEditExistFile) {
			_stprintf(szMsg, TEXT("%s %s"), szSaveModifyMsg,szFileTitle);
		} else {
			_stprintf(szMsg, TEXT("%s %s"), szSaveModifyMsg, szDefaultFileTitle);
		}
		int result = MessageBox(hWnd, szMsg, szTitle, MB_YESNOCANCEL);
		if(result == IDYES){
			SendMessage(hWnd, WM_COMMAND, MAKEWORD(IDC_SAVEBTN, 0), 0);
		}
		else if(result == IDCANCEL){
			return FALSE;
		}
	}
	return TRUE;
}

void ResetWindowCaption(HWND hWnd, PCTSTR szFileTitle)
{
	TCHAR title[MAX_LOADSTRING];
	if(bEditExistFile)
		lstrcpy(title, szFileTitle);
	else 
		lstrcpy(title, szDefaultFileTitle);

	if(bModified)
		lstrcat(title, TEXT("* - "));
	else 
		lstrcat(title, TEXT(" - "));

	lstrcat(title, szTitle);
	SetWindowText(hWnd, title);
}

void InitWindowFont(HWND hWnd)
{
	LOGFONT lf;

	ZeroMemory(&lf, sizeof(lf));
	lstrcpy(lf.lfFaceName, _T("宋体"));
	lf.lfHeight = 12;
	hWinFont = CreateFontIndirect(&lf);
	logFont = lf;
}

BOOL CALLBACK SetChildFont(HWND hWnd, LPARAM lParam)  
{  
	SendMessage(hWnd, WM_SETFONT, (WPARAM)lParam, TRUE);  
	return TRUE;  
} 

void FillListBox(HWND hListBox)
{
	TCHAR* szCheckBtnTexts[LISTBOX_ITEMS_NUM] = {
		TEXT("英文字母"),
		TEXT("数字(0-9)"),
		TEXT("英文标点"),
		TEXT("中文标点"),
		TEXT("中文常用汉字2000")
	};

	for(int i = 0; i < LISTBOX_ITEMS_NUM; i++)
	{
		SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)szCheckBtnTexts[i]);
	}
}

void AddCharsInListBox(HWND hwnd, int index)
{
	HWND hEditArea = GetDlgItem(hwnd, IDC_EDITAREA);
	int length = GetWindowTextLength(hEditArea);
	TCHAR* szCharsToAdd;
	if(index == 4) {
		szCharsToAdd = szCommonChWords;
	}
	else {
		szCharsToAdd = szCommonCharsTexts[index];
	}
	TCHAR* szWinText = (TCHAR*)malloc((length + lstrlen(szCharsToAdd) + 1) * sizeof(TCHAR));

	GetWindowText(hEditArea, szWinText, length + 1);
	lstrcat(szWinText, szCharsToAdd);
	SetWindowText(hEditArea, szWinText);
	delete szWinText;
}