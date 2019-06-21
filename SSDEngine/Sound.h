#pragma once
#include"SoundHeader.h"
#include "Resource.h"
#include "DXEnum.h"



class CSound : public CResource
{
private:
	friend class SSDCore;
	friend class CSoundPlayer;

private:
	static FMOD::System*						pFSystem;
	
	static void CreateSoundSystem();
	static void ReleaseSoundSystem();
	static void SoundSystemUpdate();

private:
	FMOD::Sound*								m_pSound;

public:
	FMOD::Sound* GetSound() { return m_pSound; }

public:
	CSound();
	~CSound();


public:
	NONE_CREATE_RESOURCE(CSound)
	LOAD_DESC_BODY(int a;)

public:
	const bool Load(const LOAD_DESC& _Desc);

};

