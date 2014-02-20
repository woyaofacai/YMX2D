#include "stdafx.h"
#include "ymx2d.h"
#include "ymxvertex.h"
#include "ymxparticle.h"

YmxExplodeParticleSystem::YmxExplodeParticleSystem(LPYMXCOMPONENT pComponent, LPYMXTEXTURE pTexture, int maxCount)
	:YmxParticleSystem(pComponent, pTexture, maxCount)
{
	m_Type = PAR_TYPE_EXPLODE;

	// init all null pointers
	m_ParVertexs = NULL;
	m_ParAttributes = NULL;
	m_ParGroups = NULL;

	m_Center.x = (m_BoundingBox.right - m_BoundingBox.left) * 0.5f;
	m_Center.y = (m_BoundingBox.bottom - m_BoundingBox.top) * 0.5f;
	m_Velocity = 10.0f;
	m_VelocityVariant = 1.0f;
	m_ParLife = 10.0f;
	m_ParLifeVariant = 0.0f;
	m_StartSize = 3.0f;
	m_EndSize = 4.0f;
	m_Delta = GetGameObject()->GetUpdateDelta();
	m_SizeVariant = 0.0f;

	m_Acceleration = 0.0f;
	m_StartAngle = 0.0f;
	m_EndAngle = 360.0f;

	m_StartColorA = m_EndColorA = 255;
	m_StartColorR = m_EndColorR = 255;
	m_StartColorG = m_EndColorG = 255;
	m_StartColorB = m_EndColorB = 255;

	m_StartColor = YMX_RGB(255, 255, 255);
	m_EndColor = YMX_RGB(255, 255, 255);
	m_RGBVariant = 0;

	m_elapseTimeSinceUpdatePSize = 0.0f;

	m_Style |= _PS_COLOR_GRADIENT;
	m_Style |= _PS_HIGHLIGHT;
	m_Style |= _PS_SIZE_GRADIENT;
	//m_Style |= _PS_EXPLODE_CONTINUOUS;
}

YmxExplodeParticleSystem::~YmxExplodeParticleSystem()
{

}

bool YmxExplodeParticleSystem::Initialize()
{

	float dt = 1.0f / m_ParLife * m_Delta;	// in order to avoid more divide;
	m_SizeIncrement = (m_EndSize - m_StartSize) * dt;
	m_AppearNumPerDelta = max(1, (int)(m_MaxCount * dt)); 

	m_StartColor = YMX_ARGB(m_StartColorA, m_StartColorR, m_StartColorG, m_StartColorB);
	m_EndColor   = YMX_ARGB(m_EndColorA, m_EndColorR, m_EndColorG, m_EndColorB);

	m_ColorIncA = (int)((float)(m_EndColorA - m_StartColorA) * dt);
	m_ColorIncR = (int)((float)(m_EndColorR - m_StartColorR) * dt);
	m_ColorIncG = (int)((float)(m_EndColorG - m_StartColorG) * dt);
	m_ColorIncB = (int)((float)(m_EndColorB - m_StartColorB) * dt);

	if(m_bSupportPSize)
	{
		m_ParVertexs = new ParticleVertex[m_MaxCount];
		m_ParAttributes = new YmxParticleAttribute[m_MaxCount];
	}
	else {
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

void YmxExplodeParticleSystem::Update(float delta)
{
	if(m_bSupportPSize)
		_Update_SupportPSize(delta);
	else 
		_Update_Not_SupportPSize(delta);
}

void YmxExplodeParticleSystem::_Update_SupportPSize(float delta)
{
	int i;
	if(!m_bStopped && m_Count < m_MaxCount)
	{
		for (i = 0; i < m_AppearNumPerDelta; i++)
		{
			_ResetParticle(m_Count);
			m_Count++;
		}
	}

	if(!(m_Style & _PS_COLOR_GRADIENT) || m_StartColor == m_EndColor)
	{
		if(m_Style & _PS_SIZE_GRADIENT) {
			for (i = 0; i < m_Count; i++)
			{
				if(m_ParAttributes[i].life < 0)
				{
					_ResetParticle(i);
				}
				else {
					m_ParVertexs[i].x += m_ParAttributes[i].velocity.x;
					m_ParVertexs[i].y += m_ParAttributes[i].velocity.y;
					m_ParVertexs[i].size += m_SizeIncrement;
					m_ParAttributes[i].velocity += m_ParAttributes[i].acceleration;
					m_ParAttributes[i].life -= delta;
				}
			}
		}
		else {
			for (i = 0; i < m_Count; i++)
			{
				if(m_ParAttributes[i].life < 0)
				{
					_ResetParticle(i);
				}
				else {
					m_ParVertexs[i].x += m_ParAttributes[i].velocity.x;
					m_ParVertexs[i].y += m_ParAttributes[i].velocity.y;
					m_ParAttributes[i].velocity += m_ParAttributes[i].acceleration;
					m_ParAttributes[i].life -= delta;
				}
			}
		}
	}
	else {
		if(m_Style & _PS_SIZE_GRADIENT)
		{
			for (i = 0; i < m_Count; i++)
			{
				if(m_ParAttributes[i].life < 0)
				{
					_ResetParticle(i);
				}
				else {
					m_ParVertexs[i].x += m_ParAttributes[i].velocity.x;
					m_ParVertexs[i].y += m_ParAttributes[i].velocity.y;
					m_ParVertexs[i].size += m_SizeIncrement;
					m_ParAttributes[i].velocity += m_ParAttributes[i].acceleration;
					m_ParAttributes[i].life -= delta;

					m_ParVertexs[i].alpha = RESTRICT_RGB(m_ParVertexs[i].alpha + m_ColorIncA);
					m_ParVertexs[i].red   = RESTRICT_RGB(m_ParVertexs[i].red + m_ColorIncR);
					m_ParVertexs[i].green = RESTRICT_RGB(m_ParVertexs[i].green + m_ColorIncG);
					m_ParVertexs[i].blue  = RESTRICT_RGB(m_ParVertexs[i].blue + m_ColorIncB);
				}
			}
		}
		else {
			for (i = 0; i < m_Count; i++)
			{
				if(m_ParAttributes[i].life < 0)
				{
					_ResetParticle(i);
				}
				else {
					m_ParVertexs[i].x += m_ParAttributes[i].velocity.x;
					m_ParVertexs[i].y += m_ParAttributes[i].velocity.y;
					m_ParAttributes[i].velocity += m_ParAttributes[i].acceleration;
					m_ParAttributes[i].life -= delta;

					m_ParVertexs[i].alpha = RESTRICT_RGB(m_ParVertexs[i].alpha + m_ColorIncA);
					m_ParVertexs[i].red   = RESTRICT_RGB(m_ParVertexs[i].red + m_ColorIncR);
					m_ParVertexs[i].green = RESTRICT_RGB(m_ParVertexs[i].green + m_ColorIncG);
					m_ParVertexs[i].blue  = RESTRICT_RGB(m_ParVertexs[i].blue + m_ColorIncB);
				}
			}
		}
	}
}

void YmxExplodeParticleSystem::_Update_Not_SupportPSize(float delta)
{
	int i;
	int groupIndex;
	if(!m_bStopped && m_Count < m_MaxCount)
	{
		for (i = 0; i < m_AppearNumPerDelta; i++)
		{
			YmxParticle *p = new YmxParticle;
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
						groupIndex = GetRandomInteger(0, m_StartSizeRange);
						p3->next = m_ParGroups[groupIndex].particles;
						m_ParGroups[groupIndex].particles = p3;
						if(p3->next == NULL)  
							m_ParGroups[groupIndex].tail = p3;
					}
				}
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

void YmxExplodeParticleSystem::Render(YmxGraphics* g)
{
	if(m_bSupportPSize)
		g->_Render_Particles(m_pTexture, m_ParVertexs, m_Count, m_Style & _PS_HIGHLIGHT);
	else {
		for (int i = 0; i < m_ParGroupNum; i++)
		{
			g->_Render_Particles(m_pTexture, m_ParGroups[i].particles, m_z, m_ParGroups[i].size, m_Style & _PS_HIGHLIGHT);
		}
	}
}


void YmxExplodeParticleSystem::_ResetParticle(YmxParticle* p)
{
	p->x = m_Center.x;
	p->y = m_Center.y;
	if(m_RGBVariant == 0)
	{
		p->color = m_StartColor;
	}else {
		p->alpha = RESTRICT_RGB(m_StartColorA + GetRandomInteger(-m_RGBVariant, m_RGBVariant));
		p->red   = RESTRICT_RGB(m_StartColorR + GetRandomInteger(-m_RGBVariant, m_RGBVariant));
		p->green = RESTRICT_RGB(m_StartColorG + GetRandomInteger(-m_RGBVariant, m_RGBVariant));
		p->blue  = RESTRICT_RGB(m_StartColorB + GetRandomInteger(-m_RGBVariant, m_RGBVariant));
	}

	float theta = GetRandomFloat(m_StartAngle, m_EndAngle);
	float velocity = m_Velocity + GetRandomFloat(-m_VelocityVariant, m_VelocityVariant);

	float cosValue = CosFastByAngle(theta);
	float sinValue = SinFastByAngle(theta);

	p->velocity.x = velocity * cosValue * m_Delta;
	p->velocity.y = velocity * sinValue * m_Delta;

	p->acceleration.x = (m_Acceleration * cosValue + m_FieldAcceleration.x) * m_Delta;
	p->acceleration.y = (m_Acceleration * sinValue + m_FieldAcceleration.y) * m_Delta;

	p->life = m_ParLife + GetRandomFloat(-m_ParLifeVariant, m_ParLifeVariant);
}

void YmxExplodeParticleSystem::_ResetParticle(int index)
{
	m_ParVertexs[index].x = m_Center.x;
	m_ParVertexs[index].y = m_Center.y;
	m_ParVertexs[index].z = m_z;
	m_ParVertexs[index].size = m_StartSize + GetRandomFloat(-m_SizeVariant, m_SizeVariant);
	if(m_RGBVariant == 0)
	{
		m_ParVertexs[index].color = m_StartColor;
	}else {
		m_ParVertexs[index].alpha = RESTRICT_RGB(m_StartColorA + GetRandomInteger(-m_RGBVariant, m_RGBVariant));
		m_ParVertexs[index].red   = RESTRICT_RGB(m_StartColorR + GetRandomInteger(-m_RGBVariant, m_RGBVariant));
		m_ParVertexs[index].green = RESTRICT_RGB(m_StartColorG + GetRandomInteger(-m_RGBVariant, m_RGBVariant));
		m_ParVertexs[index].blue  = RESTRICT_RGB(m_StartColorB + GetRandomInteger(-m_RGBVariant, m_RGBVariant));
	}

	float theta = GetRandomFloat(m_StartAngle, m_EndAngle);
	float velocity = m_Velocity + GetRandomFloat(-m_VelocityVariant, m_VelocityVariant);

	float cosValue = CosFastByAngle(theta);
	float sinValue = SinFastByAngle(theta);

	m_ParAttributes[index].velocity.x = velocity * cosValue * m_Delta;
	m_ParAttributes[index].velocity.y = velocity * sinValue * m_Delta;

	m_ParAttributes[index].acceleration.x = (m_Acceleration * cosValue + m_FieldAcceleration.x) * m_Delta;
	m_ParAttributes[index].acceleration.y = (m_Acceleration * sinValue + m_FieldAcceleration.y) * m_Delta;

	m_ParAttributes[index].life = m_ParLife + GetRandomFloat(-m_ParLifeVariant, m_ParLifeVariant);
}

void YmxExplodeParticleSystem::Release()
{
	if(m_bSupportPSize)
	{
		delete [] m_ParAttributes;
		delete [] m_ParVertexs;
	}
	else {
		int i;
		for (i = 0; i < m_ParGroupNum; i++)
		{
			_ReleaseParticleList(m_ParGroups[i].particles);
		}
		delete [] m_ParGroups;
	}
	delete this;
}

void YmxExplodeParticleSystem::SetAttribute(PARTICLE_ATTRIBUTE attr, int value)
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

void YmxExplodeParticleSystem::SetPosition(float x, float y)
{
	m_Center.x = x;
	m_Center.y = y;
}

void YmxExplodeParticleSystem::SetAttribute(PARTICLE_ATTRIBUTE attr, float value)
{
	switch(attr)
	{
	case PAR_POS_X:
		m_Center.x = value;
		break;
	case PAR_POS_Y:
		m_Center.y = value;
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
	case PAR_START_SIZE:
		m_StartSize = value;
		break;
	case PAR_END_SIZE:
		m_EndSize = value;
		break;
	case PAR_SIZE_VARIANT:
		m_SizeVariant = value;
		break;
	case PAR_ACCELERATION:
		m_Acceleration = value;
		break;
	case PAR_FIELD_ACCELERATION_X:
		m_FieldAcceleration.x = value;
		break;
	case PAR_FIELD_ACCELERATION_Y:
		m_FieldAcceleration.y = value;
		break;
	case PAR_EXPLODE_START_ANGLE:
		m_StartAngle = value * DIV_180_PI;
		break;
	case PAR_EXPLODE_END_ANGLE:
		m_EndAngle = value * DIV_180_PI;
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

bool YmxExplodeParticleSystem::GetAttribute(PARTICLE_ATTRIBUTE attr, float* value)
{
	switch(attr)
	{
	case PAR_POS_X:
		*value = m_Center.x;
		return true;
	case PAR_POS_Y:
		*value = m_Center.y;
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
	case PAR_START_SIZE:
		*value = m_StartSize;
		return true;
	case PAR_END_SIZE:
		*value = m_EndSize;
		return true;
	case PAR_SIZE_VARIANT:
		*value = m_SizeVariant;
		return true;
	case PAR_ACCELERATION:
		*value = m_Acceleration;
		return true;
	case PAR_FIELD_ACCELERATION_X:
		*value = m_FieldAcceleration.x;
		return true;
	case PAR_FIELD_ACCELERATION_Y:
		*value = m_FieldAcceleration.y;
		return true;
	case PAR_EXPLODE_START_ANGLE:
		*value = m_StartAngle * DIV_PI_180;
		return true;
	case PAR_EXPLODE_END_ANGLE:
		*value = m_EndAngle * DIV_PI_180;
		return true;
	default:
		return YmxParticleSystem::GetAttribute(attr, value);
	}
	return false;
}

bool YmxExplodeParticleSystem::GetAttribute(PARTICLE_ATTRIBUTE attr, int* value)
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
