#include "ServerManager.h"
#include "IOCPServer.h"

CServerManager::CServerManager()
	:SyncCS(CServerCS())
{
	MainPacketQueue.Init();

}


CServerManager::~CServerManager()
{

}


void CServerManager::PacketPreProcessing(PACKET& _Pack)
{
	CIOCPServer::PacketProcessing(_Pack);

}

