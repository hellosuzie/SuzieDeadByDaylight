#pragma once
#include "DXHeader.h"
#include "Resource.h"
#include <FW1CompileSettings.h>
#include <FW1FontWrapper.h>

class CDXFont : public CResource
{
	IFW1Factory*				m_pFontFactory;
	IFW1FontWrapper*			m_pFontWrapper;



public:
	void DrawString(const wchar_t* _Text, const float& FontSize, const Vec2& _Pos, const Vec4& _Color, const FW1_TEXT_FLAG& _Align = FW1_TEXT_FLAG::FW1_TOP);

public:
	CDXFont();
	~CDXFont();

public:
	NONE_LOAD_RESOURCE(CDXFont)
	CREATE_DESC_BODY(int a;)

	const bool Create(const CREATE_DESC& _Desc);
};

