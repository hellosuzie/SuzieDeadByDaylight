#include "SyncContainer.h"
#include "SSDServerCore.h"

CPacketQueue::CPacketQueue() 
	: m_SyncObj(CServerCS()), m_iHead(0), m_iTail(0), m_iSize(0), m_iCapacity(0)
	, m_pPacket(nullptr)
{

}
CPacketQueue::~CPacketQueue() 
{
	DELETE_ARRPOINTER(m_pPacket)
}

const bool CPacketQueue::PushUserPacket(CServerUser* _pUser)
{
	LocalCS Lock(&m_SyncObj);

	if (nullptr == _pUser)
		return false;

	bool PushCheck = false;

	while (_pUser->ExistUserPacket()) 
	{
		PACKET UserPacket = _pUser->GetUserPacket();
		int iTail = (m_iTail + 1) % m_iCapacity;
		memcpy(&m_pPacket[iTail], &UserPacket, sizeof(PACKET));
		m_iTail = iTail;
		++m_iSize;
		PushCheck = true;
	}

	return PushCheck;
}
const DWORD CPacketQueue::ExtractionPacketSize(CServerUser* _pUser)
{
	if (nullptr == _pUser)
		return 0;

	DWORD PacketLen = 0;
	CopyMemory(&PacketLen, _pUser->m_RecvBuffer.m_Packet, sizeof(DWORD));
	return PacketLen;
}
const bool CPacketQueue::GetData(CServerUser* _pUser)
{
	LocalCS Lock(&m_SyncObj);

	while (true)
	{
		//남은 길이가 4바이트 보다 작으면 중지
		if (m_iRemainData < sizeof(DWORD))
			break;

		// 패킷 길이 얻어오기
		int iPacketLen = 0;
		CopyMemory(&iPacketLen, m_PacketStream, sizeof(DWORD));

		//남은 길이가 패킷길이보다 작으면 중지
		if (m_iRemainData < iPacketLen)
			break;

		//패킷큐가 가득차있으면 중지
		if (Full() == true)
			break;

		//패킷으로 데이터 추출
		PACKET NewPacket;
		ZeroMemory(&NewPacket, sizeof(PACKET));

		memcpy(&NewPacket, m_PacketStream, iPacketLen);
		NewPacket.pUser = _pUser;

		//패킷큐에 집어넣기
		int iTail = (m_iTail + 1) % m_iCapacity;
		memcpy(&m_pPacket[iTail], &NewPacket, sizeof(PACKET));
		m_iTail = iTail;
		++m_iSize;

		//패킷 스트림에서 패킷만큼 앞으로 이동
		m_iRemainData -= iPacketLen;
		MoveMemory(m_PacketStream, m_PacketStream + iPacketLen, m_iRemainData);
	}

	return true;
}