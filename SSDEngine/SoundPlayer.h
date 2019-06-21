#pragma once
#include "BaseComponent.h"

#include "SoundHeader.h"
#include <string>
#include <vector>
#include <list>


#define BGMCHANNEL			 0
#define MAXGLOBALCHANNEL	 16
#define MAXRESERVECHANNEL	 8
#define MAXSOUNDLOOP		 0x0fffffff
#define NOLOOPSOUND			 -1


#define STARTVOL 0 
#define ENDVOL 1 


enum SOUNDPLAYER_MODE
{
	TRIGGER_MODE,
	FADE_MODE,
	NONE_MODE,
	MAX_SOUNDPLAYER_MODE,
};

typedef class CSoundPlayer : public CBaseComponent
{
private:
	friend class CSound;
	friend class SSDCore;
	friend class CSoundPool;

private:
	static std::vector<CSoundPlayer>						vecGlobalChannel;
	static size_t											ReserveCount;

private:
	static void InitSoundPlayer();
	static FMOD_RESULT F_CALLBACK PlayCallBack(FMOD_CHANNELCONTROL *channelcontrol
		, FMOD_CHANNELCONTROL_TYPE controltype
		, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype
		, void *commanddata1
		, void *commanddata2);

public:
	static const size_t GlobalPlaySound(const std::wstring& _SoundKey, const float& _TriggerTime = 0.f);
	static const size_t GlobalPlayLoopSound(const std::wstring& _SoundKey, const int _LoopCount = -1, const float& _TriggerTime = 0.f);
	static void PlayBGM(const std::wstring& _SoundKey,const float& _WaitTime = 0.f);
	static void PauseBGM(const bool& _Pause);
	static void SetBGMVolume(const float& _Volume);
	static void StopBGM();
	static class CSoundPool* RequestSoundPool(const size_t& _PoolSize);


public:
	static CSoundPlayer* GetBGMPlayer();

private:
	static void GlobalChannelUpdate(const float& _DeltaTime);
	static const size_t CheckGlobalChannel();
public:
	static CSoundPlayer* GetGlobalSoundPlayer(const size_t& _Index);

private:
	bool								m_bGlobalSound;					// 액터에 소속되지 않고 전역으로 만들어진 사운드 플레이어
	bool								m_bReserveSound;				//	글로벌 사운드의 경우 다른 클래스가 글로벌 사운드를 예약가능함(예약된 사운드는 이플레그가 true)
	FMOD::Channel*						m_pChannel;
	
	bool								m_bPlay;
	std::wstring						m_CurSoundKey;
	bool								m_bLoop;
	int									m_LoopCount;
	float								m_fVolume;
	bool								m_bPause;

	SOUNDPLAYER_MODE					m_eMode;
	float								m_fAccTime;
	float								m_fTriggerTime;
	
private:
	float								m_fFadeTime;
	float								m_FadeVol[2];


private:
	void Update(const float& _DeltaTime) override;


private:
	void TriggerUpdate(const float& _DeltaTime);
	void FadeUpdate(const float& _DeltaTime);


private:
	const bool SoundPlay(const std::wstring& _SoundKey, FMOD_MODE _Loop , unsigned int _LoopCount = -1);
public:
	const bool PlaySoundEffect(const std::wstring& _SoundKey, const float _TriggerTime = 0.f);
	const bool PlayLoopSound(const std::wstring& _SoundKey, const int _LoopCount = -1, const float _TriggerTime = 0.f);

public:
	void ChannelRelease();
	void PauseSound(const bool& _Pause);
	void StopSound();

public:
	void SetVolume(const float& _Vol);
	void SetFadeEffect(const float& _FadeTime, const float& _StartVol, const float& _EndVol);
public:
	CSoundPlayer();
	~CSoundPlayer();
}GlobalSoundPlayer;

class CSoundPool :public CGameObject
{
	friend CSoundPlayer;

private:
	std::list<GlobalSoundPlayer*>								m_listSoundPlayer;

public:
	const bool PlaySoundEffect(const std::wstring& _SoundKey, const float _TriggerTime = 0.f);
	const bool PlayLoopSound(const std::wstring& _SoundKey, const int _LoopCount = -1, const float _TriggerTime = 0.f);

public:
	void ReleaseSoundPool();


public:
	CSoundPool();
	~CSoundPool();
};