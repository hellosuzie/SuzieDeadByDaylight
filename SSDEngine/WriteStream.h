#pragma once
#include "FileStream.h"
class CWriteStream :
	public CFileStream
{
public:
	template<typename T>
	void Write(T& _Buf)
	{
		CFileStream::Write(&_Buf, sizeof(T));
	}
	
	void Write(std::string _Str)
	{
		unsigned int StrSize = (unsigned int)_Str.size();
		Write(StrSize);
		if (StrSize <= 0)
		{
			return;
		}
		CFileStream::Write((void*)_Str.c_str(), (unsigned int)_Str.size() );
	}

	void Write(std::wstring _Str)
	{
		unsigned int StrSize = (unsigned int)_Str.size();
		Write(StrSize);
		if (StrSize <= 0)
		{
			return;
		}
		CFileStream::Write((void*)_Str.c_str(), (unsigned int)(_Str.size() * 2));
	}


public:
	void Write(void* _WBuf, int _ByteSize);

public:
	CWriteStream(const wchar_t* _Path);
	~CWriteStream();
};

