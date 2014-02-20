#include "stdafx.h"
#include "ymx2d.h"
#include "ymxvertex.h"

#define DEFAULT_FONT_BUF_NUM 10

YmxFont::YmxFont(LPYMXCHARGROUP pChargrp, LPYMXTEXTURE pTexture, LPYMXCOMPONENT pComponent)
	:m_pChargrp(pChargrp),m_pTexture(pTexture),m_pComponent(pComponent),m_ppYmxCharsBuf(NULL),m_pStrsBuf(NULL)
{
	m_length = 0;
	m_z = 0.0f;
	m_BufferCount = 0;
	_SetStrBufCount(DEFAULT_FONT_BUF_NUM);
	m_bufCursor = 0;
	m_bMultiline = false;
	m_bAutoNewline = false;
	m_LineSpacing = 0.0f;
	m_CharSpacing = 0.0f;
	m_Scale = 1.0f;
	m_MaxCharCount = 100000;
}

YmxFont::YmxFont(YmxFont&)
{

}

YmxFont::~YmxFont()
{

}

LPYMXFONT YmxFont::Load_From_File(PCTSTR szFntFileName, LPYMXCOMPONENT pComponent)
{
	HANDLE hFile = CreateFile(szFntFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
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
	lpChargrp = (LPCHARGROUP)realloc((void*)lpChargrp, sizeof(CHARGROUP) + (charCount - 1) * sizeof(YMXCHAR));
	if(!lpChargrp) return NULL;

	LPYMXCHAR lpStartCharToRead = &lpChargrp->chars[1];

	if(!ReadFile(hFile, (void*)lpStartCharToRead, sizeof(YMXCHAR) * (charCount - 1), &numberOfBytesRead, NULL))
	{
		free(lpChargrp);
		CloseHandle(hFile);
		return NULL;
	}

	int x = sizeof(YMXCHAR);
	if(numberOfBytesRead != sizeof(YMXCHAR) * (charCount - 1))
	{
		free(lpChargrp);
		CloseHandle(hFile);
		return NULL;
	}

	CloseHandle(hFile);

	TCHAR* szTextureFileName = (TCHAR*)malloc((lstrlen(szFntFileName) + 5) * sizeof(TCHAR));
	lstrcpy(szTextureFileName, szFntFileName);
	lstrcat(szTextureFileName, TEXT(".png"));

	LPYMXTEXTURE pTexture = GetGameObject()->LoadTexture(szTextureFileName);

	if(pTexture == NULL)
	{
		free(szTextureFileName);
		return NULL;
	}
	free(szTextureFileName);
	LPYMXFONT pFont = new YMXFONT(lpChargrp, pTexture, pComponent);
	return pFont;
}

void YmxFont::Release()
{
	m_pTexture->Release();
	_ReleaseBuffer();
	//LPYMXCHAR pChar = m_pChargrp->chars + 1;
	//free(pChar);
	free(m_pChargrp);
}

void YmxFont::SetText(PTSTR str)
{
	int index = _FindStrInBuffer(str);

	if(index >= 0) // str is in the buffer 
	{
		m_currBufIndex = index;
	}
	else { // str is not in the buffer
		
		int length = lstrlen(str);
		// Add the str to m_pStrsBuf as an array element
		PTSTR strInBuf = (PTSTR)malloc((length + 1) * sizeof(TCHAR));
		lstrcpy(strInBuf, str);
		free(m_pStrsBuf[m_bufCursor]);
		m_pStrsBuf[m_bufCursor] = strInBuf;

		// Set m_ppYmxCharsBuf[m_bufCursor] 
		int i;
		LPYMXCHAR* ppYmxChars = (LPYMXCHAR*)malloc(length * sizeof(LPYMXCHAR));
		for (i = 0; i < length; i++)
		{
			ppYmxChars[i] = _FindYmxChar(str[i]);
		}
		free(m_ppYmxCharsBuf[m_bufCursor]);
		m_ppYmxCharsBuf[m_bufCursor] = ppYmxChars;
		
		// Update cursor and current Index
		m_currBufIndex = m_bufCursor;
		m_bufCursor = (m_bufCursor + 1) % m_BufferCount;
	}
}

PCTSTR YmxFont::GetText()
{
	return m_currBufIndex < 0 ? NULL : m_pStrsBuf[m_currBufIndex];
}

void YmxFont::SetZ(float z)
{
	m_z = z;
}

float YmxFont::GetZ()
{
	return m_z;
}

void YmxFont::Render(LPYMXGRAPHICS g, float x, float y)
{
	YMXCOLOR prevColor = g->SetVertexColor(m_pChargrp->color);
	_Render(g, x, y);
	g->SetVertexColor(prevColor);
}

void YmxFont::_Render(LPYMXGRAPHICS g, float x, float y)
{
	PCTSTR str = m_pStrsBuf[m_currBufIndex];
	LPYMXCHAR* ppYmxChars = m_ppYmxCharsBuf[m_currBufIndex];

	//if(_IsCharOutsideBoundingBox(bAutoNewLine, x, y)) return;
	YMXRECT rect;
	if(m_pComponent == NULL || !m_pComponent->m_bRequireClip)
	{
		rect.left = rect.top = 0;
		rect.right = g->GetWidth();
		rect.bottom = g->GetHeight();
	}
	else // else base on the component effective rect to clip
	{
		rect = m_pComponent->m_BoundingBox;
	}

	if(m_pComponent != NULL)
	{
		x += m_pComponent->m_BoundingBox.left;
		y += m_pComponent->m_BoundingBox.top;
	}

	if(x > rect.right || y > rect.bottom) return;

	// Single Line
	if(_IsCurrStrSingleLine())
	{
		if(y + m_pChargrp->tmHeight < rect.top) return;
		if(y > rect.bottom) return;

		if(m_pComponent != NULL && m_pComponent->m_bRequireClip && (y < rect.top || y + m_pChargrp->tmHeight > rect.bottom))
		{
			_RenderSingleLine(g, x, y, 0, lstrlen(str) - 1, rect, true);
			return;
		}
		_RenderSingleLine(g, x, y, 0, lstrlen(str) - 1, rect, false);
		return;
	}

	//Multiple Line
	// not use autoNewLine
	float cHeight = m_pChargrp->tmHeight + m_LineSpacing;
	if(!m_bAutoNewline) 
	{
		int start = 0;
		int s, e;
		int end = min(m_MaxCharCount, lstrlen(str)) - 1;
		PCTSTR p1 = str, p2 = str;
		while(y + cHeight < rect.top)
		{
			p1 = _tcschr(p2, TEXT('\n'));
			if(p1 == NULL) break;
			p2 = p1 + 1;
			y += cHeight;
		}

		if(p1 == NULL) return;
		s = p2 - str;
		p1 = _tcschr(p2, TEXT('\n'));
		if(p1 != NULL) e = p1 - p2 + s - 1;
		else e = end;
		_RenderSingleLine(g, x, y, s, e, rect, true);

		if(p1 == NULL) return;

		y += cHeight;
		while(y + cHeight < rect.bottom)
		{
			s = e + 2;
			p1++;
			p2 = _tcschr(p1, TEXT('\n'));
			if(p2 == NULL) e = end;
			else e += p2 - p1 + 1;
			_RenderSingleLine(g, x, y, s, e, rect, false);
			y += cHeight;
			if(p2 == NULL) break;
			p1 = p2;
		}

		if(p2 == NULL) return;
		if(y > rect.bottom) return;
		s = e + 2;
		p1++;
		p2 = _tcschr(p1, TEXT('\n'));
		if(p2 == NULL) e = end;
		else e += p2 - p1 + 1;
		_RenderSingleLine(g, x, y, s, e, rect, true);
		return;
	}

	//auto new line
	int s = 0;
	bool bOnBordary = false;
	float rightBordary = rect.right;
	if(m_pComponent != NULL) rightBordary = m_pComponent->m_BoundingBox.right;
	while(y < rect.bottom)
	{
		if((y <= rect.top && y + cHeight >= rect.top) || (y <= rect.bottom && y + cHeight >= rect.bottom)) 
			bOnBordary = true;
		else bOnBordary = false;
		if(!_RenderSingleAutoNewLine(g, x, y, s, &s, rect, rightBordary, bOnBordary)) break;
		y += cHeight;
	}
}

bool YmxFont::_IsCurrStrSingleLine()
{
	PTSTR str = m_pStrsBuf[m_currBufIndex];
	if(m_bAutoNewline) return false;

	if(!m_bMultiline) return true;

	PTSTR p = _tcschr(str, TEXT('\n'));
	if(p == NULL)
	{
		return true;
	}
	return false;
}

void YmxFont::_RenderSingleLine(LPYMXGRAPHICS g, float x, float y, int start, int end, YMXRECT &bordaryRect, bool bOnBordary)
{
	PTSTR str = m_pStrsBuf[m_currBufIndex];
	LPYMXCHAR* ppYmxChars = m_ppYmxCharsBuf[m_currBufIndex];
	int i = start;
	end = min(m_MaxCharCount - 1, end);

	if(m_pComponent != NULL && m_pComponent->m_bRequireClip)
	{
		while(i <= end && x + _GetCharOriginWidth(i) < bordaryRect.left) {
			x += _GetCharOriginWidth(i) + m_CharSpacing;
			i++;
		}
		if(i > end) return;
		if(bOnBordary)
		{
			_RenderChar(g, ppYmxChars[i], x, y, true, true, bordaryRect);
			x += _GetCharOriginWidth(i) + m_CharSpacing;
			i++;

			while(i <= end && x + _GetCharOriginWidth(i) < bordaryRect.right)
			{
				_RenderChar(g, ppYmxChars[i], x, y, true, false, bordaryRect);
				x += _GetCharOriginWidth(i) + m_CharSpacing;
				i++;
			}
			if(i > end) return;
			_RenderChar(g, ppYmxChars[i], x, y, true, true, bordaryRect);
		}
		else {
			_RenderChar(g, ppYmxChars[i], x, y, false, true, bordaryRect);
			x += _GetCharOriginWidth(i) + m_CharSpacing;
			i++;

			while(i <= end && x + _GetCharOriginWidth(i) < bordaryRect.right)
			{
				_RenderChar(g, ppYmxChars[i], x, y);
				x += _GetCharOriginWidth(i) + m_CharSpacing;
				i++;
			}
			if(i > end) return;
			if(x < bordaryRect.right)
				_RenderChar(g, ppYmxChars[i], x, y, false, true, bordaryRect);
		}

		return;
	}

	while(i <= end && x + _GetCharOriginWidth(i) < bordaryRect.left) {
		x += _GetCharOriginWidth(i) + m_CharSpacing;
		i++;
	}
	if(i > end) return;
	while(i <= end && x < bordaryRect.right)
	{
		_RenderChar(g, ppYmxChars[i], x, y);
		x += _GetCharOriginWidth(i) + m_CharSpacing;
		i++;
	}
}

bool YmxFont::_RenderSingleAutoNewLine(LPYMXGRAPHICS g, float x, float y, int start, int *newStart, YMXRECT &bordaryRect, float rightBordary, bool bOnBordary)
{
	PTSTR str = m_pStrsBuf[m_currBufIndex];
	LPYMXCHAR* ppYmxChars = m_ppYmxCharsBuf[m_currBufIndex];
	int i = start;

	bool bNeedRender = true;
	if(y + m_pChargrp->tmHeight < bordaryRect.top)
	{
		bNeedRender = false;
	}

	while(str[i] != 0 && x + _GetCharOriginWidth(i) < bordaryRect.left) {
		if(str[i] == TEXT('\n')) {
			*newStart = i + 1;
			return true;
		}
		x += _GetCharOriginWidth(i) + m_CharSpacing;
		i++;
	}

	if(str[i] == 0) return false;
	if(str[i] == TEXT('\n')) {
		*newStart = i + 1;
		return true;
	}
	if(bNeedRender)
	{
		if(m_pComponent != NULL && m_pComponent->m_bRequireClip)
		{
			_RenderChar(g, ppYmxChars[i], x, y, true, true, bordaryRect);
		}
		else {
			_RenderChar(g, ppYmxChars[i], x, y);
		}
	}
	x += _GetCharOriginWidth(i) + m_CharSpacing;
	i++;

	while(true)
	{
		if(str[i] == 0) return false;
		if(str[i] == TEXT('\n'))
		{
			*newStart = i + 1;
			return true;
		}
		if(x + _GetCharOriginWidth(i) > rightBordary)
		{
			*newStart = i;
			return true;
		}
		if(bNeedRender)
		{
			if(m_pComponent != NULL && m_pComponent->m_bRequireClip && bOnBordary)
			{
				_RenderChar(g, ppYmxChars[i], x, y, true, false, bordaryRect);
			}
			else {
				_RenderChar(g, ppYmxChars[i], x, y);
			}
		}	

		x += _GetCharOriginWidth(i) + m_CharSpacing;
		i++;
	}

	return false;
}

void YmxFont::_RenderChar(LPYMXGRAPHICS g, LPYMXCHAR pChar, float x, float y)
{
	if(pChar == NULL) return;
	YmxRect rect;
	rect.left = pChar->x;
	rect.right = pChar->x + pChar->width;
	rect.top = pChar->y;
	rect.bottom = pChar->y + m_pChargrp->tmHeight + m_pChargrp->tmExternalLeading;
	g->RenderTexture(m_pTexture, x, y, &rect, m_z);
}

void YmxFont::_RenderChar(LPYMXGRAPHICS g, LPYMXCHAR pChar, float x, float y, bool bHclip, bool bVclip,YMXRECT& bordaryRect)
{
	if(pChar == NULL) return;
	YMXRECT textureRect;
	float cHeight = m_pChargrp->tmHeight + m_pChargrp->tmExternalLeading;
	textureRect.left = pChar->x;
	textureRect.right = pChar->x + pChar->width;
	textureRect.top = pChar->y; 
	textureRect.bottom = pChar->y + cHeight;

	if(bHclip)
	{
		if(y + cHeight > bordaryRect.bottom)
			textureRect.bottom = textureRect.bottom - (y + cHeight - bordaryRect.bottom);
		if(y < bordaryRect.top)
		{
			textureRect.top += bordaryRect.top - y;
			y = bordaryRect.top;
		}
	}

	if(bVclip)
	{
		if(x + pChar->width > bordaryRect.right)
			textureRect.right = textureRect.right - (x + pChar->width - bordaryRect.right);
		if(x < bordaryRect.left)
		{
			textureRect.left += bordaryRect.left - x;
			x = bordaryRect.left;
		}
	}

	g->RenderTexture(m_pTexture, x, y, &textureRect, m_z);
}

LPYMXCHAR YmxFont::_FindYmxChar(TCHAR c)
{
	LPYMXCHAR pChars = m_pChargrp->chars;
	int left = 0;
	int right = m_pChargrp->charCount - 1;
	int mid = 0;

	//use binary search. The Chars in File is saved in order
	while(left <= right)
	{
		mid = (left + right) >> 1;
		if(pChars[mid].c == c)
		{
			return &pChars[mid];
		}

		if(pChars[mid].c < c)
		{
			left = mid + 1;
		}
		else {
			right = mid - 1;
		}
	}

	return NULL;
}

void YmxFont::SetColor(YMXCOLOR color)
{
	m_pChargrp->color = color;
}

YMXCOLOR YmxFont::GetColor()
{
	return m_pChargrp->color;
}

void YmxFont::_ReleaseBuffer()
{
	int i;
	if(m_pStrsBuf != NULL)
	{
		for(i = 0; i < m_BufferCount; i++) 
		{
			free(m_pStrsBuf[i]);
		}
		free(m_pStrsBuf);
		m_pStrsBuf = NULL;
	}

	if(m_ppYmxCharsBuf != NULL)
	{
		for(i = 0; i < m_BufferCount; i++)
		{
			free(m_ppYmxCharsBuf[i]);
		}
		free(m_ppYmxCharsBuf);
		m_ppYmxCharsBuf = NULL;
	}
}

bool YmxFont::_SetStrBufCount(int count)
{
	if(count <= 0) 
		return false;
	_ReleaseBuffer();

	m_BufferCount = count; 
	m_currBufIndex = -1;

	m_pStrsBuf = (PTSTR*)malloc(m_BufferCount * sizeof(PTSTR));
	memset(m_pStrsBuf, 0, m_BufferCount * sizeof(PTSTR));
	m_ppYmxCharsBuf = (LPYMXCHAR**)malloc(m_BufferCount * sizeof(LPYMXCHAR*));
	memset(m_ppYmxCharsBuf, 0, m_BufferCount * sizeof(LPYMXCHAR*));
	return true;
}

int YmxFont::_FindStrInBuffer(PTSTR str)
{
	int i;
	for (i = 0; i < m_BufferCount; i++)
	{
		if(m_pStrsBuf[i] != NULL && lstrcmp(m_pStrsBuf[i], str) == 0) {
			return i;
		}
	}
	return -1;
}

bool YmxFont::SetAttribute(YMXFONT_ATTRIBUTE attr, int value)
{
	switch(attr)
	{
	case FONT_BUFFER_COUNT:
		_SetStrBufCount(value);
		return true;
	case FONT_MAX_CHAR_COUNT:
		m_MaxCharCount = value;
		return true;
	default:
		break;
	}
	return false;
}

bool YmxFont::SetAttribute(YMXFONT_ATTRIBUTE attr, bool value)
{
	switch(attr)
	{
	case FONT_MULTILINE:
		m_bMultiline = value;
		return true;
	case FONT_AUTO_NEWLINE:
		m_bAutoNewline = value;
		return true;
	default:
		break;
	}
	return false;
}

int YmxFont::_GetCharOriginWidth(int index)
{
	TCHAR c = m_pStrsBuf[m_currBufIndex][index];
	LPYMXCHAR pYmxChar = m_ppYmxCharsBuf[m_currBufIndex][index]; 

	if(c == TEXT(' '))
		return m_pChargrp->tmAveCharWidth;

	if(c == TEXT('\n'))
		return 0;

	if(pYmxChar == NULL)
	{
		return m_pChargrp->tmAveCharWidth;
	}

	return pYmxChar->width;
}

void YmxFont::SetCharSpacing(float charSpacing)
{
	m_CharSpacing = charSpacing;
}

void YmxFont::SetLineSpacing(float lineSpacing)
{
	m_LineSpacing = lineSpacing;
}

float YmxFont::GetLineSpacing()
{
	return m_LineSpacing;
}

float YmxFont::GetCharSpacing()
{
	return m_CharSpacing;
}

float YmxFont::GetWidth()
{
	PTSTR str = m_pStrsBuf[m_currBufIndex];

	if(str == NULL)
		return 0;

	float width = 0;
	int i;
	int strLenght = lstrlen(str);

	if(!m_bMultiline)
	{
		for (i = 0; i < strLenght - 1; i++)
		{
			width += this->_GetCharOriginWidth(i) + m_CharSpacing;
		}
		width += this->_GetCharOriginWidth(i);
	}
	else {
		float maxWidth = 0;
		for (i = 0; i < strLenght; i++)
		{
			if(str[i] != TEXT('\n')) 
				width += this->_GetCharOriginWidth(i) + m_CharSpacing;
			else {
				if(maxWidth < width) {
					maxWidth = width;
				}
				width = 0;
			}
		}
		if(maxWidth != 0) 
			width = maxWidth - m_CharSpacing;
	}

	return width;
}

float YmxFont::GetHeight()
{
	PTSTR str = m_pStrsBuf[m_currBufIndex];
	if(!m_bMultiline)
	{
		return m_pChargrp->tmHeight + m_pChargrp->tmExternalLeading;
	}

	int strLength = min(m_MaxCharCount, lstrlen(str));
	int i;
	float height = m_pChargrp->tmHeight + m_pChargrp->tmExternalLeading;

	for (i = 0; i < strLength; i++)
	{
		if(str[i] == TEXT('\n'))
			height += m_LineSpacing + m_pChargrp->tmHeight + m_pChargrp->tmExternalLeading;
	}
	return height;
}

LPYMXCOMPONENT YmxFont::GetComponent()
{
	return m_pComponent;
}

int YmxFont::_GetCurrentIndex()
{
	return m_currBufIndex;
}

/*
SetText, but put the string in the buffer position except index
*/
void YmxFont::_SetTextExceptIndex(PTSTR str, int exceptIndex)
{
	int index = _FindStrInBuffer(str);

	if(index >= 0) // str is in the buffer 
	{
		m_currBufIndex = index;
	}
	else { // str is not in the buffer
		int length = lstrlen(str);
		PTSTR strInBuf = (PTSTR)malloc((length + 1) * sizeof(TCHAR));
		lstrcpy(strInBuf, str);

		if(m_bufCursor == exceptIndex)
			m_bufCursor = (m_bufCursor + 1) % m_BufferCount;

		free(m_pStrsBuf[m_bufCursor]);
		free(m_ppYmxCharsBuf[m_bufCursor]);

		int i;
		LPYMXCHAR* ppYmxChars = (LPYMXCHAR*)malloc(length * sizeof(LPYMXCHAR));
		for (i = 0; i < length; i++)
		{
			ppYmxChars[i] = _FindYmxChar(str[i]);
		}

		m_ppYmxCharsBuf[m_bufCursor] = ppYmxChars;
		m_pStrsBuf[m_bufCursor] = strInBuf;

		m_currBufIndex = m_bufCursor;
		m_bufCursor = (m_bufCursor + 1) % m_BufferCount;
	}
}

void YmxFont::_SetCurrentIndex(int index)
{
	m_currBufIndex = index;
}

const YMXRECT& YmxFont::_GetBoundingBox()
{
	return m_pComponent->m_BoundingBox;
}