#include "stdafx.h"
#include "ymx2d.h"
#include "ymxvertex.h"
#include "ymxparticle.h"


YmxFlameParticleSystem::YmxFlameParticleSystem(LPYMXCOMPONENT pComponent, LPYMXTEXTURE pTexture, int maxCount)
	:YmxParticleSystem(pComponent, pTexture, maxCount)
{
	m_Type = PAR_TYPE_FLAME;

	m_Particles = NULL;

	m_StartColorA = m_EndColorA = 255;
	m_StartColorR = m_EndColorR = 255;
	m_StartColorG = 255;
	m_EndColorG = 0;
	m_StartColorB = m_EndColorB = 0;

	m_RGBVariant = 0;
	m_FrameHeight = m_BoundingBox.bottom - m_BoundingBox.top;

	m_Velocity.x = 0;
	m_Velocity.y = -300.0f;

	m_ParLife = 0.35f;

	m_EndSize = 1.0f;
	m_StartSize = 10.0f;

	m_VelocityVariant.x = m_VelocityVariant.y = 1.0f;

	m_PSizeVelocity = 0.0f;

	m_bStopped = false;

	m_Acceleration.x = m_Acceleration.y = 0.0f;
	m_AccelerationVariant.x = m_AccelerationVariant.y = 0.0f;

	m_HeatSourcePoints = NULL;
	m_ParGroups = NULL;

	m_StartPoint.x = m_BoundingBox.left;
	m_EndPoint.x   = m_BoundingBox.right;
	m_StartPoint.y = m_EndPoint.y = m_BoundingBox.bottom;

	m_HeatSourcesNum = 0;
	m_ParGroupNum = 0;

	m_elapseTimeSinceUpdatePSize = 0.0f;

	m_Style |= _PS_HIGHLIGHT;
}

YmxFlameParticleSystem::~YmxFlameParticleSystem()
{

}

bool YmxFlameParticleSystem::Initialize()
{
	/* If the user not set velocity variant, then set
		velocityVariant.x = 0.33 * m_Velocity.y, 
		m_VelocityVariant.y = m_Velocity.y * 0.9f
		Which may get better effect
	*/
	if(m_VelocityVariant.x < 0)
	{
		m_VelocityVariant.x = abs(m_Velocity.y * 0.33f);
	}

	if(m_VelocityVariant.y < 0)
	{
		m_VelocityVariant.y = abs(m_Velocity.y * 0.9f);
	}

	m_ParLife = abs(m_FrameHeight / (m_Velocity.y - m_VelocityVariant.y));
	m_ParLifeVariant = m_ParLife * 0.5f;

	float delta = GetGameObject()->GetUpdateDelta();

	float dt = 1.0f / m_ParLife * delta;
	if(m_bSupportPSize)
	{
		m_PSizeVelocity = (m_StartSize - m_EndSize) / m_ParLife * delta;
	}
	else {
		m_ParGroupNum = min(floor(abs(m_StartSize - m_EndSize)) + 1, MAX_PSIZE_NUM);
		m_ParGroups = new YmxParticleGroup[m_ParGroupNum];

		float sizeInterval = (m_StartSize - m_EndSize) / m_ParGroupNum;
		float size = m_EndSize;
		for (int i = 0; i < m_ParGroupNum; i++)
		{
			m_ParGroups[i].size = size;
			size += sizeInterval;
		}
		m_PSizeVelocity = sizeInterval;
		m_UpdatePSizeDelta = m_ParLife / m_ParGroupNum;
	}
	
	m_StartColor = YMX_ARGB(m_StartColorA, m_StartColorR, m_StartColorG, m_StartColorB);
	m_EndColor   = YMX_ARGB(m_EndColorA, m_EndColorR, m_EndColorG, m_EndColorB);

	m_ColorIncA = (int)((float)(m_EndColorA - m_StartColorA) * dt);
	m_ColorIncR = (int)((float)(m_EndColorR - m_StartColorR) * dt);
	m_ColorIncG = (int)((float)(m_EndColorG - m_StartColorG) * dt);
	m_ColorIncB = (int)((float)(m_EndColorB - m_StartColorB) * dt);

	m_AppearNumPerDelta = m_MaxCount * dt;

	m_bStopped = false;

	_InitHeatSourcePoints();

	return true;
}


void YmxFlameParticleSystem::_InitAllParticles()
{
	int i;
	for (i = 0; i < m_MaxCount; i++)
	{
		YmxParticle *p = new YmxParticle;
		_ResetParticle(p);
		p->next = m_Particles;
		m_Particles = p;
		m_Count++;
	}
}

void YmxFlameParticleSystem::Update(float delta)
{
	int i;
	YmxParticle *p1, *p2, *p3;
	
	if(m_bSupportPSize) {
		if(!m_bStopped && m_Count < m_MaxCount)
		{
			for (i = 0; i < m_AppearNumPerDelta; i++)
			{
				YmxParticle *p = new YmxParticle;
				_ResetParticle(p);
				p->next = m_Particles;
				m_Particles = p;
				m_Count++;
				if(m_Count > m_MaxCount) break;
			}
		}

		p1 = m_Particles;
		p2 = NULL;
		while(p1 != NULL)
		{
			if(p1->life < 0)
			{
				if(!m_bStopped)
					_ResetParticle(p1);
				else {
					if(p2 == NULL)
					{
						m_Particles = p1->next;
						delete p1;
						p1 = m_Particles;
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
			else {
				p1->x += p1->velocity.x * delta;
				p1->y += p1->velocity.y * delta;
				p1->life -= delta;
				p1->size -= m_PSizeVelocity;
				p1->alpha = RESTRICT_RGB(p1->alpha + m_ColorIncA);
				p1->red   = RESTRICT_RGB(p1->red + m_ColorIncR);
				p1->green = RESTRICT_RGB(p1->green + m_ColorIncG);
				p1->blue  = RESTRICT_RGB(p1->blue + m_ColorIncB);
				p1->velocity += p1->acceleration;
			}

			p2 = p1;
			p1 = p1->next;
		}
	}
	else {
		if(!m_bStopped && m_Count < m_MaxCount)
		{
			for (i = 0; i < m_AppearNumPerDelta; i++)
			{
				YmxParticle *p = new YmxParticle;
				_ResetParticle(p);
				p->next = m_ParGroups[m_ParGroupNum - 1].particles;
				m_ParGroups[m_ParGroupNum - 1].particles = p;
				if(p->next == NULL) m_ParGroups[m_ParGroupNum - 1].tail = p;
				m_Count++;
				if(m_Count > m_MaxCount) break;
			}
		}

		//Update Point Size
		if(m_ParGroupNum > 1)
		{
			m_elapseTimeSinceUpdatePSize += delta;
			if(m_elapseTimeSinceUpdatePSize > m_UpdatePSizeDelta)
			{
				m_elapseTimeSinceUpdatePSize = 0;
				
				// put the second smaller size Particle group to the tail of the first Particle Group
				if(m_ParGroups[0].particles != NULL) 
					m_ParGroups[0].tail->next = m_ParGroups[1].particles;
				else 
					m_ParGroups[0].particles = m_ParGroups[1].particles;
				if(m_ParGroups[1].particles != NULL)
					m_ParGroups[0].tail = m_ParGroups[1].tail;

				for(i = 2; i < m_ParGroupNum; i++)
				{
					m_ParGroups[i - 1].particles = m_ParGroups[i].particles;
					m_ParGroups[i - 1].tail = m_ParGroups[i].tail;
				}
				m_ParGroups[m_ParGroupNum - 1].particles = NULL;
				m_ParGroups[m_ParGroupNum - 1].tail = NULL;
			}
		}


		//Update the particles, reset the dead particle and remove them to the last Particle Group 
		for (i = 0; i < m_ParGroupNum; i++)
		{
			p1 = m_ParGroups[i].particles;
			p2 = NULL;

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
					
					// TODO:
					//Update the tail, when delete

					if(p1 == NULL) m_ParGroups[i].tail = p2;

					// If has stopped, then delete the particle
					// else reset the particle and put it to Heat Source particle group again
					if (m_bStopped)
					{
						delete p3;
						m_Count--;
					}
					else {
						_ResetParticle(p3);
						if(m_ParGroupNum > 1) {
							p3->next = m_ParGroups[m_ParGroupNum - 1].particles;
							m_ParGroups[m_ParGroupNum - 1].particles = p3;
							if(p3->next == NULL)  
								m_ParGroups[m_ParGroupNum - 1].tail = p3;
						}
					}
				}
				else {
					p1->x += p1->velocity.x * delta;
					p1->y += p1->velocity.y * delta;
					p1->life -= delta;
					p1->alpha = RESTRICT_RGB(p1->alpha + m_ColorIncA);
					p1->red   = RESTRICT_RGB(p1->red + m_ColorIncR);
					p1->green = RESTRICT_RGB(p1->green + m_ColorIncG);
					p1->blue  = RESTRICT_RGB(p1->blue + m_ColorIncB);
					p1->velocity += p1->acceleration;

					p2 = p1;
					p1 = p1->next;
				}
			}
		}
	}
}

void YmxFlameParticleSystem::Render(YmxGraphics* g)
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

void YmxFlameParticleSystem::_ResetParticle(YmxParticle* p)
{
	p->alpha = RESTRICT_RGB(m_StartColorA + GetRandomInteger(-m_RGBVariant, m_RGBVariant));
	p->red = RESTRICT_RGB(m_StartColorR + GetRandomInteger(-m_RGBVariant, m_RGBVariant));
	p->green = RESTRICT_RGB(m_StartColorG + GetRandomInteger(-m_RGBVariant, m_RGBVariant));
	p->blue = RESTRICT_RGB(m_StartColorB + GetRandomInteger(-m_RGBVariant, m_RGBVariant));
	p->size = m_StartSize;
	int pointIndex = GetRandomInteger(0, m_HeatSourcesNum - 1);
	p->x = m_HeatSourcePoints[pointIndex].x;
	p->y = m_HeatSourcePoints[pointIndex].y;
	p->acceleration.x = m_Acceleration.x + GetRandomFloat(-m_AccelerationVariant.x, m_AccelerationVariant.x);
	p->acceleration.y = m_Acceleration.y + GetRandomFloat(-m_AccelerationVariant.y, m_AccelerationVariant.y);

	p->velocity.x = m_Velocity.x + GetRandomFloat(-m_VelocityVariant.x, m_VelocityVariant.x);
	p->velocity.y = m_Velocity.y + GetRandomFloat(-m_VelocityVariant.y, m_VelocityVariant.y);
	
	p->life = m_ParLife + GetRandomFloat(-m_ParLifeVariant, m_ParLifeVariant);
}


void YmxFlameParticleSystem::_InitHeatSourcePoints()
{
	float x, y, m;
	int i;

	delete [] m_HeatSourcePoints;
	float dx = abs(m_EndPoint.x - m_StartPoint.x);
	float dy = abs(m_EndPoint.y - m_StartPoint.y);

	if(dx >= dy) {
		m_HeatSourcesNum = (int)(dx + 1);
		m_HeatSourcePoints = new YmxVertex[m_HeatSourcesNum];
		if(dx < EPSILON)
		{
			m_EndPoint.x = m_StartPoint.x + 1.0f;
		}
		else if(m_StartPoint.x > m_EndPoint.x)
		{
			YMXPOINT tmp = m_StartPoint;
			m_StartPoint = m_EndPoint;
			m_EndPoint   = tmp;
		}

		x = m_StartPoint.x;
		y = m_StartPoint.y;
		m = (m_EndPoint.y - m_StartPoint.y) / (m_EndPoint.x - m_StartPoint.x);
		
		for (i = 0; i < m_HeatSourcesNum; i++)
		{
			m_HeatSourcePoints[i].x = x;
			m_HeatSourcePoints[i].y = y;
			x += 1.0f;
			y += m;
		}
	}
	else {
		m_HeatSourcesNum = (int)(dy + 1);
		m_HeatSourcePoints = new YmxVertex[m_HeatSourcesNum];
		if(dy < EPSILON)
		{
			m_EndPoint.y = m_StartPoint.y + 1.0f;
		}
		else if(m_StartPoint.y > m_EndPoint.y)
		{
			YMXPOINT tmp = m_StartPoint;
			m_StartPoint = m_EndPoint;
			m_EndPoint   = tmp;
		}
		
		x = m_StartPoint.x;
		y = m_StartPoint.y;
		m = (m_EndPoint.x - m_StartPoint.x) / (m_EndPoint.y - m_StartPoint.y);

		for (i = 0; i < m_HeatSourcesNum; i++)
		{
			m_HeatSourcePoints[i].y = y;
			m_HeatSourcePoints[i].x += m;
			y += 1.0f;
			x += m;
		}
	}

}

void YmxFlameParticleSystem::Release()
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
		delete [] m_ParGroups;
	}

	delete [] m_HeatSourcePoints;
	delete this;
}

void YmxFlameParticleSystem::SetAttribute(PARTICLE_ATTRIBUTE attr, float value)
{
	switch(attr)
	{
	case PAR_START_SIZE:
		m_StartSize = value;
		break;
	case PAR_END_SIZE:
		m_EndSize = value;
		break;
	case PAR_VELOCITY_X_VARIANT:
		m_VelocityVariant.x = value;
		break;
	case PAR_VELOCITY_Y_VARIANT:
		m_VelocityVariant.y = value;
		break;
	case PAR_VELOCITY_Y:
		m_Velocity.y = value;
		break;
	case PAR_VELOCITY_X:
		m_Velocity.x = value;
		break;
	case PAR_FLAME_HEIGHT:
		m_FrameHeight = value;
		break;
	case PAR_ACCELERATION_X:
		m_Acceleration.x = value;
		break;
	case PAR_ACCELERATION_Y:
		m_Acceleration.y = value;
		break;
	case PAR_ACCELERATION_X_VARIANT:
		m_AccelerationVariant.x = value;
		break;
	case PAR_ACCELERATION_Y_VARIANT:
		m_AccelerationVariant.y = value;
		break;
	case PAR_FLAME_START_X:
		m_StartPoint.x = value;
		break;
	case PAR_FLAME_START_Y:
		m_StartPoint.y = value;
		break;
	case PAR_FLAME_END_X:
		m_EndPoint.x = value;
		break;
	case PAR_FLAME_END_Y:
		m_EndPoint.y = value;
		break;
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
	default:
		YmxParticleSystem::SetAttribute(attr, value);
		break;
	}
}

void YmxFlameParticleSystem::SetAttribute(PARTICLE_ATTRIBUTE attr, int value)
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
	default:
		YmxParticleSystem::SetAttribute(attr, value);
		break;
	}
}

bool YmxFlameParticleSystem::GetAttribute(PARTICLE_ATTRIBUTE attr, float* value)
{
	switch(attr)
	{
	case PAR_START_SIZE:
		*value = m_StartSize;
		return true;
	case PAR_END_SIZE:
		*value = m_EndSize;
		return true;
	case PAR_VELOCITY_X_VARIANT:
		*value = m_VelocityVariant.x;
		return true;
	case PAR_VELOCITY_Y_VARIANT:
		*value = m_VelocityVariant.y;
		return true;
	case PAR_VELOCITY_Y:
		*value = m_Velocity.y;
		return true;
	case PAR_VELOCITY_X:
		*value = m_Velocity.x;
		return true;
	case PAR_FLAME_HEIGHT:
		*value = m_FrameHeight;
		return true;
	case PAR_ACCELERATION_X:
		*value = m_Acceleration.x;
		return true;
	case PAR_ACCELERATION_Y:
		*value = m_Acceleration.y;
		return true;
	case PAR_ACCELERATION_X_VARIANT:
		*value = m_AccelerationVariant.x;
		return true;
	case PAR_ACCELERATION_Y_VARIANT:
		*value = m_AccelerationVariant.y;
		return true;
	case PAR_FLAME_START_X:
		*value = m_StartPoint.x;
		return true;
	case PAR_FLAME_START_Y:
		*value = m_StartPoint.y;
		return true;
	case PAR_FLAME_END_X:
		*value = m_EndPoint.x;
		return true;
	case PAR_FLAME_END_Y:
		*value = m_EndPoint.y;
		return true;
	default:
		return YmxParticleSystem::GetAttribute(attr, value);
	}
	return false;
}

bool YmxFlameParticleSystem::GetAttribute(PARTICLE_ATTRIBUTE attr, int* value)
{
	switch(attr)
	{
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
	default:
		return YmxParticleSystem::GetAttribute(attr, value);
	}
	return false;
}

