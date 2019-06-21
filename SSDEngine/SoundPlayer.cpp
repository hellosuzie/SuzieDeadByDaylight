
#ifndef _WINSOCK2API_
#include <WinSock2.h>
#endif 

#include "SoundPlayer.h"
#include "ResourceMgr.h"
#include "Sound.h"
#include "DXMath.h"


////////////////////////////////////////////			SoundPlayer				/////////////////////////////////////////

std::vector<CSoundPlayer>					CSoundPlayer::vecGlobalChannel;
size_t										CSoundPlayer::ReserveCount = 0;


const size_t CSoundPlayer::GlobalPlaySound(const std::wstring& _SoundKey, const float& _TriggerTime /*= 0.f*/)
{
	 if (nullptr == CSound::pFSystem)
	 {
		 TASSERT(true);
		 return -1;
	 }

	 SPTR<CSound> FindSound = CResourceMgr<CSound>::Find(_SoundKey);

	 if (nullptr == FindSound)
		 return -1;

	 size_t ChannelIndex = CheckGlobalChannel();

	 if (-1 ==  ChannelIndex )
		 return -1;

	 vecGlobalChannel[ChannelIndex].PlaySoundEffect(_SoundKey, _TriggerTime);
	 return ChannelIndex;
 }
const size_t CSoundPlayer::GlobalPlayLoopSound(const std::wstring& _SoundKey, const int _LoopCount /*= -1*/, const float& _TriggerTime /*= 0.f*/)
{
	if (nullptr == CSound::pFSystem)
	{
		TASSERT(true);
		return -1;
	}

	SPTR<CSound> FindSound = CResourceMgr<CSound>::Find(_SoundKey);

	if (nullptr == FindSound)
		return -1;

	size_t ChannelIndex = CheckGlobalChannel();

	if (-1 == ChannelIndex)
		return -1;

	vecGlobalChannel[ChannelIndex].PlayLoopSound(_SoundKey, _LoopCount,_TriggerTime);
	return ChannelIndex;
}
void CSoundPlayer::PlayBGM(const std::wstring& _SoundKey, const float& _WaitTime /*= 0.f*/) 
{
	vecGlobalChannel[BGMCHANNEL].PlayLoopSound(_SoundKey,MAXSOUNDLOOP, _WaitTime);
}

void CSoundPlayer::PauseBGM(const bool& _Pause)
{
	vecGlobalChannel[BGMCHANNEL].PauseSound(_Pause);
}
void CSoundPlayer::SetBGMVolume(const float& _Volume)
{
	vecGlobalChannel[BGMCHANNEL].SetVolume(_Volume);
}
void CSoundPlayer::StopBGM()
{
	vecGlobalChannel[BGMCHANNEL].StopSound();
}

void CSoundPlayer::GlobalChannelUpdate(const float& _DeltaTime)
{
	for (size_t i = 0; i < vecGlobalChannel.size(); i++)
	{
		if (true == vecGlobalChannel[i].CGameObject::IsActive())
			vecGlobalChannel[i].Update(_DeltaTime);
	}
}
FMOD_RESULT F_CALLBACK CSoundPlayer::PlayCallBack(FMOD_CHANNELCONTROL *channelcontrol
	, FMOD_CHANNELCONTROL_TYPE controltype
	, FMOD_CHANNELCONTROL_CALLBACK_TYPE callbacktype
	, void *commanddata1
	, void *commanddata2)
{
	if (FMOD_CHANNELCONTROL_CALLBACK_END == controltype)
	{
		CSoundPlayer * pSound = nullptr;
		FMOD_Channel_GetUserData((FMOD_CHANNEL*)channelcontrol, (void**)&pSound);

		//	 Call Back 작업 내용
		if (nullptr != pSound) 
			pSound->ChannelRelease();
		
	}


	return FMOD_OK;
}
const size_t CSoundPlayer::CheckGlobalChannel()
{
	for (size_t i = 1; i < vecGlobalChannel.size(); i++)
	{
		if (true == vecGlobalChannel[i].m_bReserveSound || true == vecGlobalChannel[i].m_bPlay)
			continue;

		if (nullptr == vecGlobalChannel[i].m_pChannel)
			return i;
	}

	return -1;
}
void CSoundPlayer::InitSoundPlayer()
{
	vecGlobalChannel.reserve(MAXGLOBALCHANNEL);

	for (size_t i = 0; i < MAXGLOBALCHANNEL; i++)
	{
		vecGlobalChannel.push_back( GlobalSoundPlayer());
		vecGlobalChannel[i].m_bGlobalSound = true;
	}
	
	vecGlobalChannel[BGMCHANNEL].m_fVolume = 0.7f;
}
CSoundPlayer* CSoundPlayer::GetGlobalSoundPlayer(const size_t& _Index)
{
	if (_Index >= vecGlobalChannel.size())
		return nullptr;

	return &vecGlobalChannel[_Index];
}

CSoundPool* CSoundPlayer::RequestSoundPool(const size_t& _PoolSize)
{
	if (_PoolSize <= 0)
		return nullptr;

	if (ReserveCount + _PoolSize >= MAXRESERVECHANNEL)
		return nullptr;

	CSoundPool* NewPool = Engine::CreateGameObject<CSoundPool>();
	
	for (size_t i = MAXGLOBALCHANNEL - 1  ; i >= 1 ; --i)
	{
		if (false == vecGlobalChannel[i].m_bReserveSound) 
		{
			vecGlobalChannel[i].m_bReserveSound = true;
			NewPool->m_listSoundPlayer.push_back(&vecGlobalChannel[i]);

			if (NewPool->m_listSoundPlayer.size() >= _PoolSize)
				break;
		}
	}
	ReserveCount += _PoolSize;
	return NewPool;
}

CSoundPlayer* CSoundPlayer::GetBGMPlayer()
{
	return &vecGlobalChannel[BGMCHANNEL];
}
CSoundPlayer::CSoundPlayer()
	: m_pChannel(nullptr), m_fVolume(1.f), m_fTriggerTime(0.f), m_fAccTime(0.f)
	, m_CurSoundKey(L""), m_bPause(false), m_eMode(NONE_MODE), m_LoopCount(0), m_bLoop(false)
	, m_bGlobalSound(false), m_bReserveSound(false), m_bPlay(false)
	, m_fFadeTime(0.f)
{
	memset(m_FadeVol, 0, sizeof(m_FadeVol));
	Off();
}

CSoundPlayer::~CSoundPlayer()
{
	if (nullptr == m_pChannel)
		return;

	bool Check;

	m_pChannel->isPlaying(&Check);

	if (Check) 
		m_pChannel->stop();
	
	ChannelRelease();
	
	
}
void CSoundPlayer::Update(const float& _DeltaTime)
{
	switch (m_eMode)
	{
	case TRIGGER_MODE:
		TriggerUpdate(_DeltaTime);
		break; 
	case FADE_MODE:
		FadeUpdate(_DeltaTime);
		break;
	default:
		return;
	}

	m_fAccTime += _DeltaTime;


}

void CSoundPlayer::TriggerUpdate(const float& _DeltaTime) 
{
	if (m_fAccTime >= m_fTriggerTime) 
	{
		bool Check;
		if (true == m_bLoop)
			Check = SoundPlay(m_CurSoundKey, FMOD_LOOP_NORMAL, m_LoopCount);
		else
			Check = SoundPlay(m_CurSoundKey, FMOD_LOOP_OFF, NOLOOPSOUND);
		
		if (false == Check)
			ChannelRelease();

		m_fAccTime = 0.f;
		Off();
	}
	
}
void CSoundPlayer::FadeUpdate(const float& _DeltaTime)
{
	if (m_fAccTime >= m_fFadeTime)
	{
		m_eMode = NONE_MODE;
		Off();
		return;
	}

	m_fAccTime += _DeltaTime;
	SetVolume(DXMath::FloatLerp(m_FadeVol[STARTVOL], m_FadeVol[ENDVOL], m_fAccTime / m_fFadeTime));
	
}
const bool CSoundPlayer::SoundPlay(const std::wstring& _SoundKey, FMOD_MODE _Loop , unsigned int _LoopCount /*= -1*/)
{
	if (nullptr == CSound::pFSystem)
	{
		TASSERT(true);
		return false;
	}

	SPTR<CSound> FindSound = CResourceMgr<CSound>::Find(_SoundKey);

	if (nullptr == FindSound)
		return false;


	if (nullptr != m_pChannel) 
	{
		bool Check = false;
		m_pChannel->isPlaying(&Check);

		if (true == Check)
			StopSound();
	}


	FMOD_RESULT hr = CSound::pFSystem->playSound(FindSound->GetSound(), nullptr, false, &m_pChannel);

	if (FMOD_OK != hr)
		return false;

	m_pChannel->setMode(_Loop);
	m_pChannel->setVolume(m_fVolume);

	if (FMOD_LOOP_OFF != _Loop)
		m_pChannel->setLoopCount(_LoopCount);

	m_pChannel->setCallback(PlayCallBack);
	m_pChannel->setUserData(this);
	m_bPlay = true;
	
	return true;
}

const bool CSoundPlayer::PlaySoundEffect(const std::wstring& _SoundKey, const float _TriggerTime /*= 0.f*/)
{
	m_CurSoundKey = _SoundKey;
	m_LoopCount = 0;
	m_bLoop = false;

	if (0.f == _TriggerTime)
	{
		Off();
		return SoundPlay(m_CurSoundKey, FMOD_LOOP_OFF, NOLOOPSOUND);
	}
	else 
	{
		m_eMode = TRIGGER_MODE;
		m_fTriggerTime = _TriggerTime;
		m_bPlay = true;
		On();
	}

	return true;
}

const bool CSoundPlayer::PlayLoopSound(const std::wstring& _SoundKey, const int _LoopCount /*= -1 */ ,const float _TriggerTime /*= 0.f*/)
{
	m_CurSoundKey = _SoundKey;
	m_LoopCount = _LoopCount;
	m_bLoop = true;

	if (0.f == _TriggerTime)
	{
		Off();
		return SoundPlay(m_CurSoundKey, FMOD_LOOP_NORMAL, _LoopCount);
	}
	else
	{
		m_eMode = TRIGGER_MODE;
		m_fTriggerTime = _TriggerTime;
		m_bPlay = true;
		On();
	}

	return true;
}

void CSoundPlayer::ChannelRelease()
{
	if (nullptr != m_pChannel) 
	{
		m_pChannel = nullptr;
		m_CurSoundKey = L"";
		m_bPlay = false;
		m_fVolume = 1.f;
	}
}

void CSoundPlayer::PauseSound(const bool& _Pause)
{
	if (nullptr == m_pChannel)
		return;

	m_bPause = _Pause;
	m_pChannel->setPaused(m_bPause);
}
void CSoundPlayer::StopSound()
{
	if (nullptr == m_pChannel)
		return;

	m_pChannel->stop();
}

void CSoundPlayer::SetVolume(const float& _Vol)
{
	if (nullptr == m_pChannel)
		return;

	m_fVolume = _Vol;
	m_pChannel->setVolume(m_fVolume);
}

void CSoundPlayer::SetFadeEffect(const float& _FadeTime, const float& _StartVol, const float& _EndVol)
{
	if (_FadeTime <= 0.f)
		return;

	m_eMode = FADE_MODE;
	m_fFadeTime =  _FadeTime;
	m_fAccTime = 0.f;
	m_FadeVol[STARTVOL] = _StartVol;
	m_FadeVol[ENDVOL] = _EndVol;

	if (nullptr != m_pChannel)
		m_pChannel->setVolume(m_FadeVol[STARTVOL]);

	On();
}


//////////////////////////////////////////				Sound Pool				//////////////////////////////////

CSoundPool::CSoundPool() 
{
	m_listSoundPlayer.clear();
}
CSoundPool::~CSoundPool()
{
	for (CSoundPlayer* Player : m_listSoundPlayer)
		Player->m_bReserveSound = false;
	
	m_listSoundPlayer.clear();
}

const bool CSoundPool::PlaySoundEffect(const std::wstring& _SoundKey, const float _TriggerTime /*= 0.f*/) 
{
	for (CSoundPlayer* Player :m_listSoundPlayer)
	{
		if (false == Player->m_bPlay) 
			return Player->PlaySoundEffect(_SoundKey, _TriggerTime);
	}

	return false;
}
const bool CSoundPool::PlayLoopSound(const std::wstring& _SoundKey, const int _LoopCount /*= -1*/, const float _TriggerTime /*= 0.f*/) 
{
	for (CSoundPlayer* Player : m_listSoundPlayer)
	{
		if (false == Player->m_bPlay)
			return Player->PlayLoopSound(_SoundKey, _LoopCount,_TriggerTime);
	}

	return false;
}

void CSoundPool::ReleaseSoundPool()
{
	for (GlobalSoundPlayer* Sound : m_listSoundPlayer)
	{
		if (nullptr == Sound)
			continue;
		
		Sound->m_bReserveSound = false;
	}

	CSoundPlayer::ReserveCount = max(0, CSoundPlayer::ReserveCount - m_listSoundPlayer.size());
	m_listSoundPlayer.clear();
}