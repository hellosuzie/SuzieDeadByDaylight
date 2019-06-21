#include "ServerConnectMgr.h"
#include "IOCPServer.h"



void CServerConnectMgr::SendData(char* _Data, const unsigned int& _Size, const unsigned int& _iHeader, const unsigned int& _iDataType/* = 0*/)
{
	if(nullptr != pLogicClient)
		pLogicClient->SendData(_Data, _Size, _iHeader, _iDataType);

}
void CServerConnectMgr::SendPing()
{
	if (nullptr != pLogicClient)
		pLogicClient->SendPing();
}
void CServerConnectMgr::DisConnectLogicServer()
{
	closesocket(pLogicClient->m_UserSock);
	CServerThread::End();
	DELETE_POINTER(pLogicClient)
}