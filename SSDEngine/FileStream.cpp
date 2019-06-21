#include "FileStream.h"



CFileStream::CFileStream(const wchar_t* _FileName, const wchar_t* _Mode) : m_File(nullptr)
{
	int a =	_wfopen_s(&m_File, _FileName, _Mode);
}


CFileStream::~CFileStream()
{
	fclose(m_File);
}

void CFileStream::Seek(long _Size, int _Mode)
{
	if (nullptr != m_File)
	{
		fseek(m_File, _Size, _Mode);
	}
}

void CFileStream::Write(void * _WBuf, int _ByteSize)
{
	if (nullptr != m_File)
	{
		fwrite(_WBuf, _ByteSize, 1, m_File);
	}
}

void CFileStream::Read(void * _RBuf, int _BufByteSize, int _ReadByteSize)
{
	if (nullptr != m_File)
	{
		if (_BufByteSize <= _ReadByteSize)
		{
			_ReadByteSize = _BufByteSize;
		}
		fread_s(_RBuf, _BufByteSize, _ReadByteSize, 1, m_File);
	}
}


