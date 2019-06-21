#pragma once
#include "ServerThread.h"
#include <vector>
#include <list>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include "PacketHeader.h"
#include <queue>
#include <deque>


template<typename  KEY, typename VALUE>
class CSyncUMap
{
public:
	CServerCS								m_SyncObj;
	std::unordered_map<KEY, VALUE>			m_UMap;

public:
	size_t Size()
	{
		LocalCS LCS(&m_SyncObj);
		return m_UMap.size();
	}

	void Insert(const KEY& _Key, const VALUE& _Value)
	{
		LocalCS LCS(&m_SyncObj);
		m_UMap.insert(std::unordered_map<KEY, VALUE>::value_type(_Key, _Value));
	}

	VALUE Find(const KEY& _Key)
	{
		LocalCS LCS(&m_SyncObj);

		std::unordered_map<KEY, VALUE>::iterator FindIter = m_UMap.find(_Key);

		if (FindIter == m_UMap.end())
		{
			return VALUE();
		}

		return FindIter->second;
	}

	KEY FindKey(KEY _Value)
	{
		LocalCS LCS(&m_SyncObj);


		std::unordered_map<KEY, VALUE>::iterator Find = m_UMap.find(_Value);

		if (Find == m_UMap.end())
			return nullptr;

		return Find->first;
	}

	KEY FindKey(const VALUE& _Value)
	{
		LocalCS LCS(&m_SyncObj);

		std::unordered_map<KEY, VALUE>::iterator Start = m_UMap.begin();
		std::unordered_map<KEY, VALUE>::iterator End = m_UMap.end();

		for (; Start != End; ++Start)
		{
			if (Start->second == _Value)
				return Start->first;
		}

		return nullptr;
	}



	std::list<KEY> GetOtherKEY(const KEY& _Key)
	{
		LocalCS LCS(&m_SyncObj);

		std::unordered_map<KEY, VALUE>::iterator ValueStart = m_UMap.begin();
		std::unordered_map<KEY, VALUE>::iterator ValueEnd = m_UMap.end();

		std::list<KEY> OtherUser;

		for (; ValueStart != ValueEnd; ++ValueStart)
		{
			if (ValueStart->first == _Key)
				continue;

			OtherUser.push_back(ValueStart->first);
		}
		return OtherUser;
	}


	std::list<KEY> GetOtherKEY(const VALUE& _Value)
	{
		LocalCS LCS(&m_SyncObj);

		std::unordered_map<KEY, VALUE>::iterator ValueStart = m_UMap.begin();
		std::unordered_map<KEY, VALUE>::iterator ValueEnd = m_UMap.end();

		std::list<KEY> OtherUser;

		for (; ValueStart != ValueEnd; ++ValueStart)
		{
			if (ValueStart->second == _Value)
				continue;

			OtherUser.push_back(ValueStart->first);
		}

		return OtherUser;
	}

	const bool ReplaceValue(const KEY _Key, const VALUE _NewVal)
	{
		LocalCS LCS(&m_SyncObj);

		std::unordered_map<KEY, VALUE>::iterator Find = m_UMap.find(_Key);

		if (Find != m_UMap.end())
		{
			Find->second = _NewVal;
			return true;
		}

		return false;
	}

	const bool Erase(const KEY _Key)
	{
		LocalCS LCS(&m_SyncObj);
		std::unordered_map<KEY, VALUE>::iterator EraseIter = m_UMap.find(_Key);

		if (EraseIter == m_UMap.end())
		{
			assert(nullptr);
			return false;
		}

		m_UMap.erase(EraseIter);
		return true;
	}

	const bool Erase(const VALUE _Value)
	{
		LocalCS LCS(&m_SyncObj);
		std::unordered_map<KEY, VALUE>::iterator Start = m_UMap.begin();
		std::unordered_map<KEY, VALUE>::iterator End = m_UMap.end();

		for (; Start != End; ++Start)
		{
			if (Start->second == _Value)
			{
				m_UMap.erase(Start);
				return true;
			}
		}

		return false;
	}

	const bool Empty()
	{
		LocalCS LCS(&m_SyncObj);
		bool Check = m_UMap.empty();
		return Check;
	}

	void Clear()
	{
		LocalCS LCS(&m_SyncObj);
		m_UMap.clear();
	}


public:
	CSyncUMap() {}
	~CSyncUMap() {}
};





template<typename VALUE>
class CSyncSet
{
public:
	CServerCS								m_SyncObj;
	std::set<VALUE>							m_Set;

public:
	size_t Size()
	{
		LocalCS LCS(&m_SyncObj);
		return m_Set.size();
	}

	void Insert(const VALUE& _Value)
	{
		LocalCS LCS(&m_SyncObj);
		m_Set.insert(_Value);
	}

	VALUE Find(const VALUE& _Key)
	{
		LocalCS LCS(&m_SyncObj);

		std::set<VALUE>::iterator FindIter = m_Set.find(_Key);

		if (FindIter == m_Set.end())
		{
			return VALUE();
		}

		return (*FindIter);
	}

	void Erase(const VALUE& _Key)
	{
		LocalCS LCS(&m_SyncObj);

		m_Set.erase(_Key);
	}


	void Clear()
	{
		LocalCS LCS(&m_SyncObj);
		m_Set.clear();
	}

};

template<typename VALUE>
class CSyncUSet
{
public:
	CServerCS								m_SyncObj;
	std::unordered_set<VALUE>				m_USet;

public:
	size_t Size()
	{
		LocalCS LCS(&m_SyncObj);
		return m_USet.size();
	}

	void Insert(const VALUE& _Value)
	{
		LocalCS LCS(&m_SyncObj);
		m_USet.insert(_Value);
	}

	VALUE Find(const VALUE& _Key)
	{
		LocalCS LCS(&m_SyncObj);

		std::unordered_set<VALUE>::iterator FindIter = m_USet.find(_Key);

		if (FindIter == m_USet.end())
		{
			return VALUE();
		}

		return (*FindIter);
	}

	void Erase(const VALUE& _Key)
	{
		LocalCS LCS(&m_SyncObj);

		m_USet.erase(_Key);
	}


	void Clear()
	{
		LocalCS LCS(&m_SyncObj);
		m_USet.clear();
	}

};


template<typename VALUE>
class CSyncList
{
public:
	CServerCS									m_SyncObj;
	std::list<VALUE>							m_List;

public:
	size_t Size()
	{
		LocalCS LCS(&m_SyncObj);
		return m_List.size();
	}

	void push_back(const VALUE& _Value)
	{
		LocalCS LCS(&m_SyncObj);
		m_List.push_back(_Value);
	}

	void Clear()
	{
		LocalCS LCS(&m_SyncObj);
		m_List.clear();
	}

	const VALUE Find(const VALUE& _Val)
	{
		LocalCS Lock(&m_SyncObj);

		std::list<VALUE>::iterator Start = m_List.begin();
		std::list<VALUE>::iterator End = m_List.end();

		for (; Start != End; ++Start)
		{
			if ((*Start) == _Val)
				return (*Start);
		}

		return nullptr;
	}

	void Erase(const VALUE& _Val)
	{
		LocalCS LCS(&m_SyncObj);

		std::list<VALUE>::iterator Start = m_List.begin();
		std::list<VALUE>::iterator End = m_List.end();

		for (; Start != End; )
		{
			if ((*Start) == _Val)
			{
				Start = m_List.erase(Start);
				return;
			}
			else
			{
				++Start;
			}
		}

	}

	const bool Empty()
	{
		LocalCS Lock(&m_SyncObj);
		return m_List.empty();
	}

};


template<typename VALUE>
class CSyncQueue
{
public:
	CServerCS									m_SyncObj;
	std::queue<VALUE>							m_Queue;

public:
	size_t Size()
	{
		LocalCS LCS(&m_SyncObj);
		return m_Queue.size();
	}

	void Push(const VALUE& _Value)
	{
		LocalCS LCS(&m_SyncObj);
		m_Queue.push(_Value);
	}

	const VALUE Pop()
	{
		LocalCS LCS(&m_SyncObj);
		VALUE PopData = m_Queue.front();
		m_Queue.pop();
		return PopData;
	}

	const bool Empty()
	{
		LocalCS Lock(&m_SyncObj);
		return m_Queue.empty();
	}

};



template<typename VALUE>
class CSyncDeque
{
public:
	CServerCS									m_SyncObj;
	std::deque<VALUE*>							m_Deque;
	int											m_MaxSize;
	int											m_Size;

public:
	size_t Size()
	{
		LocalCS LCS(&m_SyncObj);
		return m_Deque.size();
	}

	void SetMaxSize(const int& _Max) 
	{
		if (m_MaxSize > _Max)
			return;

		m_MaxSize = _Max;
	}

	void Push(VALUE* _Value)
	{
		LocalCS LCS(&m_SyncObj);
		m_Deque.push_front(_Value);
	}

	VALUE* Pop()
	{
		LocalCS LCS(&m_SyncObj);
		if (true == m_Deque.empty())
		{
			if (m_MaxSize <= m_Size)
			{
				return nullptr;
			}
			else
			{
				VALUE* NewVal = new VALUE();
				++m_Size;
				return NewVal;
			}
		}
		else
		{
			VALUE* PopData = m_Deque.front();
			m_Deque.pop_front();
			return PopData;
		}
	}

	const bool Empty()
	{
		LocalCS Lock(&m_SyncObj);
		return m_Deque.empty();
	}


	CSyncDeque() 
		:m_MaxSize(1024), m_Size(0)
	{
	
	}
	~CSyncDeque() 
	{
		while (!m_Deque.empty()) 
		{
			VALUE* Val = m_Deque.front();
			m_Deque.pop_front();
			DELETE_POINTER(Val)
		
		}
	}
};


class CPacketQueue
{
private:
	CServerCS									m_SyncObj;
	char										m_PacketStream[STREAM_SIZE];
	int											m_iRemainData;

public:
	template<typename T>
	bool AddData(T* _pData, const unsigned int& _Size)
	{
		LocalCS Lock(&m_SyncObj);
		memcpy(m_PacketStream + m_iRemainData, _pData, _Size);
		m_iRemainData += _Size;
		return true;
	}

	const bool GetData(class CServerUser* _pUser);

	template<typename T>
	bool AddData(T* _pData, const unsigned int& _Size, CServerUser* _pUser)
	{
		LocalCS Lock(&m_SyncObj);
		memcpy(m_PacketStream + m_iRemainData, _pData, _Size);
		m_iRemainData += _Size;

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

	const bool PushUserPacket(CServerUser* _pUser);


private:
	const DWORD ExtractionPacketSize(class CServerUser* _pUser);

private:
	unsigned int m_iHead;
	unsigned int m_iTail;
	unsigned int m_iSize;
	unsigned int m_iCapacity;
	PACKET* m_pPacket;


public:
	void destroy()
	{
		delete m_pPacket;
	}
	unsigned int Size() const
	{
		return m_iSize;
	}

	void Clear()
	{
		LocalCS Lock(&m_SyncObj);
		m_iHead = m_iTail = m_iSize = 0;
	}
	bool Full() const { return ((m_iTail + 1) % m_iCapacity) == m_iHead; }
	bool Empty() const { return m_iHead == m_iTail; }

	const bool Init()
	{
		m_iHead = m_iTail = 0;
		m_iSize = 0;
		m_iCapacity = 300;
		m_pPacket = new PACKET[m_iCapacity];

		// 패킷 스트림 초기화
		m_iRemainData = 0;
		memset(m_PacketStream, 0, sizeof(m_PacketStream));
		return true;
	}

	void Resize(int _iCapacity)
	{
		PACKET* pPacket = new PACKET[_iCapacity];
		ZeroMemory(pPacket, sizeof(PACKET)*_iCapacity);
		memcpy(pPacket, m_pPacket, sizeof(PACKET)*m_iCapacity);

		m_iCapacity = _iCapacity;
		DELETE_ARRPOINTER(m_pPacket);
		m_pPacket = pPacket;
	}

	const bool Push(const PACKET& _Pack)
	{
		if (Full() == true)
			return false;

		LocalCS Lock(&m_SyncObj);
		int iTail = (m_iTail + 1) % m_iCapacity;
		memcpy(&m_pPacket[iTail], &_Pack, sizeof(PACKET));
		m_iTail = iTail;
		++m_iSize;
		return true;
	}

	const bool Pop(PACKET* _pPacket)
	{
		if (Empty() == true)
			return false;

		LocalCS Lock(&m_SyncObj);
		int iHead = (m_iHead + 1) % m_iCapacity;
		memcpy(_pPacket, &m_pPacket[iHead], sizeof(PACKET));
		m_iHead = iHead;
		--m_iSize;
		return true;
	}


public:
	CPacketQueue();
	~CPacketQueue();
};