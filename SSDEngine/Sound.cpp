#include "Sound.h"
#include "DXMacro.h"
#include <string>
#include <atlstr.h>
#include "SoundPlayer.h"



FMOD::System*	CSound::pFSystem = nullptr;

void CSound::CreateSoundSystem()
{
	FMOD::System_Create(&pFSystem);

	if (nullptr == pFSystem)
	{
		TASSERT(true);
		return;
	}
	pFSystem->init(32, FMOD_DEFAULT, nullptr);
	CSoundPlayer::InitSoundPlayer();
}
void CSound::ReleaseSoundSystem()
{
	pFSystem->release();
	pFSystem->close();
}
void CSound::SoundSystemUpdate()
{
	pFSystem->update();
}

CSound::CSound()
	:m_pSound(nullptr)
{
}


CSound::~CSound()
{
	if (nullptr != m_pSound)
		m_pSound->release();
}


const bool CSound::Load(const LOAD_DESC& _Desc)
{
	if (nullptr == pFSystem)
		return false;

	FMOD_RESULT hr = FMOD_OK;

	wchar_t Buff[MAXSTRING];
	char Path[MAXSTRING];
	StrCpyW(Buff, GetPath().c_str());
	int len = WideCharToMultiByte(CP_ACP, 0, &Buff[0], -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, &Buff[0], -1, &Path[0], len, NULL, NULL);


	hr = pFSystem->createSound(Path, FMOD_DEFAULT, nullptr, &m_pSound);


//#ifdef _UNICODE
//	std::string Path = CW2A(GetPath().c_str(), CP_UTF8);
//	hr = pFSystem->createSound(Path.c_str(), FMOD_DEFAULT, nullptr, &m_pSound);
//#else
//	std::string Path = CW2A(GetPath().c_str(), CP_UTF8);
//	hr = pFSystem->createSound(Path.c_str(), FMOD_DEFAULT, nullptr, &m_pSound);
//#endif

	if (FMOD_OK != hr) 
		return false;
	
	return true;
}