#include "stdafx.h"
#include "ymx2d.h"
#include "ymxvertex.h"
#include "ymxparticle.h"

YmxCometParticleSystem::YmxCometParticleSystem(LPYMXCOMPONENT pComponent, LPYMXTEXTURE pTexture, int maxCount)
	:YmxParticleSystem(pComponent, pTexture, maxCount)
{
	m_Type = PAR_TYPE_COMET;

	m_Particles = NULL;
	m_FreeParticles = NULL;
	m_ParGroups = NULL;

	m_ParLife = 0.5f;
	m_ParLifeVariant = 0.1f;
	m_StartSize = 30.0f;
	m_EndSize = 1.0f;
	m_SizeVariant = 0.0f;

	m_Velocity = 30.0f;
	m_VelocityVariant = 10.0f;
	m_Density = 10;

	m_StartColorA = m_EndColorA = 255;
	m_StartColorR = m_EndColorR = 255;
	m_StartColorG = m_EndColorG = 255;
	m_StartColorB = m_EndColorB = 255;

	m_StartColor = YMX_RGB(255, 255, 255);
	m_EndColor = YMX_RGB(255, 255, 255);
	m_RGBVariant = 0;

	m_Delta = GetGameObject()->GetUpdateDelta();

	m_Acceleration = 0.0f;
	m_AccelerationVariant = 0.0f;

	m_elapseTimeSinceUpdatePSize = 0.0f;

	m_Style |= _PS_COLOR_GRADIENT;
	m_Style |= _PS_HIGHLIGHT;
	m_Style |= _PS_SIZE_GRADIENT;
	m_Style |= _PS_COMIT_BURN_ONLY_ONMOVING;
}

YmxCometParticleSystem::~YmxCometParticleSystem()
{

}

bool YmxCometParticleSystem::Initialize()
{
	float dt = 1.0f / m_ParLife * m_Delta;	// in order to avoid more divide;
	m_SizeIncrement = (m_EndSize - m_StartSize) * dt;

	m_StartColor = YMX_ARGB(m_StartColorA, m_StartColorR, m_StartColorG, m_StartColorB);
	m_EndColor   = YMX_ARGB(m_EndColorA, m_EndColorR, m_EndColorG, m_EndColorB);

	m_ColorIncA = (int)((float)(m_EndColorA - m_StartColorA) * dt);
	m_ColorIncR = (int)((float)(m_EndColorR - m_StartColorR) * dt);
	m_ColorIncG = (int)((float)(m_EndColorG - m_StartColorG) * dt);
	m_ColorIncB = (int)((float)(m_EndColorB - m_StartColorB) * dt);

	if(!m_bSupportPSize)
	{
		m_SizeVariant = abs(m_SizeVariant);
		if(m_Style | _PS_SIZE_GRADIENT)
		{
			m_ParGroupNum = min(floor(abs(m_EndSize - m_StartSize) + m_SizeVariant) + 1, MAX_PSIZE_NUM);
		}
		else {
			m_ParGroupNum = min(floor(2 * m_SizeVariant) + 1, MAX_PSIZE_NUM);
		}
		m_ParGroups = new YmxParticleGroup[m_ParGroupNum];
		float sizeInterval = (m_EndSize - m_StartSize) / m_ParGroupNum;

		float size = m_StartSize;
		if(m_StartSize < m_EndSize)
			size -= m_SizeVariant;
		else 
			size += m_SizeVariant;

		for (int i = 0; i < m_ParGroupNum; i++)
		{
			m_ParGroups[i].size = size;
			size += sizeInterval;
		}
		m_SizeIncrement = sizeInterval;
		m_UpdatePSizeDelta = m_ParLife / m_ParGroupNum;
		m_StartSizeRange = ((int)(2 * m_SizeVariant) / sizeInterval); 
	}

	return true;
}

void YmxCometParticleSystem::Update(float delta)
{
	if(m_bSupportPSize)
		_Update_SupportPSize(delta);
	else 
		_Update_Not_SupportPSize(delta);
}

void YmxCometParticleSystem::_Update_Not_SupportPSize(float delta)
{
	int i;
	YmxParticle* p;
	int groupIndex;

	if(!(m_Style & _PS_COMIT_BURN_ONLY_ONMOVING))
	{
		for (i = 0; i < m_Density; i++)
		{
			p = _GetFreeParticle();
			_ResetParticle(p);
			groupIndex = GetRandomInteger(0, m_StartSizeRange);
			p->next = m_ParGroups[groupIndex].particles;
			m_ParGroups[groupIndex].particles = p;
			if(m_ParGroups[groupIndex].particles->next == NULL)
			{
				m_ParGroups[groupIndex].tail = p;
			}
			m_Count++;
		}
	}

	if(m_Style & _PS_SIZE_GRADIENT)
	{
		//Update Point Size
		if(m_ParGroupNum > 1)
		{
			m_elapseTimeSinceUpdatePSize += delta;
			if(m_elapseTimeSinceUpdatePSize > m_UpdatePSizeDelta)
			{
				m_elapseTimeSinceUpdatePSize = 0;

				// put the second smaller size Particle group to the tail of the first Particle Group
				if(m_ParGroups[m_ParGroupNum - 1].particles != NULL) 
					m_ParGroups[m_ParGroupNum - 1].tail->next = m_ParGroups[m_ParGroupNum - 2].particles;
				else 
					m_ParGroups[m_ParGroupNum - 1].particles = m_ParGroups[m_ParGroupNum - 2].particles;

				if(m_ParGroups[m_ParGroupNum - 2].particles != NULL)
					m_ParGroups[m_ParGroupNum - 1].tail = m_ParGroups[m_ParGroupNum - 2].tail;

				for(i = m_ParGroupNum - 3; i >= 0; i--)
				{
					m_ParGroups[i + 1].particles = m_ParGroups[i].particles;
					m_ParGroups[i + 1].tail = m_ParGroups[i].tail;
				}
				m_ParGroups[0].particles = NULL;
				m_ParGroups[0].tail = NULL;
			}
		}
	}

	for (i = 0; i < m_ParGroupNum; i++)
	{
		YmxParticle *p1 = m_ParGroups[i].particles, *p2 = NULL, *p3 = NULL;
		while(p1 != NULL)
		{
			if(p1->life < 0)
			{
				// Remove the particle from current Particle Group
				if(p2 == NULL)
				{
					m_ParGroups[i].particles = p1->next;
				}
				else {
					p2->next = p1->next;
				}
				p3 = p1;
				p1 = p1->next;
				if(p1 == NULL) m_ParGroups[i].tail = p2;

				// recycle the p3
				p3->next = m_FreeParticles;
				m_FreeParticles = p3;
				m_Count--;
			}
			else {
				p1->x += p1->velocity.x;
				p1->y += p1->velocity.y;
				p1->velocity += p1->acceleration;
				p1->life -= delta;

				if(m_Style & _PS_COLOR_GRADIENT && m_StartColor != m_EndColor)
				{
					p1->alpha = RESTRICT_RGB(p1->alpha + m_ColorIncA);
					p1->red   = RESTRICT_RGB(p1->red + m_ColorIncR);
					p1->green = RESTRICT_RGB(p1->green + m_ColorIncG);
					p1->blue  = RESTRICT_RGB(p1->blue + m_ColorIncB);
				}

				p2 = p1;
				p1 = p1->next;
			}
		}
	}

}

void YmxCometParticleSystem::_Update_SupportPSize(float delta)
{
	int i;
	YmxParticle* p;
	if(!(m_Style & _PS_COMIT_BURN_ONLY_ONMOVING))
	{
		for (i = 0; i < m_Density; i++)
		{
			p = _GetFreeParticle();
			_ResetParticle(p);
			p->next = m_Particles;
			m_Particles = p;
			m_Count++;
		}
	}

	YmxParticle* p1 = m_Particles, *p2 = NULL, *p3 = NULL;
	while(p1 != NULL)
	{
		if (p1->life < 0)
		{
			if(p2 == NULL)
			{
				m_Particles = p1->next;
				p3 = p1;
				p1 = m_Particles;
			}
			else {
				p2->next = p1->next;
				p3 = p1;
				p1 = p2->next;
			}

			p3->next = m_FreeParticles;
			m_FreeParticles = p3;
		}
		else {
			p1->life -= delta;
			p1->x += p1->velocity.x;
			p1->y += p1->velocity.y;

			if(m_Style & _PS_SIZE_GRADIENT)
				p1->size += m_SizeIncrement;

			if(m_Style & _PS_COLOR_GRADIENT && m_StartColor != m_EndColor)
			{
				p1->alpha = RESTRICT_RGB(p1->alpha + m_ColorIncA);
				p1->red   = RESTRICT_RGB(p1->red + m_ColorIncR);
				p1->green = RESTRICT_RGB(p1->green + m_ColorIncG);
				p1->blue  = RESTRICT_RGB(p1->blue + m_ColorIncB);
			}

			p1->velocity += p1->acceleration;

			p2 = p1;
			p1 = p1->next;
		}
	}
}


void YmxCometParticleSystem::SetPosition(float x, float y)
{
	if(m_Style & _PS_COMIT_BURN_ONLY_ONMOVING)
	{
		if(TWO_FLOATS_EQUAL(x, m_Position.x) && TWO_FLOATS_EQUAL(y, m_Position.y)) return;
		m_Position.x = x;
		m_Position.y = y;

		YmxParticle *p;
		int i;
		if(m_bSupportPSize)
		{
			for (i = 0; i < m_Density; i++)
			{
				p = _GetFreeParticle();
				_ResetParticle(p);
				p->next = m_Particles;
				m_Particles = p;
				m_Count++;
			}
		}
		else {
			int groupIndex;
			for (i = 0; i < m_Density; i++)
			{
				p = _GetFreeParticle();
				_ResetParticle(p);
				groupIndex = GetRandomInteger(0, m_StartSizeRange);
				p->next = m_ParGroups[groupIndex].particles;
				m_ParGroups[groupIndex].particles = p;
				if(m_ParGroups[groupIndex].particles->next == NULL)
				{
					m_ParGroups[groupIndex].tail = p;
				}
				m_Count++;
			}
		}
	}
	else {
		m_Position.x = x;
		m_Position.y = y;
	}
}


void YmxCometParticleSystem::Render(YmxGraphics* g)
{
	if(m_bSupportPSize)
		g->_Render_Particles(m_pTexture, m_Particles, m_z, 0.0f, m_Style & _PS_HIGHLIGHT);
	else {
		for (int i = 0; i < m_ParGroupNum; i++)
		{
			g->_Render_Particles(m_pTexture, m_ParGroups[i].particles, m_z, m_ParGroups[i].size, m_Style & _PS_HIGHLIGHT);
		}
	}
}


void YmxCometParticleSystem::_ResetParticle(YmxParticle* p)
{
	p->x = m_Position.x;
	p->y = m_Position.y;

	if(m_RGBVariant == 0)
	{
		p->color = m_StartColor;
	}else {
		p->alpha = RESTRICT_RGB(m_StartColorA + GetRandomInteger(-m_RGBVariant, m_RGBVariant));
		p->red   = RESTRICT_RGB(m_StartColorR + GetRandomInteger(-m_RGBVariant, m_RGBVariant));
		p->green = RESTRICT_RGB(m_StartColorG + GetRandomInteger(-m_RGBVariant, m_RGBVariant));
		p->blue  = RESTRICT_RGB(m_StartColorB + GetRandomInteger(-m_RGBVariant, m_RGBVariant));
	}

	float theta = GetRandomFloat(0.0f, 360.0f);
	float sinMulDelta = SinFastByAngle(theta) * m_Delta;
	float cosMulDelta = CosFastByAngle(theta) * m_Delta;

	float velocity = m_Velocity + GetRandomFloat(-m_VelocityVariant, m_VelocityVariant);
	float acceleration = m_Acceleration + GetRandomFloat(-m_AccelerationVariant, m_AccelerationVariant);
	p->velocity.x = velocity * cosMulDelta;
	p->velocity.y = velocity * sinMulDelta;
	p->acceleration.x = acceleration * cosMulDelta;
	p->acceleration.y = acceleration * sinMulDelta;
	p->size = m_StartSize + GetRandomFloat(-m_SizeVariant, m_SizeVariant);
	p->life = m_ParLife + GetRandomFloat(-m_ParLifeVariant, m_ParLifeVariant);
}

YmxParticle* YmxCometParticleSystem::_GetFreeParticle()
{
	YmxParticle* p = NULL;
	if(m_FreeParticles == NULL)
	{
		p = new YmxParticle;
	}
	else {
		p = m_FreeParticles;
		m_FreeParticles = m_FreeParticles->next;
	}
	return p;
}

void YmxCometParticleSystem::Release()
{
	if(m_bSupportPSize)
	{
		_ReleaseParticleList(m_Particles);
	}
	else {
		int i;
		for (i = 0; i < m_ParGroupNum; i++)
		{
			_ReleaseParticleList(m_ParGroups[i].particles);
		}
	}

	_ReleaseParticleList(m_FreeParticles);
	delete this;
}


void YmxCometParticleSystem::SetAttribute(PARTICLE_ATTRIBUTE attr, float value)
{
	switch(attr)
	{
	case PAR_START_COLOR_A:
		m_StartColorA = FToRGB(value);
		break;
	case PAR_START_COLOR_R:
		m_StartColorR = FToRGB(value);
		break;
	case PAR_START_COLOR_G:
		m_StartColorG = FToRGB(value);
		break;
	case PAR_START_COLOR_B:
		m_StartColorB = FToRGB(value);
		break;
	case PAR_END_COLOR_A:
		m_EndColorA = FToRGB(value);
		break;
	case PAR_END_COLOR_R:
		m_EndColorR = FToRGB(value);
		break;
	case PAR_END_COLOR_G:
		m_EndColorG = FToRGB(value);
		break;
	case PAR_END_COLOR_B:
		m_EndColorB = FToRGB(value);
		break;
	case PAR_POS_X:
		m_Position.x = value;
		break;
	case PAR_POS_Y:
		m_Position.y = value;
		break;
	case PAR_LIFE:
		m_ParLife = value;
		break;
	case PAR_LIFE_VARIANT:
		m_ParLifeVariant = value;
		break;
	case PAR_VELOCITY:
		m_Velocity = value;
		break;
	case PAR_VELOCITY_VARIANT:
		m_VelocityVariant = value;
		break;
	case PAR_COMET_DENSITY:
		m_Density = (int)value;
		break;
	case PAR_ACCELERATION:
		m_Acceleration = value;
		break;
	case PAR_ACCELERATION_VARIANT:
		m_AccelerationVariant = value;
		break;
	case PAR_START_SIZE:
		m_StartSize = value;
		break;
	case PAR_END_SIZE:
		m_EndSize = value;
		break;
	case PAR_SIZE_VARIANT:
		m_SizeVariant = value;
		break;
	default:
		YmxParticleSystem::SetAttribute(attr, value);
		break;
	}
}

void YmxCometParticleSystem::SetAttribute(PARTICLE_ATTRIBUTE attr, int value)
{
	switch(attr)
	{
	case PAR_START_COLOR_A:
		m_StartColorA = RESTRICT_RGB(value);
		break;
	case PAR_START_COLOR_R:
		m_StartColorR = RESTRICT_RGB(value);
		break;
	case PAR_START_COLOR_G:
		m_StartColorG = RESTRICT_RGB(value);
		break;
	case PAR_START_COLOR_B:
		m_StartColorB = RESTRICT_RGB(value);
		break;
	case PAR_END_COLOR_A:
		m_EndColorA = RESTRICT_RGB(value);
		break;
	case PAR_END_COLOR_R:
		m_EndColorR = RESTRICT_RGB(value);
		break;
	case PAR_END_COLOR_G:
		m_EndColorG = RESTRICT_RGB(value);
		break;
	case PAR_END_COLOR_B:
		m_EndColorB = RESTRICT_RGB(value);
		break;
	case PAR_COMET_DENSITY:
		m_Density = value;
		break;
	default:
		YmxParticleSystem::SetAttribute(attr, value);
		break;
	}
}

bool YmxCometParticleSystem::GetAttribute(PARTICLE_ATTRIBUTE attr, int* value)
{
	switch(attr) {
	case PAR_START_COLOR_A:
		*value = m_StartColorA;
		return true;
	case PAR_START_COLOR_R:
		*value = m_StartColorR;
		return true;
	case PAR_START_COLOR_G:
		*value = m_StartColorG;
		return true;
	case PAR_START_COLOR_B:
		*value = m_StartColorB;
		return true;
	case PAR_END_COLOR_A:
		*value = m_EndColorA;
		return true;
	case PAR_END_COLOR_R:
		*value = m_EndColorR;
		return true;
	case PAR_END_COLOR_G:
		*value = m_EndColorG;
		return true;
	case PAR_END_COLOR_B:
		*value = m_EndColorB;
		return true;
	case PAR_COMET_DENSITY:
		*value = m_Density;
		return true;
	default:
		return YmxParticleSystem::GetAttribute(attr, value);
	}
	return false;
}

bool YmxCometParticleSystem::GetAttribute(PARTICLE_ATTRIBUTE attr, float* value)
{
	switch(attr)
	{
	case PAR_POS_X:
		*value = m_Position.x;
		return true;
	case PAR_POS_Y:
		*value = m_Position.y;
		return true;
	case PAR_LIFE:
		*value = m_ParLife; 
		return true;
	case PAR_LIFE_VARIANT:
		*value = m_ParLifeVariant;
		return true;
	case PAR_VELOCITY:
		*value = m_Velocity;
		return true;
	case PAR_VELOCITY_VARIANT:
		*value = m_VelocityVariant;
		return true;
	case PAR_COMET_DENSITY:
		*value = m_Density;
		return true;
	case PAR_ACCELERATION:
		*value = m_Acceleration;
		return true;
	case PAR_ACCELERATION_VARIANT:
		*value = m_AccelerationVariant;
		return true;
	case PAR_START_SIZE:
		*value = m_StartSize;
		return true;
	case PAR_END_SIZE:
		*value = m_EndSize;
		return true;
	case PAR_SIZE_VARIANT:
		*value = m_SizeVariant;
		return true;
	default:
		return YmxParticleSystem::GetAttribute(attr, value);
	}
	return false;
}
