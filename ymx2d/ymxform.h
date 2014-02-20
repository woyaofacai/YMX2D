#include "ymx2d.h"

#ifndef YMX2D_FUNCTION_CLASS_FORM_H
#define YMX2D_FUNCTION_CLASS_FORM_H

enum YMX2D_API YMXBUTTON_STATE
{
	YMXBUTTON_NORMAL = 0,
	YMXBUTTON_PRESSED = 1,
	YMXBUTTON_MOUSEOVER = 2,
	YMXBUTTON_DISABLED = 3
};

enum YMX2D_API YMXLISTVIEW_STATE
{
	YMXLISTVIEW_NORMAL = 0,
	YMXLISTVIEW_PRESSED = 1,
	YMXLISTVIEW_MOUSEOVER = 2
};

class YMX2D_API YmxButton
{
	friend YmxComponent;
public: 
	~YmxButton();
	inline void SetTextureRect(YMXBUTTON_STATE state, YMXRECT& rect);
	inline const YMXRECT& GetTextureRect(YMXBUTTON_STATE state);
	inline void SetTexture(LPYMXTEXTURE pTexture);
	inline LPYMXTEXTURE GetTexture();
	inline void SetFont(LPYMXFONT pFont);
	inline void SetTextColor(YMXCOLOR color);
	inline LPYMXFONT GetFont();
	inline YMXCOLOR GetTextColor();
	inline void SetText(PCTSTR text);
	inline PCTSTR GetText();
	inline void SetZ(float z);
	inline float GetZ();
	bool Update(float delta); 
	void Render(YmxGraphics* g);
	inline void SetPosition(float x, float y);
	inline YMXPOINT GetPosition();
	inline float GetWidth();
	inline float GetHeight();
	inline void SetVisible(bool visible);
	inline void SetEnabled(bool enabled);
	inline bool IsVisible();
	inline bool IsEnabled();
	void Release();
private:
	YmxButton(LPYMXCOMPONENT pComponent, UINT id, LPYMXTEXTURE pTexture, LPYMXFONT pFont, float x, float y);
	LPYMXCOMPONENT m_pComponent;
	UINT m_id;
	LPYMXTEXTURE m_pTexture; 
	LPYMXFONT m_pFont;
	TCHAR* m_pText;
	YMXBUTTON_STATE m_CurState;
	YMXRECT m_TextureRects[4];
	float m_z;
	YMXPOINT m_Pos;
	bool m_bVisible;
	YMXCOLOR m_FontColor;
};

enum YMX2D_API YMX_TEXT_ALIGNMENT
{
	TEXT_ALIGNMENT_LEFT,
	TEXT_ALIGNMENT_RIGHT,
	TEXT_ALIGNMENT_CENTER
};

class YMX2D_API YmxListView
{
	friend YmxComponent;
	struct YmxListViewItem
	{
		PTSTR text;
		YMXLISTVIEW_STATE state;
		YmxListViewItem* next;
	};

public:
	~YmxListView();
	inline void SetTextureRect(YMXLISTVIEW_STATE state, YMXRECT& rect);
	inline const YMXRECT& GetTextureRect(YMXLISTVIEW_STATE state);
	inline void SetTextColor(YMXLISTVIEW_STATE state, YMXCOLOR color);
	inline YMXCOLOR GetTextColor(YMXLISTVIEW_STATE state);
	inline void SetTexture(LPYMXTEXTURE pTexture);
	inline LPYMXTEXTURE GetTexture();
	inline void SetItemSize(float width, float height);
	inline void GetItemSize(float* width, float* height);
	inline void SetFont(LPYMXFONT pFont);
	inline LPYMXFONT GetFont();
	inline void SetZ(float z);
	inline float GetZ(); 
	bool Update(float delta);
	inline void SetPosition(float x, float y);
	inline YMXPOINT GetPosition();
	void Render(YmxGraphics* g);
	inline void AddItem(PCTSTR text);
	inline void RemoveItem(int index);
	inline void SetTextAlignment(YMX_TEXT_ALIGNMENT);
	inline void EnableKeyboard(bool);
	inline void EnableMouse(bool);
	inline void SetFocusItem(int index);
	inline int GetFocusItem();
	inline void SetConfirmKey(int key);
	inline int GetConfirmKey();
	inline void SetUpKey(int key);
	inline int GetUpKey();
	inline void SetDownKey(int key);
	inline int GetDownKey();
	inline void SetVisible(bool);
	inline bool IsVisible();
	inline void Release();
private:
	YmxListView(LPYMXCOMPONENT, UINT id, LPYMXTEXTURE, LPYMXFONT, float x, float y);
	LPYMXCOMPONENT m_pComponent;
	UINT m_id;
	LPYMXTEXTURE m_pTexture;
	LPYMXFONT m_pFont;
	YmxListViewItem* m_Items;
	int m_ItemCount;
	float m_z;
	YMXPOINT m_Pos;
	YMXRECT m_TextureRects[3];
	YMXCOLOR m_FontColors[3];
	float m_ItemWidth;
	float m_ItemHeight;
	bool m_KeyboardEnabled;
	bool m_MouseEnabled;
	int m_ConfirmKey;
	int m_UpKey;
	int m_DownKey;
	bool m_bVisible;
	YMX_TEXT_ALIGNMENT m_TextAlignment;
};

enum YMX2D_API YMXEDITTEXT_CURSOR_STYLE
{
	YMXEDITTEXT_CURSOR_VERTICAL,
	YMXEDITTEXT_CURSOR_HORIZONTAL,
	YMXEDITTEXT_CURSOR_NONE
};


class YMX2D_API YmxEditText
{
	friend YmxComponent;
public:
	~YmxEditText();
	inline void SetZ(float z);
	inline float GetZ();  
	inline void SetFont(LPYMXFONT pFont);
	inline LPYMXFONT GetFont();
	inline void SetTextColor(YMXCOLOR color);
	inline YMXCOLOR GetTextColor();
	bool Update(float delta);
	inline void SetPosition(float x, float y);
	inline YMXPOINT GetPosition();
	void Render(YmxGraphics* g);
	inline void SetTextAlignment(YMX_TEXT_ALIGNMENT);
	inline void SetVisible(bool visible);
	inline bool IsVisible();
	void Release();
	inline PCTSTR GetText();
	inline void SetText(PCTSTR str);
	inline void SetCursorColor(YMXCOLOR color);
	inline void SetCursorThickness(float thickness);
	inline void SetCursorStyle(YMXEDITTEXT_CURSOR_STYLE);	
	inline void SetCursorBlink(bool blinking);
	inline void SetCursorBlinkFreq(float freq);
	inline void SetFocus(bool focus);
	inline bool IsFocused();
	inline void SetConfirmKey(int key);
	inline int GetConfirmKey();
private:
	YmxEditText(LPYMXCOMPONENT, UINT id, LPYMXFONT, float x, float y);
	inline void _AppendChar(TCHAR c);
	inline void _RenderCursor(YmxGraphics* g);
	inline float _GetCursorPosX();
	LPYMXCOMPONENT m_pComponent;
	UINT m_id;
	LPYMXFONT m_pFont;
	float m_z;
	YMXPOINT m_Pos;
	YMXRECT m_TextureRects[5];
	YMX_TEXT_ALIGNMENT m_TextAlignment;
	bool m_bVisible;
	bool m_bFocused;
	TCHAR* m_pText;
	int m_TextBufSize;
	int m_TextLength;
	int m_CursorThickness;
	bool m_bCursorVertical;
	YMXCOLOR m_CursorColor;
	YMXCOLOR m_FontColor;
	YMXEDITTEXT_CURSOR_STYLE m_CursorOrientation;
	bool m_bCursorBlink;
	float m_CursorBlinkInterval;
	float m_timeSinceCursorAppear;
	bool m_bCursorAppearNow;
	int m_ConfirmKey;
};

#endif