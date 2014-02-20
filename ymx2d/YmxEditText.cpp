#include "stdafx.h"
#include "ymx2d.h"
#include "ymxform.h"

YmxEditText::YmxEditText(LPYMXCOMPONENT pComponent, UINT id, LPYMXFONT pFont, float x, float y)
	:m_pComponent(pComponent), m_pFont(pFont)
{
	m_id = id;
	m_z = pComponent->GetZ();
	this->SetPosition(x, y);

	m_bVisible = true;

	m_TextBufSize = 10;
	m_TextLength = 0;
	m_pText = (TCHAR*)malloc(m_TextBufSize * sizeof(TCHAR));
	m_pText[0] = 0;

	m_CursorOrientation = YMXEDITTEXT_CURSOR_VERTICAL;

	if(m_pFont != NULL)
		m_FontColor = m_pFont->GetColor();
	else 
		m_FontColor = YMX_RGB(255, 255, 255);

	m_CursorColor = YMX_RGB(255, 255, 255);
	m_CursorThickness = 5.0f;
	m_bCursorBlink = true;
	m_CursorBlinkInterval = 0.5f;

	m_bCursorAppearNow = true;
	m_timeSinceCursorAppear = 0;

	m_bFocused = true;

	m_ConfirmKey = VK_RETURN;

	m_TextAlignment = TEXT_ALIGNMENT_LEFT;
}

YmxEditText::~YmxEditText()
{
	free(m_pText);
}

float YmxEditText::GetZ()
{
	return m_z;
}

void YmxEditText::SetZ(float z)
{
	m_z = RESTRICT_Z(z);
}

void YmxEditText::SetVisible(bool visible)
{
	m_bVisible = visible;
}

bool YmxEditText::IsVisible()
{
	return m_bVisible;
}

void YmxEditText::SetPosition(float x, float y)
{
	m_Pos.x = x;
	m_Pos.y = y;
}

YMXPOINT YmxEditText::GetPosition()
{
	return m_Pos;
}

bool YmxEditText::Update(float delta)
{
	if(!m_bVisible || !m_bFocused)
		return true;

	TCHAR c = m_pComponent->GetInputChar();
	if(c == VK_BACK)
	{
		if(m_TextLength > 0)
		{
			m_pText[m_TextLength - 1] = 0;
			m_TextLength--;
			m_pComponent->OnMessage(GCM_EDITTEXT_CHANGE, m_id, NULL);
		}
	}
	else if(c == m_ConfirmKey)
	{
		m_pComponent->OnMessage(GCM_EDITTEXT_CONFIRM, m_id, NULL);
	}
	else if(c != 0)
	{
		if(m_pFont != NULL && (m_pFont->_FindYmxChar(c) != NULL || c == TEXT(' '))) {
			_AppendChar(c);
			m_pComponent->OnMessage(GCM_EDITTEXT_CHANGE, m_id, NULL);
		}
	}

	//Update Cursor
	if(m_CursorOrientation != YMXEDITTEXT_CURSOR_NONE && m_bCursorBlink)
	{
		m_timeSinceCursorAppear += delta;
		if(m_timeSinceCursorAppear > m_CursorBlinkInterval)
		{
			m_timeSinceCursorAppear = 0.0f;
			m_bCursorAppearNow = !m_bCursorAppearNow;
		}
	}

	return true;
}

float YmxEditText::_GetCursorPosX()
{
	float x;
	if(m_TextLength == 0)
		x = m_pComponent->GetBoundingBox().left + m_Pos.x;
	else {
		if(m_TextAlignment == TEXT_ALIGNMENT_LEFT)
			x = m_pComponent->GetBoundingBox().left + m_Pos.x + m_pFont->GetWidth();
		else if (m_TextAlignment == TEXT_ALIGNMENT_CENTER)
			x = m_pComponent->GetBoundingBox().left + m_Pos.x + m_pFont->GetWidth() * 0.5f;
		else 
			x = m_pComponent->GetBoundingBox().left + m_Pos.x + m_pFont->GetWidth();
	}
	return x;
}


void YmxEditText::_RenderCursor(YmxGraphics* g)
{
	if(m_bCursorAppearNow)
	{
		if(m_CursorOrientation == YMXEDITTEXT_CURSOR_VERTICAL)
		{
			YmxRect cursorRect;
			cursorRect.left = _GetCursorPosX();
			cursorRect.bottom = m_pComponent->GetBoundingBox().top + m_Pos.y;
			cursorRect.top = cursorRect.bottom - m_pFont->GetHeight();
			cursorRect.right = cursorRect.left + m_CursorThickness;
			g->RenderRectangle(&cursorRect, m_CursorColor, m_z);
		}
		else if(m_CursorOrientation == YMXEDITTEXT_CURSOR_HORIZONTAL)
		{
			YmxRect cursorRect;
			cursorRect.left = _GetCursorPosX();
			cursorRect.top = m_pComponent->GetBoundingBox().top + m_Pos.y;
			cursorRect.right = cursorRect.left + m_pFont->m_pChargrp->tmAveCharWidth;
			cursorRect.bottom = cursorRect.top + m_CursorThickness;
			g->RenderRectangle(&cursorRect, m_CursorColor, m_z);
		}
	}
}

void YmxEditText::Render(YmxGraphics* g)
{
	if(m_pFont == NULL || !m_bVisible) 
		return;

	if(m_TextLength != 0)
	{
		YMXCOLOR prevColor = m_pFont->GetColor();
		m_pFont->SetColor(m_FontColor);
		int prevIndex = m_pFont->_GetCurrentIndex();
		m_pFont->_SetTextExceptIndex(m_pText, prevIndex);
		float x, y;

		if(m_pFont->GetComponent() == NULL)
		{
			float prevZ = m_pFont->GetZ();
			m_pFont->SetZ(m_z);
			if(m_TextAlignment == TEXT_ALIGNMENT_LEFT)
				x = m_Pos.x + m_pComponent->GetBoundingBox().left;
			else if(m_TextAlignment == TEXT_ALIGNMENT_CENTER)
				x = m_Pos.x + m_pComponent->GetBoundingBox().left - m_pFont->GetWidth() * 0.5f;
			else 
				x = m_Pos.x + m_pComponent->GetBoundingBox().left - m_pFont->GetWidth();
			y = m_Pos.y + m_pComponent->GetBoundingBox().top - m_pFont->GetHeight();
			m_pFont->Render(g, x, y);
			m_pFont->SetZ(prevZ);
		}
		else if(m_pFont->GetComponent() == m_pComponent)
		{
			if(m_TextAlignment == TEXT_ALIGNMENT_LEFT)
				x = m_Pos.x;
			else if(m_TextAlignment == TEXT_ALIGNMENT_CENTER)
				x = m_Pos.x - m_pFont->GetWidth() * 0.5f;
			else 
				x = m_Pos.x - m_pFont->GetWidth();
			y = m_Pos.y - m_pFont->GetHeight();
			m_pFont->Render(g, x, y);
		}
		else 
		{

			float prevZ = m_pFont->GetZ();
			m_pFont->SetZ(m_z);
			const LPYMXCOMPONENT pFontComponent = m_pFont->GetComponent();
			if(m_TextAlignment == TEXT_ALIGNMENT_LEFT)
				x = m_Pos.x + m_pComponent->GetBoundingBox().left - pFontComponent->GetBoundingBox().left;
			else if (m_TextAlignment == TEXT_ALIGNMENT_CENTER)
				x = m_Pos.x + m_pComponent->GetBoundingBox().left - pFontComponent->GetBoundingBox().left - m_pFont->GetWidth() * 0.5f;
			else 
				x = m_Pos.x + m_pComponent->GetBoundingBox().left - pFontComponent->GetBoundingBox().left - m_pFont->GetWidth();
			y = m_Pos.y + m_pComponent->GetBoundingBox().top - pFontComponent->GetBoundingBox().top - m_pFont->GetHeight();
			m_pFont->Render(g, x, y);
			m_pFont->SetZ(prevZ);
		}

		if(m_bFocused)
			_RenderCursor(g);

		if(prevIndex != -1)
			m_pFont->_SetCurrentIndex(prevIndex);

		m_pFont->SetColor(prevColor);
	}
	else {
		if(m_bFocused)
			_RenderCursor(g);
	}
}

void YmxEditText::SetTextAlignment(YMX_TEXT_ALIGNMENT align)
{
	m_TextAlignment = align;
}

void YmxEditText::Release()
{
	free(m_pText);
}

void YmxEditText::SetText(PCTSTR str)
{
	int len = lstrlen(str);
	if(len > m_TextBufSize - 1)
	{
		free(m_pText);
		if(len + 1 > m_TextBufSize * 2)
		{
			m_TextBufSize = len + 1;
		}
		else {
			m_TextBufSize *= 2;
		}
		m_pText = (TCHAR*)malloc((len + 1) * sizeof(TCHAR));
	}
	lstrcpy(m_pText, str);
	m_TextLength = len;
}

void YmxEditText::_AppendChar(TCHAR c)
{
	if(m_TextLength == m_TextBufSize - 1)
	{
		m_TextBufSize *= 2;
		m_pText = (TCHAR*)realloc(m_pText, m_TextBufSize * sizeof(TCHAR));
	}

	m_pText[m_TextLength] = c;
	m_pText[++m_TextLength] = 0;
}

PCTSTR YmxEditText::GetText()
{
	return m_pText;
}

void YmxEditText::SetCursorStyle(YMXEDITTEXT_CURSOR_STYLE orientation)
{
	m_CursorOrientation = orientation;
}

void YmxEditText::SetCursorThickness(float thickness)
{
	m_CursorThickness = thickness;
}

void YmxEditText::SetCursorColor(YMXCOLOR color)
{
	m_CursorColor = color;
}

void YmxEditText::SetFont(LPYMXFONT pFont)
{
	m_pFont = pFont;
}

LPYMXFONT YmxEditText::GetFont()
{
	return m_pFont;
}

void YmxEditText::SetTextColor(YMXCOLOR color)
{
	m_FontColor = color;
}

YMXCOLOR YmxEditText::GetTextColor()
{
	return m_FontColor;
}

void YmxEditText::SetCursorBlink(bool blinking)
{
	m_bCursorBlink = blinking;
}

void YmxEditText::SetCursorBlinkFreq(float freq)
{
	m_CursorBlinkInterval = 1.0f / freq;
}

void YmxEditText::SetFocus(bool focus)
{
	m_bFocused = focus;
}

bool YmxEditText::IsFocused()
{
	return m_bFocused;
}

void YmxEditText::SetConfirmKey(int key)
{
	m_ConfirmKey = key;
}

int YmxEditText::GetConfirmKey()
{
	return m_ConfirmKey;
}
