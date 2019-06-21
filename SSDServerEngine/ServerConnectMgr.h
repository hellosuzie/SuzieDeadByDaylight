#pragma once
#include "NetworkClient.h"


class CServerConnectMgr
{
public:
	NetClient*								pLogicClient;
	

public:
	const bool IsConnect() 
	{
		if (nullptr == pLogicClient)
			return false;

		return pLogicClient->IsConnect();
	}


public:
	void SendData(char* _Data, const unsigned int& _Size,const unsigned int& _iHeader, const unsigned int& _iDataType = 0);
	void SendPing();


public:
	virtual void DisConnectLogicServer();


protected:
	CServerConnectMgr() :pLogicClient(nullptr) {}
	virtual ~CServerConnectMgr() 
	{
	
	}

};

