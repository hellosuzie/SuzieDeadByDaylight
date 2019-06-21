#pragma once
#include "TimerMgr.h"

class CProfiler
{
public:
	static CTimerMgr::CPulse		ProfilerPulse;

	static bool						ProfilerMode;
	static SPTR<class CDXFont>		m_DebugFont;

public:
	static LARGE_INTEGER			UpdateStartTimer;
	static LARGE_INTEGER			UpdateEndTimer;
	static LARGE_INTEGER			UpdateTimer;

	static LARGE_INTEGER			CollisionStartTimer;
	static LARGE_INTEGER			CollisionEndTimer;
	static LARGE_INTEGER			CollisionTimer;

	static LARGE_INTEGER			RenderStartTimer;
	static LARGE_INTEGER			RenderEndTimer;
	static LARGE_INTEGER			RenderTimer;

	static LARGE_INTEGER			PresentStartTimer;
	static LARGE_INTEGER			PresentEndTimer;
	static LARGE_INTEGER			PresentTimer;

	static LARGE_INTEGER			LastUpdateStartTimer;
	static LARGE_INTEGER			LastUpdateEndTimer;
	static LARGE_INTEGER			LastUpdateTimer;

	static LARGE_INTEGER			TotalStartTimer;
	static LARGE_INTEGER			TotalEndTimer;
	static LARGE_INTEGER			TotalTimer;

	static float					UpdateRatio;
	static float					CollisionRatio;
	static float					RenderRatio;
	static float					PresentRatio;
	static float					LastUpdateRatio;
	static float					Return;

public:
	static void Init();

	static void UpdateTimerStart();
	static void CollisionTimerStart();
	static void RenderTimerStart();
	static void PresentTimerStart();
	static void LastUpdateTimerStart();
	static void TotalTimerStart();

	static void UpdateTimerEnd();
	static void CollisionTimerEnd();
	static void RenderTimerEnd();
	static void PresentTimerEnd();
	static void LastUpdateTimerEnd();
	static LARGE_INTEGER TotalTimerEnd();
	
	static LARGE_INTEGER GetProfilingTime(LARGE_INTEGER _Start, LARGE_INTEGER _End);
	//static float GetProfilingRatio(LARGE_INTEGER _Total, LARGE_INTEGER _Value, bool _LastCheck = false);
	static float UpdateProfilingRatio(LARGE_INTEGER _Total);
	static float CollisionProfilingRatio(LARGE_INTEGER _Total);
	static float RenderProfilingRatio(LARGE_INTEGER _Total);
	static float PresentProfilingRatio(LARGE_INTEGER _Total);
	static float LastUpdateProfilingRatio(LARGE_INTEGER _Total);

	static void ResetProfileValue();
	static void DebugRender();

public:
	static void ProfilerOn()
	{
		ProfilerMode = true;

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

	static void ProfilerOff()
	{
		ProfilerMode = false;

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

public:
	CProfiler();
	~CProfiler();
};

//객체 하나만
//리셋 
