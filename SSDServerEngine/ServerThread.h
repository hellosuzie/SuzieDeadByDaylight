#pragma once
#include "ServerObject.h"
#include <WinSock2.h>
#include <MSWSock.h>
#include <unordered_map>
#include <process.h>
#include <functional>
#include <list>
#include <thread>
#include <assert.h>
#include "ServerMacro.h"

#pragma comment (lib, "WS2_32.lib")
#pragma comment(lib, "mswsock.lib")


class CServerCS
{
private:
	CRITICAL_SECTION m_CRI;

public:
	void Enter();
	void Leave();

public:
	CServerCS();
	CServerCS(const CServerCS& _Other);
	~CServerCS();
};

class LocalCS
{
public:
	CServerCS* m_CS;

public:
	LocalCS();
	LocalCS(CServerCS* _CS);
	~LocalCS();
};


class CServerThread : public CServerObject
{
	// 쓰레드 관리 
private:
	static std::unordered_map<std::wstring, CServerThread*> m_mapThread;
	
private:
	static CServerThread* FindAndCreateServerThread(const wchar_t* _ThreadName);
	static CServerThread* FindServerThread(const wchar_t* _ThreadName);
	static CServerThread* CreateServerThread(const wchar_t* _ThreadName);

public:
	static CServerThread* Processing(const wchar_t* _Name, _beginthreadex_proc_type _Type, void* _Arg);
	// 만약에 하고 있는 일이 있다면 뒤이어서 바로 일을 시작할수 있게 만들어 놓는다.
	static bool PushProc(const wchar_t* _Name, _beginthreadex_proc_type _Type);
	// 멈춰라.
	static bool Pause(const wchar_t* _Name);
	static void End();

	class CProcData
	{
	public:
		void*							m_Arg;
		_beginthreadex_proc_type		m_GlobalProc;
		std::function<unsigned(void*)>	m_MemberProc;

	};
	// 쓰레드 객체
private:
	friend unsigned int __stdcall ServerThreadBaseFunc(void* _Arg);

private:
	std::thread							m_ThreadObj;
	CServerCS							m_ThreadCS;
	std::list<CProcData>				m_listThreadProc;


public:
	void PushProc(const CProcData& _Proc);
	const size_t GetProcCount();
	const bool IsProcessing() 
	{
		m_ThreadCS.Enter();
		bool End = !m_ThreadObj.joinable();
		m_ThreadCS.Leave();
		return End;
	}

	 HANDLE GetHandle() { return m_ThreadObj.native_handle(); }


public:
	void StartProcessing();
	void Processing();

public:
	CServerThread();
	~CServerThread();
};

