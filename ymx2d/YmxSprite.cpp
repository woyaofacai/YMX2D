#include "stdafx.h"
#include "ymx2d.h"
#include "ymxvertex.h"

YmxSprite::YmxSprite(LPYMXTEXTURE pTexture, YMXRECT& textureRect, LPYMXCOMPONENT pComponent)
	:m_pTexture(pTexture), m_TextureRect(textureRect), m_pComponent(pComponent)
{
	m_z = 0.0f;
	m_hotSpot.x = 0;
	m_hotSpot.y = 0;
}

float YmxSprite::GetWidth()
{
	return m_TextureRect.right - m_TextureRect.left;
}

float YmxSprite::GetHeight()
{
	return m_TextureRect.bottom - m_TextureRect.top;
}

void YmxSprite::Render(LPYMXGRAPHICS g, float x, float y)
{
	x = x - m_hotSpot.x;
	y = y - m_hotSpot.y;

	if(m_pComponent == NULL)
	{
		g->RenderTexture(m_pTexture, x, y, &m_TextureRect, m_z);
		return;
	}
	
	//if no need to clip
	if(!m_pComponent->m_bRequireClip)
	{
		g->RenderTexture(m_pTexture, x + m_pComponent->m_BoundingBox.left, y + m_pComponent->m_BoundingBox.top, &m_TextureRect, m_z);
		return;
	}

	float w = m_TextureRect.right - m_TextureRect.left;
	float h = m_TextureRect.bottom - m_TextureRect.top;

	float boundingBoxWidth = m_pComponent->m_BoundingBox.right - m_pComponent->m_BoundingBox.left;
	float boundingBoxHeight = m_pComponent->m_BoundingBox.bottom - m_pComponent->m_BoundingBox.top;

	// if the sprite is out of BoundingBox, then no need to render
	if(x + w <= 0 || x >= boundingBoxWidth || y + h <= 0 || y >= boundingBoxHeight)
	{
		return;
	}

	// if the whole sprite is in the boundingBox, no need to clip
	if(x >= 0 && x + w <= boundingBoxWidth && y >= 0 && y + h <= boundingBoxHeight)
	{
		g->RenderTexture(m_pTexture, x + m_pComponent->m_BoundingBox.left, y + m_pComponent->m_BoundingBox.top, &m_TextureRect, m_z);
		return;
	}

	/* Need Clipping Condition.
	   because no rotate, so don't use complex algorithm*/

	// calculate the new TextureRect, clipped by the bordary.
	YMXRECT newTexRect = m_TextureRect;
	float start_x = x, start_y = y;
	if(x < 0) {
		newTexRect.left -= x;
		start_x = 0;
	}
	if(x + w > boundingBoxWidth) newTexRect.right -= x + w - boundingBoxWidth;
	if(y < 0) {
		newTexRect.top -= y;
		start_y = 0;
	}
	if(y + h > boundingBoxHeight) newTexRect.bottom -= y + h - boundingBoxHeight;

	//Rendering based on the new textRect is just ok
	g->RenderTexture(m_pTexture, start_x + m_pComponent->m_BoundingBox.left, start_y + m_pComponent->m_BoundingBox.top, &newTexRect, m_z);
}

/*
* Render Sprite After rotate and scaling, Use more complex algorithm
*/
void YmxSprite::Render(LPYMXGRAPHICS g, float x, float y, float rot, float hScale, float vScale)
{
	float w = m_TextureRect.right - m_TextureRect.left;
	float h = m_TextureRect.bottom - m_TextureRect.top;

	// first Calculate the sprite rect after Scaling
	YMXRECT r;
	r.left = m_hotSpot.x * (1 - hScale);
	r.top = m_hotSpot.y * (1 - vScale);
	r.right = (w - m_hotSpot.x) * hScale + m_hotSpot.x;
	r.bottom = (h - m_hotSpot.y) * vScale + m_hotSpot.y;

	//Second, Rotate the sprite rect, get the rotated quad
	YMXQUAD quad = {r.left, r.top, r.right, r.top, r.right, r.bottom, r.left, r.bottom};
	RotateVertex(&quad.v[0], &m_hotSpot, rot);
	RotateVertex(&quad.v[1], &m_hotSpot, rot);
	RotateVertex(&quad.v[2], &m_hotSpot, rot);
	RotateVertex(&quad.v[3], &m_hotSpot, rot);

	float shift_x = x - m_hotSpot.x;
	float shift_y = y - m_hotSpot.y;
	if(m_pComponent != NULL) {
		shift_x += m_pComponent->m_BoundingBox.left;
		shift_y += m_pComponent->m_BoundingBox.top;
	}

	quad.v[0].x += shift_x; quad.v[0].y += shift_y;
	quad.v[1].x += shift_x; quad.v[1].y += shift_y;
	quad.v[2].x += shift_x; quad.v[2].y += shift_y;
	quad.v[3].x += shift_x; quad.v[3].y += shift_y;

	// The Quad which stored texture Rect Coordinate
	YMXQUAD textQuad = {m_TextureRect.left, m_TextureRect.top, m_TextureRect.right, m_TextureRect.top, 
		m_TextureRect.right, m_TextureRect.bottom, m_TextureRect.left, m_TextureRect.bottom};

	// If no need clipping
	if(m_pComponent == NULL || !m_pComponent->m_bRequireClip || _Quad_Inside(&quad))
	{
		g->_RenderTexture_Polygon(m_pTexture, quad.v, textQuad.v, 4, m_z);
		return;
	}

	// At beginning the pts1 is stored the sprite rect coordinate, pts2 is empty 
	YMXPOINT pts1[16], pts2[16];
	memcpy((void*)pts1, (void*)quad.v, sizeof(quad.v));
	int ptsCount;
	ptsCount = _Clip_By_Bordary(LEFT, pts1, 4, pts2);
	ptsCount = _Clip_By_Bordary(RIGHT, pts2, ptsCount, pts1);
	ptsCount = _Clip_By_Bordary(BOTTOM, pts1, ptsCount, pts2);
	ptsCount = _Clip_By_Bordary(TOP, pts2, ptsCount, pts1); // the result coords is in pts1 array, size is ptsCount

	memcpy((void*)pts2, (void*)pts1, sizeof(YMXPOINT) * ptsCount);

	// Now, the valid points is in pts2
	YMXPOINT hotSpot = {x + m_pComponent->m_BoundingBox.left, y + m_pComponent->m_BoundingBox.top};
	YMXPOINT texCoords[8];
	float hScaleReciprocal = 1.0f / hScale;
	float vScaleReciprocal = 1.0f / vScale;
	// reverse-rotate the points in pts2, in order to calculate the texture coordinate
	for (int i = 0; i < ptsCount; i++)
	{
		RotateVertex(&pts2[i], &hotSpot, -rot);
		texCoords[i].x = (pts2[i].x - hotSpot.x) * hScaleReciprocal + m_TextureRect.left + m_hotSpot.x;
		texCoords[i].y = (pts2[i].y - hotSpot.y) * vScaleReciprocal + m_TextureRect.top + m_hotSpot.y;
	}

	g->_RenderTexture_Polygon(m_pTexture, pts1, texCoords, ptsCount, m_z);

}

void YmxSprite::SetHotSpot(float x, float y)
{
	m_hotSpot.x = x;
	m_hotSpot.y = y;
}

YMXPOINT YmxSprite::GetHotSpot()
{
	return m_hotSpot;
}

void YmxSprite::SetTextureRect(YMXRECT& rect)
{
	m_TextureRect = rect;
}

const YMXRECT& YmxSprite::GetTextureRect()
{
	return m_TextureRect;
}

void YmxSprite::Release()
{
	delete this;
}

void YmxSprite::SetZ(float z)
{
	m_z = RESTRICT_Z(z);
}

float YmxSprite::GetZ()
{
	return m_z;
}

YmxSprite::~YmxSprite()
{

}

bool YmxSprite::_InsideBordary(YMXBORDARY bordary, LPYMXPOINT p)
{
	switch(bordary)
	{
	case LEFT:
		if(p->x > m_pComponent->m_BoundingBox.left) return true; else return false;
	case RIGHT:
		if(p->x < m_pComponent->m_BoundingBox.right) return true; else return false;
	case BOTTOM:
		if(p->y < m_pComponent->m_BoundingBox.bottom) return true; else return false;
	case TOP:
		if(p->y > m_pComponent->m_BoundingBox.top) return true; else return false;
	}
	return true;
}

/************************************************************************/
/* Calculate the intersect point from line (p1-p2) to bordary line
/************************************************************************/
YMXPOINT YmxSprite::_Intersect_Bordary_Point(YMXBORDARY bordary, LPYMXPOINT p1, LPYMXPOINT p2)
{
	YMXPOINT pt;
	float m;
	switch(bordary)
	{
	case LEFT:
		m = (p2->y - p1->y) / (p2->x - p1->x);
		pt.x = m_pComponent->m_BoundingBox.left;
		pt.y = m * (m_pComponent->m_BoundingBox.left - p1->x) + p1->y;
		break;
	case RIGHT:
		m = (p2->y - p1->y) / (p2->x - p1->x);
		pt.x = m_pComponent->m_BoundingBox.right;
		pt.y = m * (m_pComponent->m_BoundingBox.right - p1->x) + p1->y;
		break;
	case BOTTOM:
		m = (p2->x - p1->x) / (p2->y - p1->y);
		pt.y = m_pComponent->m_BoundingBox.bottom;
		pt.x = m * (m_pComponent->m_BoundingBox.bottom - p1->y) + p1->x;
		break;
	case TOP:
		m = (p2->x - p1->x) / (p2->y - p1->y);
		pt.y = m_pComponent->m_BoundingBox.top;
		pt.x = m * (m_pComponent->m_BoundingBox.top - p1->y) + p1->x;
		break;
	}
	return pt;
}

/************************************************************************/
/* bordary : the clipped by which edge : LEFT, TOP, BOTTOM, RIGHT 
   ptsIn : The points which will be clipped.
   ptsInCount : the valid element in ptsIn
   ptsOut : The result points after clipped
   return : the valid count in ptsOut 
*/
/************************************************************************/
int YmxSprite::_Clip_By_Bordary(YMXBORDARY bordary, LPYMXPOINT ptsIn, int ptsInCount, LPYMXPOINT ptsOut)
{
	int i;
	int ptsOutCount = 0;
	int inOutCode;
	int first, second;
	YMXPOINT pt;
	for (i = 0; i < ptsInCount; i++)
	{
		first = i;	
		if(i == ptsInCount - 1) second = 0; else second = i + 1;
		inOutCode = (_InsideBordary(bordary, &ptsIn[first]) << 1) | _InsideBordary(bordary, &ptsIn[second]);
		switch(inOutCode)
		{
		case 0x03: // inside - inside
			ptsOut[ptsOutCount++] = ptsIn[second];
			break;
		case 0x02: // inside - outside
			ptsOut[ptsOutCount++] = _Intersect_Bordary_Point(bordary, &ptsIn[first], &ptsIn[second]);
			break;
		case 0x01: // outside - inside
			ptsOut[ptsOutCount++] = _Intersect_Bordary_Point(bordary, &ptsIn[first], &ptsIn[second]);
			ptsOut[ptsOutCount++] = ptsIn[second];
			break;
		default:
			break;
		}
	}
	return ptsOutCount;
}

bool YmxSprite::_Quad_Inside(LPYMXQUAD pQuad)
{
	LPYMXPOINT pts = pQuad->v;
	return pts[0].x >= m_pComponent->m_BoundingBox.left && pts[0].x <= m_pComponent->m_BoundingBox.right 
		&& pts[0].y >= m_pComponent->m_BoundingBox.top && pts[0].y <= m_pComponent->m_BoundingBox.bottom 
		&& pts[1].x >= m_pComponent->m_BoundingBox.left && pts[1].x <= m_pComponent->m_BoundingBox.right 
		&& pts[1].y >= m_pComponent->m_BoundingBox.top && pts[1].y <= m_pComponent->m_BoundingBox.bottom 
		&& pts[2].x >= m_pComponent->m_BoundingBox.left && pts[2].x <= m_pComponent->m_BoundingBox.right 
		&& pts[2].y >= m_pComponent->m_BoundingBox.top && pts[2].y <= m_pComponent->m_BoundingBox.bottom 
		&& pts[3].x >= m_pComponent->m_BoundingBox.left && pts[3].x <= m_pComponent->m_BoundingBox.right 
		&& pts[3].y >= m_pComponent->m_BoundingBox.top && pts[3].y <= m_pComponent->m_BoundingBox.bottom;
}

LPYMXTEXTURE YmxSprite::GetTexture()
{
	return m_pTexture;
}

LPYMXCOMPONENT YmxSprite::GetComponent()
{
	return m_pComponent;
}

