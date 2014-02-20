#include "stdafx.h"
#include "ymx2d.h"
#include "ymxform.h"

YmxListView::YmxListView(LPYMXCOMPONENT pComponent, UINT id, LPYMXTEXTURE pTexture, LPYMXFONT pFont, float x, float y)
	:m_pComponent(pComponent), m_pTexture(pTexture)
{
	m_id = id;
	m_z = pComponent->GetZ();
	this->SetPosition(x, y);
	this->SetFont(pFont);
	m_Items = NULL;
	m_ItemCount = 0;
	m_TextAlignment = TEXT_ALIGNMENT_LEFT;
	m_ItemWidth = 300.0f;
	m_ItemHeight = 100.0f;

	m_KeyboardEnabled = false;
	m_MouseEnabled = true;

	m_ConfirmKey = VK_RETURN;
	m_UpKey = VK_UP;
	m_DownKey = VK_DOWN;

	m_bVisible = true;

} 

YmxListView::~YmxListView()
{

}

void YmxListView::SetFont(LPYMXFONT pFont)
{
	m_pFont = pFont;
	if(m_pFont != NULL)
	{
		m_FontColors[0] = m_pFont->GetColor();
		m_FontColors[1] = m_pFont->GetColor();
		m_FontColors[2] = m_pFont->GetColor();
		m_FontColors[3] = m_pFont->GetColor();
	}
}

LPYMXFONT YmxListView::GetFont()
{
	return m_pFont;
}

void YmxListView::SetZ(float z)
{
	m_z = z;
}

float YmxListView::GetZ()
{
	return m_z;
}

void YmxListView::SetTexture(LPYMXTEXTURE pTexture)
{
	m_pTexture = pTexture;
}

LPYMXTEXTURE YmxListView::GetTexture()
{
	return m_pTexture;
}

void YmxListView::SetTextureRect(YMXLISTVIEW_STATE state, YMXRECT& rect)
{
	m_TextureRects[state] = rect;
}

const YMXRECT& YmxListView::GetTextureRect(YMXLISTVIEW_STATE state)
{
	return m_TextureRects[state];
}

void YmxListView::SetTextColor(YMXLISTVIEW_STATE state, YMXCOLOR color)
{
	m_FontColors[state] = color;
}

YMXCOLOR YmxListView::GetTextColor(YMXLISTVIEW_STATE state)
{
	return m_FontColors[state];
}

void YmxListView::SetPosition(float x, float y)
{
	// m_Pos是相对于Component的位置
	m_Pos.x = x;
	m_Pos.y = y;
}

YMXPOINT YmxListView::GetPosition()
{
	return m_Pos;
}

bool YmxListView::Update(float delta)
{
	if(!m_bVisible)
		return true;

	float itemY = m_Pos.y + (m_ItemCount - 1) * m_ItemHeight;

	if(m_MouseEnabled)
	{
		float mousePosX, mousePosY;
		m_pComponent->GetMousePos(&mousePosX, &mousePosY);
		
		YmxListViewItem* p = m_Items;
		int index = m_ItemCount;

		if(mousePosX > m_Pos.x && mousePosX < m_Pos.x + m_ItemWidth)
		{
			while(p != NULL)
			{
				if(mousePosY > itemY && mousePosY < itemY + m_ItemHeight)
				{
					if(m_pComponent->KeyDown(VK_LBUTTON))
					{
						p->state = YMXLISTVIEW_PRESSED;
					}

					if(p->state == YMXLISTVIEW_NORMAL)
					{
						p->state = YMXLISTVIEW_MOUSEOVER;
						m_pComponent->OnMessage(GCM_LIST_MOUSEOVER, m_id, (LPVOID)index);
					}
				}
				else 
				{
					if(p->state == YMXLISTVIEW_MOUSEOVER)
					{
						p->state = YMXLISTVIEW_NORMAL;
						m_pComponent->OnMessage(GCM_LIST_MOUSEOUT, m_id, (LPVOID)index);
					}
				}

				p = p->next;
				index--;
				itemY -= m_ItemHeight;
			}
		}
		else {
			while(p != NULL)
			{
				if(p->state == YMXLISTVIEW_MOUSEOVER)
				{
					p->state = YMXLISTVIEW_NORMAL;
					m_pComponent->OnMessage(GCM_LIST_MOUSEOUT, m_id, (LPVOID)index);
				}
				p = p->next;
				index--;
			}
		}

		if(m_pComponent->KeyUp(VK_LBUTTON))
		{
			p = m_Items;
			index = m_ItemCount;
			while (p != NULL)
			{
				if(p->state == YMXLISTVIEW_PRESSED)
				{
					m_pComponent->OnMessage(GCM_LIST_CLICK, m_id, (LPVOID)index);
					p->state = YMXLISTVIEW_NORMAL;
				}
				p = p->next;
				index--;
			}
		}
	}

	if(m_KeyboardEnabled)
	{
		YmxListViewItem* p = m_Items;
		
		if(m_pComponent->KeyUp(m_UpKey))
		{
			int index = m_ItemCount;
			while(p != NULL)
			{
				if(p->state == YMXLISTVIEW_MOUSEOVER)
				{
					p->state = YMXLISTVIEW_NORMAL;
					m_pComponent->OnMessage(GCM_LIST_MOUSEOUT, m_id, (LPVOID)index);
					break;
				}
				index--;
				p = p->next;
			}

			if(m_Items != NULL) 
			{
				// if no item selected now or the first one selected now, just select the last one
				if((p == NULL || p->next == NULL))
				{
					m_Items->state = YMXLISTVIEW_MOUSEOVER;
					m_pComponent->OnMessage(GCM_LIST_MOUSEOVER, m_id, (LPVOID)m_ItemCount);
				}
				else if(p->next != NULL){
					p->next->state = YMXLISTVIEW_MOUSEOVER;
					m_pComponent->OnMessage(GCM_LIST_MOUSEOVER, m_id, (LPVOID)(index - 1));
				}
				
			}
		}
		else if (m_pComponent->KeyUp(m_DownKey))
		{
			YmxListViewItem* p2 = NULL;
			int index = m_ItemCount;
			while(p != NULL)
			{
				if(p->state == YMXLISTVIEW_MOUSEOVER)
				{
					p->state = YMXLISTVIEW_NORMAL;
					m_pComponent->OnMessage(GCM_LIST_MOUSEOUT, m_id, (LPVOID)index);
					break;
				}
				index--;
				p2 = p;
				p = p->next;
			}

			// if no item selected, or the last item selected, just select the first item
			if((p == NULL || p2 == NULL) && m_Items != NULL)
			{
				p = m_Items;
				while(p->next != NULL) p = p->next;
				p->state = YMXLISTVIEW_MOUSEOVER;
				m_pComponent->OnMessage(GCM_LIST_MOUSEOVER, m_id, (LPVOID)1);
			}
			else if(p2 != NULL)
			{
				p2->state = YMXLISTVIEW_MOUSEOVER;
				m_pComponent->OnMessage(GCM_LIST_MOUSEOVER, m_id, (LPVOID)(index + 1));
			}
		}
		else if(m_pComponent->KeyDown(m_ConfirmKey))
		{
			int index = m_ItemCount;
			while(p != NULL)
			{
				if(p->state == YMXLISTVIEW_MOUSEOVER)
				{
					m_pComponent->OnMessage(GCM_LIST_CLICK, m_id, (LPVOID)index);
					break;
				}
				p = p->next;
				index--;
			}
		}
	}

	return true;
}

void YmxListView::Render(YmxGraphics* g)
{
	if(!m_bVisible)
		return;

	// draw from the bottom item to the top item, because of the order to AddItem
	YMXRECT dstRect;
	dstRect.left = m_Pos.x + m_pComponent->GetBoundingBox().left;
	dstRect.right = dstRect.left + m_ItemWidth;
	dstRect.top = m_Pos.y + m_pComponent->GetBoundingBox().top + (m_ItemCount - 1) * m_ItemHeight; 
	dstRect.bottom = dstRect.top + m_ItemHeight;
	
	if(m_pFont != NULL) {
		int prevIndex = m_pFont->_GetCurrentIndex();
		YMXCOLOR prevColor = m_pFont->GetColor();
		float prevz = m_pFont->GetZ();
		m_pFont->SetZ(m_z);
		YmxListViewItem* p = m_Items;

		float fontHeight = m_pFont->GetHeight();

		float fontPosY;
		if(m_pFont->GetComponent() == NULL)
			fontPosY = dstRect.top + (m_ItemHeight - m_pFont->GetHeight()) * 0.5f + m_pComponent->GetBoundingBox().top;
		else if (m_pFont->GetComponent() == m_pComponent)
			fontPosY = dstRect.top + (m_ItemHeight - m_pFont->GetHeight()) * 0.5f;
		else 
			fontPosY = dstRect.top + (m_ItemHeight - m_pFont->GetHeight()) * 0.5f + m_pComponent->GetBoundingBox().top - m_pFont->_GetBoundingBox().top;

		while(p != NULL)
		{
			//For this one list item, draw the background texture
			if(m_pTexture != NULL)
				g->RenderTexture(m_pTexture, &dstRect, &m_TextureRects[p->state], m_z);
			
			//then draw the text on the background
			//m_pFont->SetText(p->text);
			m_pFont->_SetTextExceptIndex(p->text, prevIndex);
			m_pFont->SetColor(m_FontColors[p->state]);
			float fontPosX;

			if(m_pFont->GetComponent() == NULL)
			{
				if(m_TextAlignment == TEXT_ALIGNMENT_LEFT)
					fontPosX = dstRect.left;
				else if(m_TextAlignment == TEXT_ALIGNMENT_CENTER)
					fontPosX = dstRect.left + (m_ItemWidth - m_pFont->GetWidth()) * 0.5f;
				else 
					fontPosX = dstRect.left + (m_ItemWidth - m_pFont->GetWidth());
			}
			else if(m_pFont->GetComponent() == m_pComponent)
			{
				if(m_TextAlignment == TEXT_ALIGNMENT_LEFT)
					fontPosX = m_Pos.x;
				else if(m_TextAlignment == TEXT_ALIGNMENT_CENTER)
					fontPosX = m_Pos.x + (m_ItemWidth - m_pFont->GetWidth()) * 0.5f;
				else 
					fontPosX = m_Pos.x + (m_ItemWidth - m_pFont->GetWidth());
			}
			else {
				if(m_TextAlignment == TEXT_ALIGNMENT_LEFT)
					fontPosX = dstRect.left - m_pFont->_GetBoundingBox().left;
				else if(m_TextAlignment == TEXT_ALIGNMENT_CENTER)
					fontPosX = dstRect.left + (m_ItemWidth - m_pFont->GetWidth()) * 0.5f - m_pFont->_GetBoundingBox().left;
				else 
					fontPosX = dstRect.left + (m_ItemWidth - m_pFont->GetWidth()) - m_pFont->_GetBoundingBox().left;
			}

			m_pFont->Render(g, fontPosX, fontPosY);
			
			// move to next item
			p = p->next;
			dstRect.top -= m_ItemHeight;
			dstRect.bottom -= m_ItemHeight;
			fontPosY -= m_ItemHeight;
		}
		m_pFont->SetColor(prevColor);
		m_pFont->SetZ(prevz);
		if(prevIndex != -1)
			m_pFont->_SetCurrentIndex(prevIndex);
	}
}

void YmxListView::SetItemSize(float width, float height)
{
	m_ItemWidth = max(0, width);
	m_ItemHeight = max(0, height);
}

void YmxListView::GetItemSize(float* width, float* height)
{
	*width = m_ItemWidth;
	*height = m_ItemHeight;
}

void YmxListView::AddItem(PCTSTR text)
{
	if(text == NULL) 
		return;
	YmxListViewItem* item = new YmxListViewItem;
	item->text = (TCHAR*)malloc((lstrlen(text) + 1) * sizeof(TCHAR));
	lstrcpy(item->text, text);

	item->state = YMXLISTVIEW_NORMAL;

	if(m_Items == NULL)
	{
		m_Items = item;
		m_Items->next = NULL;
	}
	else {
		item->next = m_Items;
		m_Items = item;
	}

	m_ItemCount++;
}

void YmxListView::RemoveItem(int index)
{
	int numToMove = m_ItemCount - index;
	if(numToMove < 0) 
		return;
	YmxListViewItem *p1 = m_Items, *p2 = NULL;
	int i;
	for (i = 0; i < numToMove; i++)
	{
		p2 = p1;
		p1 = p1->next;
	}

	if(p2 == NULL)
	{
		m_Items = m_Items->next;
	}
	else {
		p2->next = p1->next;
	}

	free(p1->text);
	delete p1;
	m_ItemCount--;
}

void YmxListView::SetTextAlignment(YMX_TEXT_ALIGNMENT align)
{
	m_TextAlignment = align;
}

void YmxListView::EnableKeyboard(bool b)
{
	m_KeyboardEnabled = b;
}

void YmxListView::EnableMouse(bool b)
{
	m_MouseEnabled = b;
}

void YmxListView::SetFocusItem(int index)
{
	if(index < 1 || index > m_ItemCount) return;

	YmxListViewItem* p = m_Items;
	int i = m_ItemCount;

	while(p != NULL)
	{
		if(i == index)
		{
			p->state = YMXLISTVIEW_MOUSEOVER;
		}
		else {
			p->state = YMXLISTVIEW_NORMAL;
		}
		p = p->next;
		i--;
	}
}

int YmxListView::GetFocusItem()
{
	YmxListViewItem* p = m_Items;
	int i = m_ItemCount;

	while(p != NULL)
	{
		if(p->state != YMXLISTVIEW_NORMAL)
		{
			return i;
		}
		p = p->next;
		i--;
	}

	return -1;
}

void YmxListView::SetConfirmKey(int key)
{
	m_ConfirmKey = key;
}

int YmxListView::GetConfirmKey()
{
	return m_ConfirmKey;
}

void YmxListView::SetUpKey(int key)
{
	m_UpKey = key;
}

int YmxListView::GetUpKey()
{
	return m_UpKey;
}

void YmxListView::SetDownKey(int key)
{
	m_DownKey = key;
}

int YmxListView::GetDownKey()
{
	return m_DownKey;
}

void YmxListView::SetVisible(bool visible)
{
	m_bVisible = visible;
}

bool YmxListView::IsVisible()
{
	return m_bVisible;
}

void YmxListView::Release()
{
	while(m_ItemCount > 0)
	{
		RemoveItem(1);
	}

	delete this;
}