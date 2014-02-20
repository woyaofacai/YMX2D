#include "stdafx.h"
#include "ymxsound.h"

YmxWaveSound::YmxWaveSound()
{
	m_dsbuffer = NULL;
	m_curPosition = 0;
}

YmxWaveSound::~YmxWaveSound()
{

}

void YmxWaveSound::Release()
{
	if(m_dsbuffer != NULL)
	{
		((LPDIRECTSOUNDBUFFER)m_dsbuffer)->Release();
		m_dsbuffer = NULL;
	}
	delete this;
}

bool YmxWaveSound::Play(bool bLooping)
{
	if(m_dsbuffer != NULL)
	{
		int flags = (bLooping) ? DSBPLAY_LOOPING : 0;
		if(FAILED(((LPDIRECTSOUNDBUFFER)m_dsbuffer)->SetCurrentPosition(m_curPosition)))
			return false;

		if(FAILED(((LPDIRECTSOUNDBUFFER)m_dsbuffer)->Play(0, 0, flags)))
			return false;
	}
	return true;
}

bool YmxWaveSound::Stop()
{
	if(m_dsbuffer)
	{
		if(FAILED(((LPDIRECTSOUNDBUFFER)m_dsbuffer)->Stop()))
			return false;

		m_curPosition = 0;
	}
	return true;
}

bool YmxWaveSound::Pause()
{
	if(m_dsbuffer)
	{
		if(FAILED(((LPDIRECTSOUNDBUFFER)m_dsbuffer)->GetCurrentPosition(&m_curPosition, NULL)))
			return false;
		if(FAILED(((LPDIRECTSOUNDBUFFER)m_dsbuffer)->Stop()))
			return false;
	}
	return true;
}

bool YmxWaveSound::SetVolume(int volumne)
{
	if(m_dsbuffer)
	{
		volumne = max(0, min(volumne, 100));

		if(FAILED(((LPDIRECTSOUNDBUFFER)m_dsbuffer)->SetVolume(DSVOLUME_TO_DB(volumne))))
			return false;
	}
	return true;
}

bool YmxWaveSound::SetFrequency(int freq)
{
	if(m_dsbuffer)
	{
		freq = max(DSBFREQUENCY_MIN, min(DSBFREQUENCY_MAX, freq));

		if(FAILED(((LPDIRECTSOUNDBUFFER)m_dsbuffer)->SetFrequency(freq)))
			return false;
	}
	return true;
}

bool YmxWaveSound::SetPan(int pan)
{
	if(m_dsbuffer)
	{
		pan = max(DSBPAN_LEFT, min(DSBPAN_RIGHT, pan));

		if(FAILED(((LPDIRECTSOUNDBUFFER)m_dsbuffer)->SetPan(pan)))
			return false;
	}

	return true;
}

bool YmxWaveSound::GetVolume(int* volume)
{
	if(m_dsbuffer)
	{
		long db;
		if(FAILED(((LPDIRECTSOUNDBUFFER)m_dsbuffer)->GetVolume(&db)))
			return false;

		*volume = DB_TO_DSVOLUME(db);
		return true;
	}
	return false;
}

bool YmxWaveSound::GetPan(int* pan)
{
	if(m_dsbuffer)
	{
		long p;
		if(FAILED(((LPDIRECTSOUNDBUFFER)m_dsbuffer)->GetPan(&p)))
			return false;

		*pan = p;
		return true;
	}
	return false;
}

bool YmxWaveSound::GetFrequency(int* freq)
{
	if(m_dsbuffer)
	{
		DWORD f;
		if(FAILED(((LPDIRECTSOUNDBUFFER)m_dsbuffer)->GetFrequency(&f)))
			return false;

		*freq = f;
		return true;
	}
	return false;
}

bool YmxWaveSound::IsPlaying()
{
	if(m_dsbuffer)
	{
		DWORD status;
		if(FAILED(((LPDIRECTSOUNDBUFFER)m_dsbuffer)->GetStatus(&status)))
			return false;

		return status & DSBSTATUS_PLAYING;
	}
	return false;
}

bool YmxWaveSound::IsLooping()
{
	if(m_dsbuffer)
	{
		DWORD status;
		if(FAILED(((LPDIRECTSOUNDBUFFER)m_dsbuffer)->GetStatus(&status)))
			return false;

		return status & DSBSTATUS_LOOPING;
	}
	return false;
}

