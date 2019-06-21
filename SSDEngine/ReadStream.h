#pragma once
#include "FileStream.h"

class CReadStream :
	public CFileStream
{
public:
	template<typename T>
	void Read(T& _RBuf)
	{
		Read(&_RBuf, sizeof(T), sizeof(T));
	}

	void Read(void* _RBuf, unsigned int _Size)
	{
		Read(_RBuf, _Size, _Size);
	}

	void Read(std::string& _Str)
	{
		unsigned int Size = 0;
		Read(&Size, sizeof(unsigned int), sizeof(unsigned int));
		if (Size <= 0)
		{
			return;
		}
		_Str.resize(Size);
		Read((void*)_Str.c_str(), Size, Size);
	}

	void Read(std::wstring& _Str)
	{
		unsigned int Size = 0;
		Read(&Size, sizeof(unsigned int), sizeof(unsigned int));
		if (Size <= 0)
		{
			return;
		}
		_Str.resize(Size);
		Read((void*)_Str.c_str(), Size * 2, Size * 2);
	}

	void Read(void* _RBuf, int _BufByteSize, int _ReadByteSize);
	void Seek(long _Size, int _Mode = SEEK_SET);

public:
	CReadStream(const wchar_t* _Path);
	~CReadStream();
};

