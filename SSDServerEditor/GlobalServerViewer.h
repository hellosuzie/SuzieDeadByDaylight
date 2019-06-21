#pragma once
#include <NetworkClient.h>



class GlobalServerViewer
{
public:
	static 	CNetworkClient* 		pLogicClient;


public:
	static const bool ConnectToLogicServer();
	static const bool DisConnectToLogicServer();

public:

private:
	GlobalServerViewer() {}
	virtual ~GlobalServerViewer() = 0 {}
};

