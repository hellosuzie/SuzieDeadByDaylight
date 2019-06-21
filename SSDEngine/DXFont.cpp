#include "DXFont.h"
#include"Device.h"
#include"SamplerState.h"
#include"BlendState.h"
#



CDXFont::CDXFont()
	:m_pFontFactory(nullptr), m_pFontWrapper(nullptr)
{
}


CDXFont::~CDXFont()
{
	SAFE_RELEASE(m_pFontFactory)
	SAFE_RELEASE(m_pFontWrapper)
}

const bool CDXFont::Create(const CREATE_DESC& _Desc)
{
	if (S_OK != FW1CreateFactory(FW1_VERSION, &m_pFontFactory) || nullptr == m_pFontFactory) 
	{
		TASSERT(true);
		return false;
	}

	if (S_OK != m_pFontFactory->CreateFontWrapper(DXDEVICE, GetPath().c_str(), &m_pFontWrapper) || nullptr == m_pFontFactory) 
	{
		TASSERT(true);
		return false;
	}


	return true;
}
void CDXFont::DrawString(const wchar_t* _Text,const float& _FontSize, const Vec2& _Pos, const Vec4& _Color,const FW1_TEXT_FLAG& _Align /*= FW1_TEXT_FLAG::FW1_TOP*/)
{
	if (nullptr == m_pFontWrapper)
		return;

	UINT32 Color = _Color.GetABGR();
	m_pFontWrapper->DrawString(DXCONTEXT, _Text, _FontSize, _Pos.x, _Pos.y, Color, _Align);

	DXCONTEXT->HSSetShader(nullptr, 0, 0);
	DXCONTEXT->DSSetShader(nullptr, 0, 0);
	DXCONTEXT->GSSetShader(nullptr, 0, 0);
	DXCONTEXT->VSSetShader(nullptr, 0, 0);
	DXCONTEXT->PSSetShader(nullptr, 0, 0);
	DXCONTEXT->CSSetShader(nullptr, 0, 0);
	DXCONTEXT->OMSetDepthStencilState(nullptr, 0);
	
}