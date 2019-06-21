#pragma once
#include "Resource.h"
#include "DXHeader.h"


class CDXBuffer : public CResource
{
protected:
	bool											m_bDraw;
	D3D11_BUFFER_DESC								m_BufferDesc;
	ID3D11Buffer*									m_pBuffer;


public:
	void DrawOn() { m_bDraw = true; }
	void DrawOff() { m_bDraw = false; }
	void Draw(const bool& _bDraw) { m_bDraw = _bDraw; }
	const bool IsDraw() { return m_bDraw; }
public:
	ID3D11Buffer* CreateBuffer(void* _pInitData = nullptr);
	void MappingBuffer(void* _pData);

public:
	CDXBuffer();
	~CDXBuffer();
};

