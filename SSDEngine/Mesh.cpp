#include "Mesh.h"
#include "Engine.h"
#include "Device.h"
#include "DXMacro.h"
#include "Material.h"
#include "Scene.h"
#include "InstancingBuffer.h"
#include "Fbx.h"

/////////////////////////////////////////////////				Vertex Buffer				/////////////////////////////
CMesh::CVertexBuffer::CVertexBuffer() 
	:CDXBuffer(), m_VtxSize(0), m_VtxCount(0)
{

}
CMesh::CVertexBuffer::CVertexBuffer(const UINT& _iVtxCount, const UINT& _iVtxSize, const D3D11_USAGE& _VtxUsage)
	: CDXBuffer(), m_VtxSize(_iVtxSize), m_VtxCount(_iVtxCount)
{
	m_BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_BufferDesc.Usage = _VtxUsage;
	
	if (m_BufferDesc.Usage == D3D11_USAGE::D3D11_USAGE_DYNAMIC)
		m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	else if (m_BufferDesc.Usage == D3D11_USAGE::D3D11_USAGE_STAGING)
		m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	else
		m_BufferDesc.CPUAccessFlags = 0;

	m_BufferDesc.ByteWidth = m_VtxCount * m_VtxSize;
}

CMesh::CVertexBuffer::~CVertexBuffer() 
{
}

/////////////////////////////////////////////////				Index Buffer				/////////////////////////////

CMesh::CIndexBuffer::CIndexBuffer()
	:CDXBuffer(), m_IdxSize(0), m_IdxCount(0), m_IBFormat(DXGI_FORMAT_UNKNOWN)
{

}

CMesh::CIndexBuffer::CIndexBuffer(const UINT& _iVtxIndex, const UINT& _iIdxCount, const UINT& _iIdxSize, const D3D11_USAGE& _IdxUsage, const DXGI_FORMAT& _IdxFm)
	: CDXBuffer(), m_VtxIndex(_iVtxIndex), m_IdxSize(_iIdxSize), m_IdxCount(_iIdxCount), m_IBFormat(_IdxFm)
{
	m_BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_BufferDesc.Usage = _IdxUsage;

	if (m_BufferDesc.Usage == D3D11_USAGE::D3D11_USAGE_DYNAMIC)
		m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	else if (m_BufferDesc.Usage == D3D11_USAGE::D3D11_USAGE_STAGING)
		m_BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	else
		m_BufferDesc.CPUAccessFlags = 0;

	m_BufferDesc.ByteWidth = m_IdxSize * m_IdxCount;
	m_BufferDesc.StructureByteStride = 0;
	m_BufferDesc.MiscFlags = 0;


}
CMesh::CIndexBuffer::~CIndexBuffer()
{
}

/////////////////////////////////////////////////				Mesh						///////////////////////////

CMesh::CMesh()
	: m_DrawMode(D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST), m_bDraw(true)
{
}

CMesh::CMesh(const CMesh& _Other)
	:m_DrawMode(_Other.m_DrawMode), m_bDraw(_Other.m_bDraw)
{

}

CMesh::~CMesh()
{
	m_vecVB.clear();
	m_vecIB.clear();
}

void CMesh::Update()
{
	DXCONTEXT->IASetPrimitiveTopology(m_DrawMode);
}

void CMesh::VBUpdate(const int& _Index, void* _pData)
{
	if (nullptr == _pData || m_vecVB.size() <= _Index)
		return;

	m_vecVB[_Index]->MappingBuffer(_pData);
}

void CMesh::VtxRender(const int& _Index, const UINT& _DrawCount/* = -1*/)
{
	if (m_vecVB.size() <= _Index) 
	{
		return;
	}

	UINT OffSet = 0;
	ID3D11Buffer* VB = m_vecVB[_Index]->m_pBuffer;
	UINT Stride = m_vecVB[_Index]->m_VtxSize;
	
	// 현재 랜더링할 VertexBuffer를 IA단계에 결속
	DXCONTEXT->IASetVertexBuffers(0, 1, &VB, &Stride, &OffSet);
	if (-1 == _DrawCount) 
	{
		DXCONTEXT->Draw(m_vecVB[_Index]->m_VtxCount, 0);
	}
	else
	{
		DXCONTEXT->Draw(_DrawCount, 0);
	}
}
void CMesh::VtxRender(const int& _Index, const UINT& _DrawCount, const UINT& _OffSet)
{
	if (m_vecVB.size() <= _Index)
	{
		return;
	}

	UINT OffSet = 0 ;
	ID3D11Buffer* VB = m_vecVB[_Index]->m_pBuffer;
	UINT Stride = m_vecVB[_Index]->m_VtxSize;

	// 현재 랜더링할 VertexBuffer를 IA단계에 결속
	DXCONTEXT->IASetVertexBuffers(0, 1, &VB, &Stride, &OffSet);
	DXCONTEXT->Draw(_DrawCount, _OffSet);
	
}

void CMesh::Render(int _Idx) 
{
	UINT OffSet = 0;

	int a = m_vecIB[_Idx]->m_VtxIndex;
 
	ID3D11Buffer* VB = m_vecVB[m_vecIB[_Idx]->m_VtxIndex]->m_pBuffer;
	UINT Stride = m_vecVB[m_vecIB[_Idx]->m_VtxIndex]->m_VtxSize;

	// 현재 랜더링할 VertexBuffer를 IA단계에 결속
	DXCONTEXT->IASetVertexBuffers(0, 1, &VB, &Stride, &OffSet);
	// 현재 랜더링할 IndexBuffer를 IA단계에 결속
	DXCONTEXT->IASetIndexBuffer(m_vecIB[_Idx]->m_pBuffer, m_vecIB[_Idx]->m_IBFormat, 0);
	// IA 단계에 셋팅된 VB,IB를 Index Count 만큼 랜더링
	DXCONTEXT->DrawIndexed(m_vecIB[_Idx]->m_IdxCount, 0, 0);
}

void CMesh::Render()
{
	UINT OffSet = 0;

	for (size_t i = 0; i < m_vecIB.size(); i++)
	{
		if (m_vecIB[i]->m_VtxIndex >= m_vecVB.size())
			continue;

		ID3D11Buffer* VB = m_vecVB[m_vecIB[i]->m_VtxIndex]->m_pBuffer;
		UINT Stride = m_vecVB[m_vecIB[i]->m_VtxIndex]->m_VtxSize;

		// 현재 랜더링할 VertexBuffer를 IA단계에 결속
		DXCONTEXT->IASetVertexBuffers(0, 1, &VB, &Stride, &OffSet);
		// 현재 랜더링할 IndexBuffer를 IA단계에 결속
		DXCONTEXT->IASetIndexBuffer(m_vecIB[i]->m_pBuffer, m_vecIB[i]->m_IBFormat, 0);
		// IA 단계에 셋팅된 VB,IB를 Index Count 만큼 랜더링
		DXCONTEXT->DrawIndexed(m_vecIB[i]->m_IdxCount, 0, 0);
	}
}

void CMesh::InstancingRender()
{
}

CMesh::CVertexBuffer* CMesh::CreateVertexBuffer(const UINT _iVtxCount, UINT _iVtxSize, D3D11_USAGE _VtxUsage, void* _VtxMem)
{
	CVertexBuffer* NewVB = new CVertexBuffer(_iVtxCount, _iVtxSize, _VtxUsage);

	if (nullptr == NewVB->CreateBuffer(_VtxMem)) 
	{
		TASSERT(true);
		SAFE_DELETE(NewVB);
		return nullptr;
	}

	m_vecVB.push_back(NewVB);
	return NewVB;
}
CMesh::CIndexBuffer*  CMesh::CreateIndexBuffer(UINT _iVtxUdx, UINT _iIdxCount, UINT _iIdxSize, D3D11_USAGE _IdxUsage, void* _IdxMem, DXGI_FORMAT _IdxFm)
{
	CIndexBuffer* NewIB = new CIndexBuffer(_iVtxUdx, _iIdxCount, _iIdxSize, _IdxUsage, _IdxFm);

	if (false == NewIB->CreateBuffer(_IdxMem))
	{
		TASSERT(true);
		SAFE_DELETE(NewIB);
		return nullptr;
	}

	m_vecIB.push_back(NewIB);
	return NewIB;
}

CMesh::CVertexBuffer* CMesh::GetVtxBuffer(const int& _Index)
{
	if (m_vecVB.size() <= _Index)
	{
		TASSERT(true);
		return nullptr;
	}

	return m_vecVB[_Index];
}
const bool CMesh::Create(const CREATE_DESC& _Desc)
{
	if (nullptr == DXDEVICE)
		return  true;

 	CVertexBuffer* NewVB = CreateVertexBuffer(_Desc.iVtxCount, _Desc.iVtxSize, _Desc.VtxUsage, _Desc.VtxMem);
	
	if (0 != _Desc.iIdxCount) 
	{
		CIndexBuffer* NewIB = CreateIndexBuffer(0, _Desc.iIdxCount, _Desc.iIdxSize, _Desc.IdxUsage, _Desc.IdxMem,_Desc.IBFormat);
	}

	if ( nullptr == NewVB )
	{
		TASSERT(true);
		return false;
	}

	m_DrawMode = _Desc.DrawMode;

	return true;
}

///////////////////////////////////////////////			FBX Mesh				//////////////////////////////////

CFBXMesh::CFBXMesh() 
	:CMesh(), m_MeshIdx(-1), m_Fbx(nullptr)
{
	m_vecMaterialList.clear();
}

CFBXMesh::CFBXMesh(const CFBXMesh& _Other)
	:CMesh(_Other), m_MeshIdx(_Other.m_MeshIdx), m_Fbx(_Other.m_Fbx)
{
	m_vecVB.assign(_Other.m_vecVB.begin(), _Other.m_vecVB.end());
	m_vecIB.assign(_Other.m_vecIB.begin(), _Other.m_vecIB.end());
	m_vecMaterialList.clear();
	m_vecMaterialList.assign(m_vecIB.size(), std::list<SPTR<CMaterial>>());
}

CFBXMesh::~CFBXMesh()
{
	m_vecMaterialList.clear();
}


void CFBXMesh::Render()
{
	DXCONTEXT->IASetPrimitiveTopology(m_DrawMode);
	ID3D11Buffer* VB = m_vecVB[0]->m_pBuffer;
	UINT Stride = m_vecVB[0]->m_VtxSize;
	UINT OffSet = 0;

	// 현재 랜더링할 VertexBuffer를 IA단계에 결속
	DXCONTEXT->IASetVertexBuffers(0, 1, &VB, &Stride, &OffSet);

	for (UINT Idx = 0; Idx < m_vecIB.size(); ++Idx)
	{
		DXCONTEXT->IASetIndexBuffer(m_vecIB[Idx]->m_pBuffer, m_vecIB[Idx]->m_IBFormat, 0);

		for (SPTR<CMaterial> Mat : m_vecMaterialList[Idx])
		{
			if (CSceneRenderMgr::IsDeferred != Mat->IsDeferred())
			{
				continue;
			}

			if (nullptr == m_Fbx)
			{
				return;
			}

			if (true == Mat->IsDeferred())
			{
				Mat->SetConstantBufferData(L"MaterialColor", CBUFFER::MaterialColor(m_Fbx->GetBoneMesh(0)->GetMaterialValue(Idx)->MaterialColor.Emissive));
				Vec4 Tem = m_Fbx->GetBoneMesh(0)->GetMaterialValue(Idx)->MaterialColor.Emissive;
			}
			else
			{
				//Mat->SetConstantBufferData(L"ForwardLightBuffer", CBUFFER::ForwardLightBuffer(CSceneRenderMgr::FLightBuffer));
			}

			Mat->Update();
			DXCONTEXT->DrawIndexed(m_vecIB[Idx]->m_IdxCount, 0, 0);
			Mat->StateRollBack();
			Mat->TexSamRollBack();
			CDevice::ResetConstantBuffer();
		}
	}
}

void CFBXMesh::InstancingRender()
{
	DXCONTEXT->IASetPrimitiveTopology(m_DrawMode);
	ID3D11Buffer* VB[2] = { m_vecVB[m_vecIB[0]->m_VtxIndex]->m_pBuffer, CInstancingBuffer::Inst()->GetInstancingBuffer() };
	UINT Stride[2] = { m_vecVB[m_vecIB[0]->m_VtxIndex]->m_VtxSize, sizeof(tInstancingData) };
	UINT OffSet[2] = { 0, 0 };

	// 현재 랜더링할 VertexBuffer를 IA단계에 결속
	DXCONTEXT->IASetVertexBuffers(0, 2, VB, Stride, OffSet);

	for (UINT Idx = 0; Idx < m_vecIB.size(); ++Idx)
	{
		DXCONTEXT->IASetIndexBuffer(m_vecIB[Idx]->m_pBuffer, m_vecIB[Idx]->m_IBFormat, 0);

		for (SPTR<CMaterial> Mat : m_vecMaterialList[Idx])
		{
			if (CSceneRenderMgr::IsDeferred != Mat->IsDeferred())
			{
				continue;
			}

			if (nullptr == m_Fbx)
			{
				return;
			}
			Mat->SetConstantBufferData(L"MaterialColor", CBUFFER::MaterialColor(m_Fbx->GetBoneMesh(0)->GetMaterialValue(Idx)->MaterialColor.Emissive));
			Vec4 Tem = m_Fbx->GetBoneMesh(0)->GetMaterialValue(Idx)->MaterialColor.Emissive;
			CBUFFER::MaterialColor matcolor = CBUFFER::MaterialColor(m_Fbx->GetBoneMesh(0)->GetMaterialValue(Idx)->MaterialColor.Emissive);

			SPTR<CMaterial> TempMat = CResourceMgr<CMaterial>::Find(L"BaseFBXInstMaterial")->Clone();
			std::unordered_map<unsigned int, TexData>::iterator StartIter = Mat->m_mapTexture.begin();
			std::unordered_map<unsigned int, TexData>::iterator EndIter = Mat->m_mapTexture.end();
			for (; StartIter != EndIter; ++StartIter)
			{
				TempMat->m_mapTexture.insert(std::unordered_map<unsigned int, TexData>::value_type(StartIter->first, StartIter->second));
			}

			TempMat->SetConstantBufferData(L"MaterialColor", CBUFFER::MaterialColor(m_Fbx->GetBoneMesh(0)->GetMaterialValue(Idx)->MaterialColor.Emissive));
			TempMat->Update();
			DXCONTEXT->DrawIndexedInstanced(m_vecIB[Idx]->m_IdxCount, CInstancingBuffer::Inst()->GetInstancingCount(), 0, 0, 0);
			TempMat->StateRollBack();
			TempMat->TexSamRollBack();
			CDevice::ResetConstantBuffer();
		}
	}
}

void CFBXMesh::SetRasterizerState(const wchar_t* _RSKey)
{
	size_t MatCount = m_vecMaterialList.size();
	
	for (size_t i = 0; i < MatCount; i++)
	{
		m_vecMaterialList[i].front()->SetRasterizerState(_RSKey);
	}
}
CFBXMesh* CFBXMesh::Clone()
{
	return new CFBXMesh(*this);
}

void CFBXMesh::AllocateMeshMaterial()
{
	m_vecMaterialList.assign(m_vecIB.size(), std::list<SPTR<CMaterial>>());
}