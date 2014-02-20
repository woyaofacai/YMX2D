#include "stdafx.h"
#include "ymx2d.h"
#include "ymxvertex.h"
#include "ymxparticle.h"

YmxParticleSystem::YmxParticleSystem(LPYMXCOMPONENT pComponent, LPYMXTEXTURE pTexture, int maxCount)
	:m_pComponent(pComponent),m_pTexture(pTexture), m_MaxCount(maxCount)
{
	m_bSupportPSize = GetGameObject()->_IsSupporFVFPSize();

	if(pComponent != 0)
	{
		m_z = pComponent->GetZ();
		m_BoundingBox = pComponent->GetBoundingBox();
	}
	else {
		m_z = 0.0f;
		m_BoundingBox = YMXRECT(0, 0, GetGameObject()->GetWindowWidth(), GetGameObject()->GetWindowHeight());
	}

	m_Count = 0;
	
	m_Style = 0;
	m_bStopped = false;
}

YmxParticleSystem::~YmxParticleSystem()
{

}

void YmxParticleSystem::SetZ(float z)
{
	m_z = z;
}

float YmxParticleSystem::GetZ()
{
	return m_z;
}

void YmxParticleSystem::SetTexture(LPYMXTEXTURE pTexture)
{
	m_pTexture = pTexture;
}

LPYMXTEXTURE YmxParticleSystem::GetTexture()
{
	return m_pTexture;
}

void YmxParticleSystem::SetAttribute(PARTICLE_ATTRIBUTE attr, float value)
{
	switch(attr)
	{
	case PAR_Z:
		m_z = value;
		break;
	default:
		break;
	}
}

void YmxParticleSystem::SetAttribute(PARTICLE_ATTRIBUTE attr, int value)
{
	switch(attr)
	{
	case PAR_MAX_COUNT:
		m_MaxCount = value;
		break;
	default:
		SetAttribute(attr, (float)value);
		break;
	}
}

bool YmxParticleSystem::GetAttribute(PARTICLE_ATTRIBUTE attr, float* value)
{
	switch(attr)
	{
	case PAR_Z:
		*value = m_z;
		return true;
	}
	return false;
}

bool YmxParticleSystem::GetAttribute(PARTICLE_ATTRIBUTE attr, int* value)
{
	switch(attr)
	{
	case PAR_MAX_COUNT:
		*value = m_Count;
		return true;
	}
	return false;
}

void YmxParticleSystem::SetAttribute(PARTICLE_ATTRIBUTE attr, bool value)
{
	DWORD styleCode = 0;
	switch(attr)
	{
	case PAR_FROM_NOTING:
		styleCode = _PS_FROM_NOTING;
		break;
	case PAR_COLOR_GRADIENT:
		styleCode = _PS_COLOR_GRADIENT;
		break;
	case PAR_HIGHLIGHT:
		styleCode = _PS_HIGHLIGHT;
		break;
	case PAR_SIZE_GRADIENT:
		styleCode = _PS_SIZE_GRADIENT;
		break;
	case PAR_EXPLODE_CONTINUOUS:
		styleCode = _PS_EXPLODE_CONTINUOUS;
		break;
	case PAR_COMIT_BURN_ONLY_ONMOVING:
		styleCode = _PS_COMIT_BURN_ONLY_ONMOVING;
		break;
	default:
		break;
	}

	if(styleCode != 0)
	{
		if(value) {
			m_Style |= styleCode;
		}
		else {
			m_Style &= ~styleCode;
		}
	}
}

bool YmxParticleSystem::GetAttribute(PARTICLE_ATTRIBUTE attr, bool* value)
{
	switch(attr)
	{
	case PAR_FROM_NOTING:
		*value = (bool) m_Style & _PS_FROM_NOTING;
		return true;
	case PAR_COLOR_GRADIENT:
		*value = (bool) m_Style & _PS_COLOR_GRADIENT;
		return true;
	case PAR_HIGHLIGHT:
		*value = (bool) m_Style & _PS_HIGHLIGHT;
		return true;
	case PAR_SIZE_GRADIENT:
		*value = (bool) m_Style & _PS_SIZE_GRADIENT;
		return true;
	case PAR_EXPLODE_CONTINUOUS:
		*value = (bool) m_Style & _PS_EXPLODE_CONTINUOUS;
		return true;
	case PAR_COMIT_BURN_ONLY_ONMOVING:
		*value = (bool) m_Style & _PS_COMIT_BURN_ONLY_ONMOVING;
		return true;
	default:
		break;
	}
	return false;
}

void YmxParticleSystem::Stop()
{
	m_bStopped = true;
}

void YmxParticleSystem::Restart()
{
	m_bStopped = false;
}

void YmxParticleSystem::SetPosition(float x, float y)
{

}

void YmxParticleSystem::_ReleaseParticleList(YmxParticle* particles)
{
	YmxParticle *p1, *p2 = NULL;
	p1 = particles;
	while(p1 != NULL)
	{
		p2 = p1;
		p1 = p1->next;
		delete p2;
	}
}

LPPARTICLEYMXTEM YmxParticleSystem::CreateParticleSystem(LPYMXCOMPONENT pComponent, 
	PARTICLESYSTEM_TYPE parType, LPYMXTEXTURE pTexture, int maxCount)
{
	LPPARTICLEYMXTEM p = NULL;
	switch(parType)
	{
	case PAR_TYPE_SNOW:
		p = new YmxSnowParticleSystem(pComponent, pTexture, maxCount);
		break;
	case PAR_TYPE_FLAME:
		p = new YmxFlameParticleSystem(pComponent, pTexture, maxCount);
		break;
	case PAR_TYPE_EXPLODE:
		p = new YmxExplodeParticleSystem(pComponent, pTexture, maxCount);
		break;
	case PAR_TYPE_COMET:
		p = new YmxCometParticleSystem(pComponent, pTexture, maxCount);
		break;
	default:
		break;
	}
	return p;
}

PARTICLESYSTEM_TYPE YmxParticleSystem::GetType()
{
	return m_Type;
}