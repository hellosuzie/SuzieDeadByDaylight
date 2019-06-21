#include "GlobalServerViewer.h"
#include <IOCPServer.h>
#include <DXMacro.h>

CNetworkClient*		GlobalServerViewer::pLogicClient = nullptr;

const bool GlobalServerViewer::ConnectToLogicServer() 
{
	pLogicClient = new CNetworkClient();
	if (nullptr == pLogicClient)
		return false;

	return pLogicClient->Connect(54321, L"127.0.0.1");
}
const bool GlobalServerViewer::DisConnectToLogicServer() 
{
	if (nullptr == pLogicClient)
		return false;

	if (false == pLogicClient->IsConnect())
		return false;


	pLogicClient->PacketSend(REQUESTDISCONNECT, 1024);
	CServerThread::End();
	SAFE_DELETE(pLogicClient)
	return true;

}