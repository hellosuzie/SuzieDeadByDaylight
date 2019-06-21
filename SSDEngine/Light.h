#pragma once
#include "SubTrans.h"
#include "DXEnum.h"
#include "DXStruct.h"
#include "DXMath.h"

typedef struct _tagLIGHTDATA 
{
	RENDER_LAYER			Layer;
	LIGHT_TYPE				Type;
	bool					VolumeCheck;
	std::wstring			MeshName;

}LIGHT_DATA;


class CLight : public CSubTrans
{
private:
	SPTR<class CMesh>						m_LightMesh;
	SPTR<class CMaterial>					m_LightMaterial;

	SPTR<class CMesh>						m_VolumeMesh;
	SPTR<class CMaterial>					m_VolumeMaterial;

	bool									m_bBufferUpdate;

	Vec4									m_LightDir;
	bool									m_VolumeCheck;

public:
	CBUFFER::LightBuffer					m_LightBufferData;
	LIGHT_DATA								m_LightData;

protected:
	SPTR<class CDebugRenderer>				m_DebugRen;

public:
	void SetLightBuffer(const CBUFFER::LightBuffer& _Buffer)
	{
		m_bBufferUpdate = true;
		m_LightBufferData = _Buffer;
	}
	void SetLightSize(Vec3 _Size)
	{
		SetSubSize(_Size);
	}
	void SetLightPivot(Vec3 _Pivot)
	{
		SetSubPivot(_Pivot);
	}
	void SetLightColor(const Vec4& _Color) 
	{
		m_bBufferUpdate = true;
		m_LightBufferData.LightColor = _Color;
	}
	void SetLightDir(const Vec3 _LightDir) 
	{
		m_bBufferUpdate = true;
		m_LightDir = _LightDir.GetNormalizeVec();
	}
	void SetLightPower(const float& _Pow) 
	{
		m_bBufferUpdate = true;
		m_LightBufferData.LightPower = _Pow;
	}
	void SetLightType(const LIGHT_TYPE& _Type) 
	{
		m_bBufferUpdate = true;
		m_LightBufferData.LightType = _Type;
	}
	//void SetLightRange(const float& _Range) 
	//{
	//	m_bBufferUpdate = true;
	//	m_LightData.LightRange = _Range;
	//}
	void SetLightAtenuation(const Vec4& _Atenuation) 
	{
		m_bBufferUpdate = true;
		m_LightBufferData.LightAtenuation = _Atenuation;
	}
	//void SetSpotPower(const float& _Power) 
	//{
	//	m_bBufferUpdate = true;
	//	m_LightData.SpotPower = _Power;
	//}
	void SetSpecPower(const float& _Power)
	{
		m_bBufferUpdate = true;
		m_LightBufferData.SpecularPower = _Power;
	}
	void SetAmbiFactor(const float& _Factor)
	{
		m_bBufferUpdate = true;
		m_LightBufferData.AmbientFactor = _Factor;
	}

	Vec3 GetLightSize()
	{
		return m_pSubData->SubSize;
	}
	Vec3 GetLightPivot()
	{
		return m_pSubData->SubPivot;
	}
	const Vec4 GetLightColor()
	{
		return m_LightBufferData.LightColor;
	}
	const Vec3 GetLightDir()
	{
		return m_LightDir;
	}
	const float GetLightPower()
	{
		return m_LightBufferData.LightPower;
	}
	const LIGHT_TYPE GetLightType()
	{
		return (LIGHT_TYPE)m_LightBufferData.LightType;
	}
	const float GetLightRange()
	{
		return m_LightBufferData.LightRange;
	}
	const Vec4 GetLightAtenuation()
	{
		return m_LightBufferData.LightAtenuation;
	}
	const float GetSpotPower()
	{
		return m_LightBufferData.SpotPower;
	}
	const float GetSpecPower()
	{
		return m_LightBufferData.SpecularPower;
	}
	const float GetAmbiFactor()
	{
		return m_LightBufferData.AmbientFactor;
	}
	CBUFFER::LightBuffer GetLightData()
	{
		return m_LightBufferData;
	}
	const std::wstring GetVolumeMeshName();

	bool GetIsVolume()
	{
		return m_VolumeCheck;
	}

public:
	const bool Init(const LIGHT_DATA& _InitData);
	void PostUpdate(const float& _DeltaTime);

public:
	void Lighting(SPTR<class CCamera> _Camera);
	void VolumeLighting(SPTR<class CCamera> _Camera);

	void SetVolumeLightMesh(const std::wstring& _MeshName);
	
	float TriangletoExponent(const float _Diameter, const float _Height);

public:
	CLight();
	~CLight();
};

