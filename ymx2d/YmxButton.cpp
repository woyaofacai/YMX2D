#include "stdafx.h"
#include "ymx2d.h"
#include "ymxform.h"

YmxButton::YmxButton(LPYMXCOMPONENT pComponent, UINT id, LPYMXTEXTURE pTexture, LPYMXFONT pFont, float x, float y)
	:m_pComponent(pComponent), m_pTexture(pTexture), m_pFont(pFont)
{
	m_id = id;
	m_z = pComponent->GetZ();
	this->SetPosition(x, y);

	m_CurState = YMXBUTTON_NORMAL;
	m_pText = NULL;

	m_bVisible = true;

	if(m_pFont == NULL)
		m_FontColor = YMX_RGB(255, 0, 0);
	else 
		m_FontColor = m_pFont->GetColor();
} 

YmxButton::~YmxButton()
{
	free(m_pText);
}

void YmxButton::SetFont(LPYMXFONT pFont)
{
	m_pFont = pFont;
}

LPYMXFONT YmxButton::GetFont()
{
	return m_pFont;
}

YMXCOLOR YmxButton::GetTextColor()
{
	return m_FontColor;
}

void YmxButton::SetZ(float z)
{
	m_z = z;
}

float YmxButton::GetZ()
{
	return m_z;
}

void YmxButton::SetTexture(LPYMXTEXTURE pTexture)
{
	m_pTexture = pTexture;
}

LPYMXTEXTURE YmxButton::GetTexture()
{
	return m_pTexture;
}

void YmxButton::SetTextureRect(YMXBUTTON_STATE state, YMXRECT& rect)
{
	m_TextureRects[state] = rect;
}

const YMXRECT& YmxButton::GetTextureRect(YMXBUTTON_STATE state)
{
	return m_TextureRects[state];
}

void YmxButton::SetText(PCTSTR text)
{
	free(m_pText);
	m_pText = (TCHAR*)malloc((lstrlen(text) + 1) * sizeof(TCHAR));
	lstrcpy(m_pText, text);
}

PCTSTR YmxButton::GetText()
{
	return m_pText;
}

void YmxButton::SetPosition(float x, float y)
{
	// m_Pos是相对于Component的位置
	m_Pos.x = x;
	m_Pos.y = y;
}

YMXPOINT YmxButton::GetPosition()
{
	return m_Pos;
}

bool YmxButton::Update(float delta)
{
	if(m_CurState == YMXBUTTON_DISABLED || !m_bVisible) 
		return true;

	float mousePosX, mousePosY;
	m_pComponent->GetMousePos(&mousePosX, &mousePosY);

	float btnWidth  = m_TextureRects[m_CurState].right - m_TextureRects[m_CurState].left;
	float btnHeight = m_TextureRects[m_CurState].bottom - m_TextureRects[m_CurState].top;

	//mouse on the button
	if(mousePosX > m_Pos.x && mousePosX < m_Pos.x + btnWidth && mousePosY > m_Pos.y && mousePosY < m_Pos.y + btnHeight)
	{
		if(m_CurState == YMXBUTTON_NORMAL) {
			m_CurState = YMXBUTTON_MOUSEOVER;
			m_pComponent->OnMessage(GCM_BUTTON_MOUSEOVER, m_id, NULL);
		}

		if(m_pComponent->KeyDown(VK_LBUTTON)) {
			m_CurState = YMXBUTTON_PRESSED;
		}	
	}
	else {
		if(m_CurState == YMXBUTTON_MOUSEOVER) {
			m_pComponent->OnMessage(GCM_BUTTON_MOUSEOUT, m_id, NULL);
			m_CurState = YMXBUTTON_NORMAL;
		}
	}

	if(m_pComponent->KeyUp(VK_LBUTTON) && m_CurState == YMXBUTTON_PRESSED) {
		m_CurState = YMXBUTTON_NORMAL;
		m_pComponent->OnMessage(GCM_BUTTON_CLICK, m_id, NULL);
	}

	return true;
}

void YmxButton::Render(YmxGraphics* g)
{
	if(!m_bVisible)
		return;

	g->RenderTexture(m_pTexture, m_Pos.x + m_pComponent->GetBoundingBox().left, 
		m_Pos.y + m_pComponent->GetBoundingBox().top, &m_TextureRects[m_CurState], m_z);

	if(m_pFont != NULL && m_pText != NULL)
	{
		int prevIndex = m_pFont->_GetCurrentIndex();
		m_pFont->_SetTextExceptIndex(m_pText, prevIndex);
		float prevZ = m_pFont->GetZ();
		m_pFont->SetZ(m_z);
		YMXCOLOR prevColor = m_pFont->GetColor();
		m_pFont->SetColor(m_FontColor);

		float fontPosX, fontPosY;

		if(m_pFont->GetComponent() == NULL)
		{
			fontPosX = (GetWidth() - m_pFont->GetWidth()) * 0.5f + m_Pos.x + m_pComponent->GetBoundingBox().left;
			fontPosY = (GetHeight() - m_pFont->GetHeight()) * 0.5f + m_Pos.y + m_pComponent->GetBoundingBox().top;
		}
		else if(m_pFont->GetComponent() == m_pComponent)
		{
			fontPosX = (GetWidth() - m_pFont->GetWidth()) * 0.5f + m_Pos.x;
			fontPosY = (GetHeight() - m_pFont->GetHeight()) * 0.5f + m_Pos.y;
		}
		else {
			fontPosX = (GetWidth() - m_pFont->GetWidth()) * 0.5f + m_Pos.x + m_pComponent->GetBoundingBox().left - m_pFont->_GetBoundingBox().left;
			fontPosY = (GetHeight() - m_pFont->GetHeight()) * 0.5f + m_Pos.y + m_pComponent->GetBoundingBox().top - m_pFont->_GetBoundingBox().top;
		}

		m_pFont->Render(g, fontPosX, fontPosY);
		if(prevIndex != -1)
			m_pFont->_SetCurrentIndex(prevIndex);
		m_pFont->SetZ(prevZ);
		m_pFont->SetColor(prevColor);
	}
}

float YmxButton::GetWidth()
{
	return m_TextureRects[m_CurState].right - m_TextureRects[m_CurState].left;
}

float YmxButton::GetHeight()
{
	return m_TextureRects[m_CurState].bottom - m_TextureRects[m_CurState].top;
}

bool YmxButton::IsEnabled()
{
	return m_CurState != YMXBUTTON_DISABLED;
}

bool YmxButton::IsVisible()
{
	return m_bVisible;
}

void YmxButton::SetVisible(bool visible)
{
	m_bVisible = visible;
}

void YmxButton::SetEnabled(bool enabled)
{
	if(enabled)
		m_CurState = YMXBUTTON_NORMAL;
	else 
		m_CurState = YMXBUTTON_DISABLED;
}

void YmxButton::Release()
{
	delete this;
}

void YmxButton::SetTextColor(YMXCOLOR color)
{
	m_FontColor = color;
}
