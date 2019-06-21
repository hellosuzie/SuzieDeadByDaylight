#include "ThreadMaker.h"




CThreadMaker::CThreadMaker()
{

}


CThreadMaker::~CThreadMaker()
{


}


void CThreadMaker::Release()
{
	DWORD  ThreadCount = (DWORD)m_vecThread.size();


	WaitForMultipleObjects(ThreadCount, &m_vecThreadHandle[0], TRUE, INFINITE);

	for (size_t i = 0; i < m_vecThread.size(); i++)
	{
		if (true == m_vecThread[i].joinable())
		{
			m_vecThread[i].join();
		}
	}

	m_vecThread.clear();
	m_vecThreadHandle.clear();
}