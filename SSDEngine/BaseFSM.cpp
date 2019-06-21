#include "BaseFSM.h"



CBaseFSM::CBaseFSM()
	:m_CurFunction(nullptr)
{

}


CBaseFSM::~CBaseFSM()
{
}

void CBaseFSM::Update(const float& _DeltaTime)
{
	if (nullptr == m_CurFunction)
		return;

	m_CurFunction(_DeltaTime);
}

