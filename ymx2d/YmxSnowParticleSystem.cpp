#include "stdafx.h"
#include "ymx2d.h"
#include "ymxvertex.h"
#include "ymxparticle.h"

#define PARTICLES_APPEAR_PERIOD 0.1f
 

YmxSnowParticleSystem::YmxSnowParticleSystem(LPYMXCOMPONENT pComponent, LPYMXTEXTURE pTexture, int maxCount)
	:YmxParticleSystem(pComponent, pTexture, maxCount)
{
	m_Type = PAR_TYPE_SNOW;
	m_MaxCount = maxCount;
	m_elapseTimeSinceAppearPeriod = 0.0f;
	m_bStopped = false;
	m_Particles = NULL;
	m_Count = 0;
	m_MaxSize = m_MinSize = 10.0f;
	m_MinColorA = m_MaxColorA = m_MinColorR = m_MaxColorR = m_MinColorB = m_MaxColorB = m_MinColorG = m_MaxColorG = 255;

	m_MinVelocity.x = m_MaxVelocity.x = 0.0f;
	m_MinVelocity.y = m_MaxVelocity.y = -10.f;
}

bool YmxSnowParticleSystem::Initialize()
{
	int i;
	_InitAppearRatesOnBordary();
	_InitPeriodAppearNum();

	if(!m_bSupportPSize)
	{
		m_psizeNum = floor(m_MaxSize - m_MinSize) + 1;
		if(m_psizeNum > MAX_PSIZE_NUM) m_psizeNum = MAX_PSIZE_NUM;
		m_ParsGroups = new YmxParticleGroup[m_psizeNum];
		float sizeInterval = (m_MaxSize - m_MinSize) / (m_psizeNum - 1);
		float psize = m_MinSize;
		for (i = 0; i < m_psizeNum; i++)
		{
			m_ParsGroups[i].size = psize;
			psize += sizeInterval;
		}
	}
	m_Count = 0;
	
	// If the snow is not appear from nothing, then add a large number of snow particles.
	if(!(m_Style & _PS_FROM_NOTING)) {

		for (i = 0; i < m_MaxCount; i++)
		{
			_AddParticle(false);
		}	
	}

	return true;
}

void YmxSnowParticleSystem::Update(float delta)
{
	int i;
	// if the m_count is not reached the maxCount, then continue to add particles
	if(!m_bStopped && m_Count < m_MaxCount)
	{
		m_elapseTimeSinceAppearPeriod += delta;
		if(m_elapseTimeSinceAppearPeriod > PARTICLES_APPEAR_PERIOD)
		{
			m_elapseTimeSinceAppearPeriod = 0.0f;
			for (i = 0; i < m_PeriodAppearNum; i++)
			{
				_AddParticle(true);
				if(m_Count >= m_MaxCount) break;
			}
		}
	}

	if(m_bSupportPSize)
	{
		_UpdateParticleList(&m_Particles, delta);
	}
	else {
		for (i = 0; i < m_psizeNum; i++)
		{
			if(m_ParsGroups[i].particles != NULL)
				_UpdateParticleList(&m_ParsGroups[i].particles, delta);
		}
	}
}

void YmxSnowParticleSystem::_UpdateParticleList(YmxParticle** pList, float delta)
{
	YmxParticle* p1 = *pList, *p2 = NULL;
	while(p1 != NULL)
	{
		// if the particle is out of bounding box
		if(p1->x < m_BoundingBox.left || p1->x > m_BoundingBox.right 
			|| p1->y < m_BoundingBox.top || p1->y > m_BoundingBox.bottom)
		{
			if(!m_bStopped && m_Count <= m_MaxCount) {
				_ResetParticle(p1, true);
			}
			else {
				if(p2 == NULL) // p1 is the header
				{
					*pList = p1->next;
					delete p1;
					p1 = *pList;
				}
				else {
					p2->next = p1->next;
					delete p1;
					p1 = p2->next;
				}
				m_Count--;
				continue;
			}
		}
		else{
			p1->x += p1->velocity.x * delta;
			p1->y += p1->velocity.y * delta;
		}
		p2 = p1;
		p1 = p1->next;
	}
}

void YmxSnowParticleSystem::Render(YmxGraphics* g)
{
	if(m_bSupportPSize)
	{
		g->_Render_Particles(m_pTexture, m_Particles, m_z, 0.0f);
	}
	else {
		int  i;
		for (i = 0; i < m_psizeNum; i++)
		{
			g->_Render_Particles(m_pTexture, m_ParsGroups[i].particles, m_z, m_ParsGroups[i].size);
		}
	}
}

void YmxSnowParticleSystem::_ResetParticle(YmxParticle* p, bool bOnBordary)
{
	if(bOnBordary) {
		float k = GetRandomFloat(0.0f, 1.0f);

		if(k < m_AppearRatesOnBordary[LEFT])
		{
			p->x = m_BoundingBox.left;
			p->y = GetRandomFloat(m_BoundingBox.top, m_BoundingBox.bottom);
		}
		else if(k < m_AppearRatesOnBordary[RIGHT])
		{
			p->x = m_BoundingBox.right;
			p->y = GetRandomFloat(m_BoundingBox.top, m_BoundingBox.bottom);
		}
		else if(k < m_AppearRatesOnBordary[BOTTOM])
		{
			p->x = GetRandomFloat(m_BoundingBox.left, m_BoundingBox.right);
			p->y = m_BoundingBox.bottom;
		}
		else {
			p->x = GetRandomFloat(m_BoundingBox.left, m_BoundingBox.right);
			p->y = m_BoundingBox.top;
		}
	}
	else {
		p->x = GetRandomFloat(m_BoundingBox.left, m_BoundingBox.right);
		p->y = GetRandomFloat(m_BoundingBox.top, m_BoundingBox.bottom);
	}
	p->velocity.x = GetRandomFloat(m_MinVelocity.x, m_MaxVelocity.x);
	p->velocity.y = GetRandomFloat(m_MinVelocity.y, m_MaxVelocity.y);
	p->color = _GetRandomColor();
}

void YmxSnowParticleSystem::_InitAppearRatesOnBordary()
{
	float rates[4] = {0};
	int hBordary;
	int vBordary;
	float bodaryHeight = m_BoundingBox.bottom - m_BoundingBox.top;
	float bodaryWidth = m_BoundingBox.right - m_BoundingBox.left;

	if(m_MinVelocity.x < 0)
		vBordary = RIGHT;
	else 
		vBordary = LEFT;

	if(m_MinVelocity.y > 0)
		hBordary = TOP;
	else 
		hBordary = BOTTOM;

	rates[hBordary] += abs(m_MinVelocity.y) / (abs(m_MinVelocity.x) + abs(m_MinVelocity.y)) * bodaryWidth;
	rates[vBordary] += abs(m_MinVelocity.x) / (abs(m_MinVelocity.x) + abs(m_MinVelocity.y)) * bodaryHeight;


	if(m_MaxVelocity.x < 0)
		vBordary = RIGHT;
	else 
		vBordary = LEFT;

	if(m_MaxVelocity.y > 0)
		hBordary = TOP;
	else 
		hBordary = BOTTOM;

	rates[hBordary] += abs(m_MaxVelocity.y) / (abs(m_MaxVelocity.x) + abs(m_MaxVelocity.y)) * bodaryWidth;
	rates[vBordary] += abs(m_MaxVelocity.x) / (abs(m_MaxVelocity.x) + abs(m_MaxVelocity.y)) * bodaryHeight;

	float sumRecip = 1.0f / (rates[0] + rates[1] + rates[2] + rates[3]);
	m_AppearRatesOnBordary[0] = rates[0] * sumRecip;
	m_AppearRatesOnBordary[1] = rates[1] * sumRecip + m_AppearRatesOnBordary[0];
	m_AppearRatesOnBordary[2] = rates[2] * sumRecip + m_AppearRatesOnBordary[1];
	m_AppearRatesOnBordary[3] = rates[3] * sumRecip + m_AppearRatesOnBordary[2];
}

// Calculate the number of particles appear during one period(0.1s), when beginning.
void YmxSnowParticleSystem::_InitPeriodAppearNum()
{
	float avgVx = (abs(m_MinVelocity.x) + abs(m_MaxVelocity.x)) * 0.5f;
	float avgVy = (abs(m_MinVelocity.y) + abs(m_MaxVelocity.y)) * 0.5f;

	if(TWO_FLOATS_EQUAL(avgVx, 0.0f) && TWO_FLOATS_EQUAL(avgVy, 0.0f))
	{
		m_PeriodAppearNum = 1;
		return;
	}

	float appearParticlePeriod;
	if(avgVy > avgVx)
	{
		float height = m_BoundingBox.bottom - m_BoundingBox.top;
		appearParticlePeriod = height / (avgVy * m_MaxCount);
	}
	else {
		float width = m_BoundingBox.right - m_BoundingBox.left;
		appearParticlePeriod = width / (avgVx * m_MaxCount);
	}

	m_PeriodAppearNum = max(1, (int)(PARTICLES_APPEAR_PERIOD / appearParticlePeriod));
}

void YmxSnowParticleSystem::_AddParticle(bool bOnBordary)
{
	YmxParticle *p = new YmxParticle;
	_ResetParticle(p, bOnBordary);
	p->next = NULL;

	if(m_bSupportPSize)
	{
		p->size = GetRandomFloat(m_MinSize, m_MaxSize);
		p->next = m_Particles;
		m_Particles = p;
	}
	else {
		int index = GetRandomInteger(0, m_psizeNum - 1);
		p->next = m_ParsGroups[index].particles;
		m_ParsGroups[index].particles = p;
	}
	m_Count++;
}

void YmxSnowParticleSystem::Release()
{
	if(m_bSupportPSize)
	{
		_ReleaseParticleList(m_Particles);
	}
	else {
		int i;
		YmxParticle *p1, *p2 = NULL;
		for (i = 0; i < m_psizeNum; i++)
		{
			_ReleaseParticleList(m_ParsGroups[i].particles);
		}
		delete [] m_ParsGroups;
	}
	delete this;
}



YMXCOLOR YmxSnowParticleSystem::_GetRandomColor()
{
	int r = GetRandomInteger(m_MinColorR, m_MaxColorR);
	int g = GetRandomInteger(m_MinColorG, m_MaxColorG);
	int b = GetRandomInteger(m_MinColorB, m_MaxColorB);
	int a = GetRandomInteger(m_MinColorA, m_MaxColorA);
	return YMX_ARGB(a, r, g, b);
}

void YmxSnowParticleSystem::SetAttribute(PARTICLE_ATTRIBUTE attr, int value)
{
	switch(attr)
	{
	case PAR_COLOR_MIN_R:
		m_MinColorR = RESTRICT_RGB(value);
		break;
	case PAR_COLOR_MAX_R:
		m_MaxColorR = RESTRICT_RGB(value);
		break;
	case PAR_COLOR_MIN_G:
		m_MinColorG = RESTRICT_RGB(value);
		break;
	case PAR_COLOR_MAX_G:
		m_MaxColorG = RESTRICT_RGB(value);
		break;
	case PAR_COLOR_MIN_B:
		m_MinColorB = RESTRICT_RGB(value);
		break;
	case PAR_COLOR_MAX_B:
		m_MaxColorB = RESTRICT_RGB(value);
		break;
	case PAR_COLOR_R:
		m_MinColorR = m_MaxColorR = RESTRICT_RGB(value);
		break;
	case PAR_COLOR_G:
		m_MinColorG = m_MaxColorG = RESTRICT_RGB(value);
		break;
	case PAR_COLOR_B:
		m_MinColorB = m_MaxColorB = RESTRICT_RGB(value);
		break;
	case PAR_COLOR_MIN_A:
		m_MinColorA = RESTRICT_RGB(value);
		break;
	case PAR_COLOR_MAX_A:
		m_MaxColorA = RESTRICT_RGB(value);
		break;
	case PAR_COLOR_A:
		m_MinColorA = m_MaxColorA = RESTRICT_RGB(value);
		break;
	default:
		YmxParticleSystem::SetAttribute(attr, value);
		break;
	}
}

void YmxSnowParticleSystem::SetAttribute(PARTICLE_ATTRIBUTE attr, float value)
{
	switch(attr)
	{
	case PAR_MIN_SIZE:
		m_MinSize = value;
		break;
	case PAR_MAX_SIZE:
		m_MaxSize = value;
		break;
	case PAR_SIZE:
		m_MaxSize = m_MinSize = value;
		break;
	case PAR_COLOR_MIN_R:
		m_MinColorR = FToRGB(value);
		break;
	case PAR_COLOR_MAX_R:
		m_MaxColorR = FToRGB(value);
		break;
	case PAR_COLOR_MIN_G:
		m_MinColorG = FToRGB(value);
		break;
	case PAR_COLOR_MAX_G:
		m_MaxColorG = FToRGB(value);
		break;
	case PAR_COLOR_MIN_B:
		m_MinColorB = FToRGB(value);
		break;
	case PAR_COLOR_MAX_B:
		m_MaxColorB = FToRGB(value);
		break;
	case PAR_COLOR_MIN_A:
		m_MinColorA = FToRGB(value);
		break;
	case PAR_COLOR_MAX_A:
		m_MaxColorA = FToRGB(value);
		break;
	case PAR_COLOR_R:
		m_MinColorR = m_MaxColorR = FToRGB(value);
		break;
	case PAR_COLOR_G:
		m_MinColorG = m_MaxColorG = FToRGB(value);
		break;
	case PAR_COLOR_B:
		m_MinColorB = m_MaxColorB = FToRGB(value);
		break;
	case PAR_COLOR_A:
		m_MinColorA = m_MaxColorA = FToRGB(value);
		break;
	case PAR_MIN_VELOCITY_X:
		m_MinVelocity.x = value;
		break;
	case PAR_MIN_VELOCITY_Y:
		m_MinVelocity.y = value;
		break;
	case PAR_MAX_VELOCITY_X:
		m_MaxVelocity.x = value;
		break;
	case PAR_MAX_VELOCITY_Y:
		m_MaxVelocity.y = value;
		break;
	case PAR_VELOCITY_X:
		m_MinVelocity.x = m_MaxVelocity.x = value;
		break;
	case PAR_VELOCITY_Y:
		m_MinVelocity.y = m_MaxVelocity.y = value;
		break;
	case PAR_BOUNDING_LEFT:
		m_BoundingBox.left = value;
		break;
	case PAR_BOUNDING_RIGHT:
		m_BoundingBox.right = value;
		break;
	case PAR_BOUNDING_TOP:
		m_BoundingBox.top = value;
		break;
	case PAR_BOUNDING_BOTTOM:
		m_BoundingBox.bottom = value;
		break;
	default:
		YmxParticleSystem::SetAttribute(attr, value);
		break;
	}
}

bool YmxSnowParticleSystem::GetAttribute(PARTICLE_ATTRIBUTE attr, float* value)
{
	switch(attr)
	{
	case PAR_MIN_SIZE:
		*value = m_MinSize;
		return true;
	case PAR_MAX_SIZE:
		*value = m_MaxSize;
		return true;
	case PAR_MIN_VELOCITY_X:
		*value = m_MinVelocity.x;
		return true;
	case PAR_MIN_VELOCITY_Y:
		*value = m_MinVelocity.y;
		return true;
	case PAR_MAX_VELOCITY_X:
		*value = m_MaxVelocity.x;
		return true;
	case PAR_MAX_VELOCITY_Y:
		*value = m_MaxVelocity.y;
		return true;
	case PAR_BOUNDING_LEFT:
		*value = m_BoundingBox.left;
		break;
	case PAR_BOUNDING_RIGHT:
		*value = m_BoundingBox.right;
		break;
	case PAR_BOUNDING_TOP:
		*value = m_BoundingBox.top;
		break;
	case PAR_BOUNDING_BOTTOM:
		*value = m_BoundingBox.bottom;
		break;
	default:
		return YmxParticleSystem::GetAttribute(attr, value);
	}
	return false;
}

bool YmxSnowParticleSystem::GetAttribute(PARTICLE_ATTRIBUTE attr, int* value)
{
	switch(attr)
	{
	case PAR_COLOR_MIN_R:
		*value = m_MinColorR;
		return true;
	case PAR_COLOR_MAX_R:
		*value = m_MaxColorR;
		return true;
	case PAR_COLOR_MIN_G:
		*value = m_MinColorG;
		return true;
	case PAR_COLOR_MAX_G:
		*value = m_MaxColorG;
		return true;
	case PAR_COLOR_MIN_B:
		*value = m_MinColorB;
		return true;
	case PAR_COLOR_MAX_B:
		*value = m_MaxColorB;
		return true;
	case PAR_COLOR_MIN_A:
		*value = m_MinColorA;
		return true;
	case PAR_COLOR_MAX_A:
		*value = m_MaxColorA;
		return true;
	default:
		return YmxParticleSystem::GetAttribute(attr, value);
	}
	return false;
}
