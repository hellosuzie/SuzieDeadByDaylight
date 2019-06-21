#include "ReadStream.h"




CReadStream::CReadStream(const wchar_t * _Path) : CFileStream(_Path, L"rb")
{
}

CReadStream::~CReadStream()
{
}

void CReadStream::Read(void * _RBuf, int _BufByteSize, int _ReadByteSize)
{
	CFileStream::Read(_RBuf, _BufByteSize, _ReadByteSize);
}

void CReadStream::Seek(long _Size, int _Mode)
{
	CFileStream::Seek(_Size, _Mode);
}

