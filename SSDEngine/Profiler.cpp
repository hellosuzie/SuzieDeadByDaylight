#include "Profiler.h"
#include "DXMacro.h"
#include "DXFont.h"
#include "ResourceMgr.h"

LARGE_INTEGER			CProfiler::UpdateStartTimer = LARGE_INTEGER();
LARGE_INTEGER			CProfiler::UpdateEndTimer = LARGE_INTEGER();
LARGE_INTEGER			CProfiler::UpdateTimer = LARGE_INTEGER();

LARGE_INTEGER			CProfiler::CollisionStartTimer = LARGE_INTEGER();
LARGE_INTEGER			CProfiler::CollisionEndTimer = LARGE_INTEGER();
LARGE_INTEGER			CProfiler::CollisionTimer = LARGE_INTEGER();

LARGE_INTEGER			CProfiler::RenderStartTimer = LARGE_INTEGER();
LARGE_INTEGER			CProfiler::RenderEndTimer = LARGE_INTEGER();
LARGE_INTEGER			CProfiler::RenderTimer = LARGE_INTEGER();

LARGE_INTEGER			CProfiler::PresentStartTimer = LARGE_INTEGER();
LARGE_INTEGER			CProfiler::PresentEndTimer = LARGE_INTEGER();
LARGE_INTEGER			CProfiler::PresentTimer = LARGE_INTEGER();

LARGE_INTEGER			CProfiler::LastUpdateStartTimer = LARGE_INTEGER();
LARGE_INTEGER			CProfiler::LastUpdateEndTimer = LARGE_INTEGER();
LARGE_INTEGER			CProfiler::LastUpdateTimer = LARGE_INTEGER();

LARGE_INTEGER			CProfiler::TotalStartTimer = LARGE_INTEGER();
LARGE_INTEGER			CProfiler::TotalEndTimer = LARGE_INTEGER();
LARGE_INTEGER			CProfiler::TotalTimer = LARGE_INTEGER();

float					CProfiler::UpdateRatio = 0.f;
float					CProfiler::CollisionRatio = 0.f;
float					CProfiler::RenderRatio = 0.f;
float					CProfiler::PresentRatio = 0.f;
float					CProfiler::LastUpdateRatio = 0.f;
float					CProfiler::Return = 0.f;

bool					CProfiler::ProfilerMode = true;
SPTR<CDXFont>			CProfiler::m_DebugFont = nullptr;
CTimerMgr::CPulse		CProfiler::ProfilerPulse(1.f);

CProfiler::CProfiler()
{
}

CProfiler::~CProfiler()
{
}

void CProfiler::Init()
{
	m_DebugFont = CResourceMgr<CDXFont>::Find(L"³ª´®¹Ù¸¥Ææ");
}

void CProfiler::UpdateTimerStart()
{
	if (false == ProfilerMode)
	{
		return;
	}
	QueryPerformanceCounter(&CProfiler::UpdateStartTimer);
}

void CProfiler::CollisionTimerStart()
{
	if (false == ProfilerMode)
	{
		return;
	}
	QueryPerformanceCounter(&CProfiler::CollisionStartTimer);
}

void CProfiler::RenderTimerStart()
{
	if (false == ProfilerMode)
	{
		return;
	}
	QueryPerformanceCounter(&CProfiler::RenderStartTimer);
}

void CProfiler::PresentTimerStart()
{
	if (false == ProfilerMode)
	{
		return;
	}
	QueryPerformanceCounter(&CProfiler::PresentStartTimer);
}

void CProfiler::LastUpdateTimerStart()
{
	if (false == ProfilerMode)
	{
		return;
	}
	QueryPerformanceCounter(&CProfiler::LastUpdateStartTimer);
}

void CProfiler::TotalTimerStart()
{
	if (false == ProfilerMode)
	{
		return;
	}
	QueryPerformanceCounter(&CProfiler::TotalStartTimer);
}

void CProfiler::UpdateTimerEnd()
{
	if (false == ProfilerMode)
	{
		return;
	}
	QueryPerformanceCounter(&CProfiler::UpdateEndTimer);
	CProfiler::UpdateTimer.QuadPart += GetProfilingTime(CProfiler::UpdateStartTimer, CProfiler::UpdateEndTimer).QuadPart;
}

void CProfiler::CollisionTimerEnd()
{
	if (false == ProfilerMode)
	{
		return;
	}
	QueryPerformanceCounter(&CProfiler::CollisionEndTimer);
	CProfiler::CollisionTimer.QuadPart += GetProfilingTime(CProfiler::CollisionStartTimer, CProfiler::CollisionEndTimer).QuadPart;
}

void CProfiler::RenderTimerEnd()
{
	if (false == ProfilerMode)
	{
		return;
	}
	QueryPerformanceCounter(&CProfiler::RenderEndTimer);
	CProfiler::RenderTimer.QuadPart += GetProfilingTime(CProfiler::RenderStartTimer, CProfiler::RenderEndTimer).QuadPart;
}

void CProfiler::PresentTimerEnd()
{
	if (false == ProfilerMode)
	{
		return;
	}
	QueryPerformanceCounter(&CProfiler::PresentEndTimer);
	CProfiler::PresentTimer.QuadPart += GetProfilingTime(CProfiler::PresentStartTimer, CProfiler::PresentEndTimer).QuadPart;
}

void CProfiler::LastUpdateTimerEnd()
{
	if (false == ProfilerMode)
	{
		return;
	}
	QueryPerformanceCounter(&CProfiler::LastUpdateEndTimer);
	CProfiler::LastUpdateTimer.QuadPart += GetProfilingTime(CProfiler::LastUpdateStartTimer, CProfiler::LastUpdateEndTimer).QuadPart;
}

LARGE_INTEGER CProfiler::TotalTimerEnd()
{
	if (false == ProfilerMode)
	{
		return LARGE_INTEGER();
	}
	QueryPerformanceCounter(&CProfiler::TotalEndTimer);
	CProfiler::TotalTimer.QuadPart += GetProfilingTime(CProfiler::TotalStartTimer, CProfiler::TotalEndTimer).QuadPart;
	return CProfiler::TotalTimer;
}

LARGE_INTEGER CProfiler::GetProfilingTime(LARGE_INTEGER _Start, LARGE_INTEGER _End)
{
	if (false == ProfilerMode)
	{
		return LARGE_INTEGER();
	}
	LARGE_INTEGER Return;
	Return.QuadPart = _End.QuadPart - _Start.QuadPart;
	return Return;
}

//
//float CProfiler::GetProfilingRatio(LARGE_INTEGER _Total, LARGE_INTEGER _Value, bool _LastCheck/* = false*/)
//{
//	if (false == ProfilerMode)
//	{
//		return 0.f;
//	}
//
//	if (true == ProfilerPulse.CheckPulse())
//	{
//		CProfiler::Return = 100.f * _Value.QuadPart / _Total.QuadPart;
//	}
//	if (true == _LastCheck)
//	{
//		ResetProfileValue();
//	}
//	return CProfiler::Return;
//}

float CProfiler::UpdateProfilingRatio(LARGE_INTEGER _Total)
{
	if (false == ProfilerMode)
	{
		return 0.f;
	}
	if (true == ProfilerPulse.CheckPulse())
	{
		CProfiler::UpdateRatio = 100.f * CProfiler::UpdateTimer.QuadPart / _Total.QuadPart;
	}
	return CProfiler::UpdateRatio;
}

float CProfiler::CollisionProfilingRatio(LARGE_INTEGER _Total)
{
	if (false == ProfilerMode)
	{
		return 0.f;
	}
	if (true == ProfilerPulse.CheckPulse())
	{
		CProfiler::CollisionRatio = 100.f * CProfiler::CollisionTimer.QuadPart / _Total.QuadPart;
	}
	return CProfiler::CollisionRatio;
}

float CProfiler::RenderProfilingRatio(LARGE_INTEGER _Total)
{
	if (false == ProfilerMode)
	{
		return 0.f;
	}
	if (true == ProfilerPulse.CheckPulse())
	{
		CProfiler::RenderRatio = 100.f * CProfiler::RenderTimer.QuadPart / _Total.QuadPart;
	}
	return CProfiler::RenderRatio;
}

float CProfiler::PresentProfilingRatio(LARGE_INTEGER _Total)
{
	if (false == ProfilerMode)
	{
		return 0.f;
	}
	if (true == ProfilerPulse.CheckPulse())
	{
		CProfiler::PresentRatio = 100.f * CProfiler::PresentTimer.QuadPart / _Total.QuadPart;
	}
	return CProfiler::PresentRatio;
}

float CProfiler::LastUpdateProfilingRatio(LARGE_INTEGER _Total)
{
	if (false == ProfilerMode)
	{
		return 0.f;
	}
	if (true == ProfilerPulse.CheckPulse())
	{
		CProfiler::LastUpdateRatio = 100.f * CProfiler::LastUpdateTimer.QuadPart / _Total.QuadPart;
	}
	return CProfiler::LastUpdateRatio;
}

void CProfiler::ResetProfileValue()
{
	UpdateStartTimer.QuadPart = (LONGLONG)0.f;
	UpdateEndTimer.QuadPart = (LONGLONG)0.f;
	UpdateTimer.QuadPart = (LONGLONG)0.f;

	CollisionStartTimer.QuadPart = (LONGLONG)0.f;
	CollisionEndTimer.QuadPart = (LONGLONG)0.f;
	CollisionTimer.QuadPart = (LONGLONG)0.f;

	RenderStartTimer.QuadPart = (LONGLONG)0.f;
	RenderEndTimer.QuadPart = (LONGLONG)0.f;
	RenderTimer.QuadPart = (LONGLONG)0.f;

	PresentStartTimer.QuadPart = (LONGLONG)0.f;
	PresentEndTimer.QuadPart = (LONGLONG)0.f;
	PresentTimer.QuadPart = (LONGLONG)0.f;

	LastUpdateStartTimer.QuadPart = (LONGLONG)0.f;
	LastUpdateEndTimer.QuadPart = (LONGLONG)0.f;
	LastUpdateTimer.QuadPart = (LONGLONG)0.f;

	TotalStartTimer.QuadPart = (LONGLONG)0.f;
	TotalEndTimer.QuadPart = (LONGLONG)0.f;
	TotalTimer.QuadPart = (LONGLONG)0.f;
}

void CProfiler::DebugRender()
{
	if (false == ProfilerMode)
	{
		return;
	}

	ProfilerPulse.Update(CTimerMgr::DeltaTime());

	wchar_t	DebugBuff[MAXSTRING];
	memset(DebugBuff, 0, sizeof(wchar_t)*MAXSTRING);

	swprintf_s(DebugBuff, L"UpdateRatio %f", CProfiler::UpdateRatio);
	m_DebugFont->DrawString(DebugBuff, 20.f, Vec2(700.f, 0.f), Vec4::Pink);
	swprintf_s(DebugBuff, L"CollisionRatio %f", CProfiler::CollisionRatio);
	m_DebugFont->DrawString(DebugBuff, 20.f, Vec2(700.f, 20.f), Vec4::Pink);
	swprintf_s(DebugBuff, L"RenderRatio %f", CProfiler::RenderRatio);
	m_DebugFont->DrawString(DebugBuff, 20.f, Vec2(700.f, 40.f), Vec4::Pink);
	swprintf_s(DebugBuff, L"PresentRatio %f", CProfiler::PresentRatio);
	m_DebugFont->DrawString(DebugBuff, 20.f, Vec2(700.f, 60.f), Vec4::Pink);
	swprintf_s(DebugBuff, L"LastUpdateRatio %f", CProfiler::LastUpdateRatio);
	m_DebugFont->DrawString(DebugBuff, 20.f, Vec2(700.f, 80.f), Vec4::Pink);
}
