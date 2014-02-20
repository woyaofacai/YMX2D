#include "stdafx.h"
#include "ymxsound.h"


LPDIRECTSOUND ppds = NULL;
//WAVEFORMATEX pcmwf; 
DSBUFFERDESC dsbd;

bool Initialize_DSound()
{
	if(FAILED(DirectSoundCreate(NULL, &ppds, NULL)))
		return false;

	if(FAILED(ppds->SetCooperativeLevel(hwnd, DSSCL_NORMAL)))
		return false;

	return true;
}

bool Release_DSound()
{
	if(ppds != NULL)
	{
		ppds->Release();
		ppds = NULL;
	}

	return true;
}


YmxWaveSound* CreateWaveSoundFromFile(PTSTR filename)
{
	HMMIO 			hwav;    
	MMCKINFO		parent,  child;   
	WAVEFORMATEX    wfmtx;

	UCHAR *snd_buffer, *audio_ptr_1=NULL, *audio_ptr_2=NULL;

	DWORD audio_length_1=0, audio_length_2=0; 

	YmxWaveSound* lpSound = new YmxWaveSound;

	parent.ckid 	    = (FOURCC)0;
	parent.cksize 	    = 0;
	parent.fccType	    = (FOURCC)0;
	parent.dwDataOffset = 0;
	parent.dwFlags		= 0;

	child = parent;

	if ((hwav = mmioOpen(filename, NULL, MMIO_READ | MMIO_ALLOCBUF))==NULL)
		return NULL;

	
	parent.fccType = mmioFOURCC('W', 'A', 'V', 'E');

	if (mmioDescend(hwav, &parent, NULL, MMIO_FINDRIFF))
	{
		mmioClose(hwav, 0);
		return NULL; 	
	} 

	child.ckid = mmioFOURCC('f', 'm', 't', ' ');

	if (mmioDescend(hwav, &child, &parent, 0))
	{
		mmioClose(hwav, 0);
		return NULL; 
	}

	if (mmioRead(hwav, (char *)&wfmtx, sizeof(wfmtx)) != sizeof(wfmtx))
	{
		mmioClose(hwav, 0);
		return NULL;
	} 

	if (wfmtx.wFormatTag != WAVE_FORMAT_PCM)
	{
		mmioClose(hwav, 0);
		return NULL;
	}

	if (mmioAscend(hwav, &child, 0))
	{
		mmioClose(hwav, 0);
		return NULL;
	}

	child.ckid = mmioFOURCC('d', 'a', 't', 'a');

	if (mmioDescend(hwav, &child, &parent, MMIO_FINDCHUNK))
	{
		mmioClose(hwav, 0);
		return NULL;
	} 
	snd_buffer = (UCHAR *)malloc(child.cksize);
	mmioRead(hwav, (char *)snd_buffer, child.cksize);
	mmioClose(hwav, 0);

	lpSound->m_rate = wfmtx.nSamplesPerSec;
	lpSound->m_size = child.cksize;

	
	dsbd.dwSize			= sizeof(DSBUFFERDESC);
	dsbd.dwFlags		= DSBCAPS_CTRLFREQUENCY | DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_STATIC | DSBCAPS_LOCSOFTWARE;
	dsbd.dwBufferBytes	= child.cksize;
	dsbd.lpwfxFormat	= &wfmtx;

	
	if (FAILED(ppds->CreateSoundBuffer(&dsbd,(LPDIRECTSOUNDBUFFER*)(&lpSound->m_dsbuffer),NULL)))
	{
		free(snd_buffer);
		return NULL;
	}

	
	if (FAILED(((LPDIRECTSOUNDBUFFER)(lpSound->m_dsbuffer))->Lock(0,					 
		child.cksize,			
		(void **) &audio_ptr_1, 
		&audio_length_1,
		(void **)&audio_ptr_2, 
		&audio_length_2,
		DSBLOCK_FROMWRITECURSOR)))
		return(0);

	
	memcpy(audio_ptr_1, snd_buffer, audio_length_1);
	memcpy(audio_ptr_2, (snd_buffer+audio_length_1),audio_length_2);
	
	if (FAILED(((LPDIRECTSOUNDBUFFER)(lpSound->m_dsbuffer))->Unlock(audio_ptr_1, 
		audio_length_1, 
		audio_ptr_2, 
		audio_length_2)))
		return(0);

	
	free(snd_buffer);
	return lpSound;
}
