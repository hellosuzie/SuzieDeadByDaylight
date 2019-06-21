#include "FSMDriver.h"
#include"BaseFSM.h"


CFSMDriver::CFSMDriver()
{
}


CFSMDriver::~CFSMDriver()
{
}

void CFSMDriver::Update(const float& _DeltaTime)
{
	if (nullptr == m_MainFSM )
		return;

	m_MainFSM->PreUpdate(_DeltaTime);
	m_MainFSM->Update(_DeltaTime);
	m_MainFSM->PostUpdate(_DeltaTime);
}