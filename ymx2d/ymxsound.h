#include "ymx2d.h"

#ifndef YMX2D_FUNCTION_CLASS_FORM_SOUND_H
#define YMX2D_FUNCTION_CLASS_FORM_SOUND_H

#define DSVOLUME_TO_DB(volume) ((DWORD)(-30 * (100 - volume)))
#define DB_TO_DSVOLUME(db) ((DWORD)(100 + db / 30))

class YMX2D_API YmxSound;
class YMX2D_API YmxWaveSound;
//class YMX2D_API YmxMidiMusic;

//extern IDirectMusicPerformance8 *dm_perf;

bool Initialize_DSound();
bool Initialize_DMusic();
bool Release_DSound();
bool Release_DMusic();

YmxWaveSound* CreateWaveSoundFromFile(PTSTR filename);
//YmxMidiMusic* CreateMidiMusicFromFile(PTSTR filename);

class YMX2D_API YmxSound
{
public:
	virtual void Release() = 0;
	virtual bool Play(bool bLooping = false) = 0;
	virtual bool Stop() = 0;
	virtual bool Pause() = 0;
	virtual bool SetFrequency(int freq) = 0;
	virtual bool SetPan(int pan) = 0;
	virtual bool SetVolume(int volume) = 0;
	virtual bool GetFrequency(int* freq) = 0;
	virtual bool GetPan(int* pan) = 0;
	virtual bool GetVolume(int* volume) = 0;
	virtual bool IsPlaying() = 0;
	virtual bool IsLooping() = 0;
};

class YMX2D_API YmxWaveSound : public YmxSound
{
	friend YmxWaveSound* CreateWaveSoundFromFile(PTSTR filename);
public:
	void Release();
	bool Play(bool bLooping = false);
	bool Stop();
	bool Pause();
	bool SetFrequency(int freq);
	bool SetPan(int pan);
	bool SetVolume(int volume);
	bool GetFrequency(int* freq);
	bool GetPan(int* pan);
	bool GetVolume(int* volume);
	bool IsPlaying();
	bool IsLooping();
	~YmxWaveSound();
	//static YmxSound* LoadFromWaveFile(PTSTR filename, int control_flags);
private:
	YmxWaveSound();
	//LPDIRECTSOUNDBUFFER m_dsbuffer;
	PVOID m_dsbuffer;
	int m_rate;                       // playback rate
	int m_size;                       // size of sound
	DWORD m_curPosition;
};

/*
class YMX2D_API YmxMidiMusic : public YmxSound
{
	friend YmxMidiMusic* CreateMidiMusicFromFile(PTSTR filename);
public:
	void Release();
	bool Play();
	bool Stop();
	bool SetFrequency(int freq);
	bool SetPan(int pan);
	bool SetVolume(int volume);
	void SetLooping(bool looping);
	~YmxMidiMusic();
	//static YmxSound* LoadFromWaveFile(PTSTR filename, int control_flags);
private:
	YmxMidiMusic();
	IDirectMusicSegment8 *m_dmSegment;  // the directmusic segment
	IDirectMusicSegmentState *m_dmSegState; // the state of the segment
	IDirectMusicAudioPath* m_dmAudioPath;
	int m_state;
};
*/

#endif