#pragma once
#include <iostream>

class CFileStream
{
public:
	FILE*			m_File;

public:
	CFileStream(const wchar_t* _FileName, const wchar_t* _Mode);
	virtual ~CFileStream();

public:
	void Seek(long _Size, int _Mode = SEEK_SET);
	void Write(void* _WBuf, int _ByteSize);
	void Read(void* _RBuf, int _BufByteSize, int _ReadByteSize);

public:
	static bool FileExistCheck(const wchar_t* _Path)
	{
		errno_t Result = _waccess_s(_Path, 0);
		if (ENOENT == Result || EACCES == Result)
		{
			return false;
		}
		return true;
	}

};

