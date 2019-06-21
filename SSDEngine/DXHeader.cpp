#include"DXHeader.h"


const float _tagVEC2::Cross(const _tagVEC2& _Other)
{
	return DirectX::XMVector2Cross(*this, _Other).m128_f32[0];
}
const float _tagVEC2::Dot(const _tagVEC2& _Other) 
{
	return DirectX::XMVector2Dot(*this, _Other).m128_f32[0];
}
_tagVEC2 _tagVEC2::operator=(const _tagVEC2& _Other)
{
	x = _Other.x;
	y = _Other.y;
	return *this;
}

_tagVEC2 _tagVEC2::operator+(const _tagVEC2& _Other) const
{
	_tagVEC2 Result;
	Result.x = x + _Other.x;
	Result.y = y + _Other.y;
	
	return Result;
}

_tagVEC2 _tagVEC2::operator+=(const _tagVEC2& _Other)
{
	x += _Other.x;
	y += _Other.y;
	return *this;
}

_tagVEC2 _tagVEC2::operator-(const _tagVEC2& _Other) const
{
	_tagVEC2 Result;
	Result.x = x - _Other.x;
	Result.y = y - _Other.y;
	return Result;
}

_tagVEC2 _tagVEC2::operator-=(const _tagVEC2& _Other)
{
	x -= _Other.x;
	y -= _Other.y;
	return *this;
}

_tagVEC2 _tagVEC2::operator*(const _tagVEC2& _Other) const
{
	_tagVEC2 Result;
	Result.x = x * _Other.x;
	Result.y = y * _Other.y;
	return Result;
}
_tagVEC2 _tagVEC2::operator*=(const _tagVEC2& _Other)
{
	x *= _Other.x;
	y *= _Other.y;
	return *this;
}

_tagVEC2 _tagVEC2::operator*(const float& _Scala) const 
{
	return _tagVEC2(x*_Scala, y*_Scala);
}
_tagVEC2 _tagVEC2::operator*=(const float& _Scala) 
{
	x *= _Scala;
	y *= _Scala;
	return *this;
}
_tagVEC2 _tagVEC2::operator/(const float& _Scala) const 
{
	return _tagVEC2(x*_Scala, y*_Scala);
}
_tagVEC2 _tagVEC2::operator/=(const float& _Scala) 
{
	x *= _Scala;
	y *= _Scala;
	return *this;
}
_tagVEC2 _tagVEC2::operator*(const struct _tagMAT& _Mat) const
{
	Vec2 Result = *this;

	for (size_t i = 0; i < 2; i++)
		Result.pf[i] = x * _Mat.M[0][i] + y * _Mat.M[1][i];

	return Result;
}
_tagVEC2 _tagVEC2::operator*=(const struct _tagMAT& _Mat)
{
	_tagVEC2 Temp = *this;

	for (size_t i = 0; i < 2; i++)
		pf[i] = Temp.x * _Mat.M[0][i] + Temp.y * _Mat.M[1][i];

	return *this;
}
const bool _tagVEC2::operator==(const _tagVEC2& _Other) const
{
	return (x == _Other.x )&& (y == _Other.y);
}
const bool _tagVEC2::operator!=(const _tagVEC2& _Other) const 
{
	return (x != _Other.x) || (y != _Other.y);
}
const float _tagVEC2::Length() const
{
	if (*this == _tagVEC2(1.f, 0.f) || *this == _tagVEC2(0.f, 1.f) )
		return 1.f;

	//return sqrtf((x*x) + (y*y));
	return DirectX::XMVector2Length(*this).m128_f32[0];
}
_tagVEC2 _tagVEC2::GetNormalizeVec() const
{
	float Len = this->Length();

	// 만약 길이가 0인 벡터가 들어오면 원래값을 반환한다. 
	if (0.f == Len)
		return *this;

	if (1.f == Len)
		return *this;

	//return _tagVEC2(this->x / Len, this->y / Len);
	return Vec2(DirectX::XMVector2Normalize(*this));
}
void _tagVEC2::Normalize() 
{
	float Len = this->Length();

	// 길이가 0인 벡터의 경우 정규화를 수행하지 않는다. 
	if (0.f == Len)
		return;

	if (1.f == Len)
		return;

	//this->x /= Len;
	//this->y /= Len;
	*this = DirectX::XMVector2Normalize(*this);
}

///////////////////////////////////////////////				Vec3				/////////////////////////////
_tagVEC3	_tagVEC3::Zero3{ 0.f,0.f,0.f };
_tagVEC3	_tagVEC3::One3{ 1.f,1.f,1.f };
_tagVEC3	_tagVEC3::Right3{ 1.f,0.f,0.f };
_tagVEC3	_tagVEC3::Left3{ -1.f,0.f,0.f };
_tagVEC3	_tagVEC3::Up3{ 0.f,1.f,0.f };
_tagVEC3	_tagVEC3::Down3{ 0.f, -1.f,0.f };
_tagVEC3	_tagVEC3::Forward3{ 0.f,0.f,1.f };
_tagVEC3	_tagVEC3::Back3{ 0.f,0.f,-1.f };


_tagVEC3 _tagVEC3::operator=(const _tagVEC3& _Other)
{
	x = _Other.x;
	y = _Other.y;
	z = _Other.z;
	return *this;
}

_tagVEC3 _tagVEC3::operator=(const _tagVEC2& _Other) 
{
	x = _Other.x;
	y = _Other.y;
	z = 0.f;
	return *this;
}
_tagVEC3 _tagVEC3::operator=(const  DirectX::XMVECTOR& _XMVec)
{
	DirectX::XMStoreFloat3(&XMF3, _XMVec);
	return *this;
}

_tagVEC3 _tagVEC3::operator+(const _tagVEC3& _Other) const
{
	_tagVEC3 Result;
	Result.x = x + _Other.x;
	Result.y = y + _Other.y;
	Result.z = z + _Other.z;
	return Result;
}
_tagVEC3 _tagVEC3::operator+=(const _tagVEC3& _Other)
{
	x += _Other.x;
	y += _Other.y;
	z += _Other.z;
	return *this;
}
_tagVEC3 _tagVEC3::operator-(const _tagVEC3& _Other) const
{
	_tagVEC3 Result;
	Result.x = x - _Other.x;
	Result.y = y - _Other.y;
	Result.z = z - _Other.z;
	return Result;
}
_tagVEC3 _tagVEC3::operator-=(const _tagVEC3& _Other)
{
	x -= _Other.x;
	y -= _Other.y;
	z -= _Other.z;
	return *this;
}
_tagVEC3 _tagVEC3::operator*(const _tagVEC3& _Other) const
{
	_tagVEC3 Result;
	Result.x = x * _Other.x;
	Result.y = y * _Other.y;
	Result.z = z * _Other.z;
	return Result;
}
_tagVEC3 _tagVEC3::operator*=(const _tagVEC3& _Other)
{
	x *= _Other.x;
	y *= _Other.y;
	z *= _Other.z;
	return *this;
}
_tagVEC3 _tagVEC3::operator*(const struct _tagMAT& _Mat) const 
{
	Vec3 Result = *this;

	for (size_t i = 0; i < 3; i++)
		Result.pf[i] = x * _Mat.M[0][i] + y * _Mat.M[1][i] + z * _Mat.M[2][i];

	return Result;
}
_tagVEC3 _tagVEC3::operator*=(const struct _tagMAT& _Mat) 
{
	Vec3 Temp = *this;

	for (size_t i = 0; i < 3; i++)
		pf[i] = Temp.x * _Mat.M[0][i] + Temp.y * _Mat.M[1][i] + Temp.z * _Mat.M[2][i];

	return *this;
}
const _tagVEC4 _tagVEC3::GetVec4_Normal() const
{
	return _tagVEC4(x, y, z, 0.f);
}
const _tagVEC4 _tagVEC3::GetVec4_Coord() const 
{
	return _tagVEC4(x, y, z, 1.f);
}
const float _tagVEC3::Length() const 
{
	if (*this == _tagVEC3::Right3 || *this == _tagVEC3::Up3 || *this == _tagVEC3::Forward3
		|| *this == _tagVEC3::Left3 || *this == _tagVEC3::Down3 || *this == _tagVEC3::Back3)
		return 1.f;

	//return sqrtf((x*x) + (y*y) + (z*z));
	return DirectX::XMVector3Length(*this).m128_f32[0];
}

const float _tagVEC3::Length_Square() const
{
	if (*this == _tagVEC3::Right3 || *this == _tagVEC3::Up3 || *this == _tagVEC3::Forward3
		|| *this == _tagVEC3::Left3 || *this == _tagVEC3::Down3 || *this == _tagVEC3::Back3)
		return 1.f;

	return ((x * x) + (y * y) + (z * z));
	
}

_tagVEC3 _tagVEC3::GetNormalizeVec() const 
{
	float Len = this->Length();

	// 만약 길이가 0인 벡터가 들어오면 원래값을 반환한다. 
	if (0.f == Len)
		return *this;

	if (1.f == Len)
		return *this;

	//return _tagVEC3(this->x / Len, this->y / Len, this->z / Len);
	return Vec3(DirectX::XMVector3Normalize(*this));
}

void _tagVEC3::Normalize()
{
	float Len = this->Length();

	// 길이가 0인 벡터의 경우 정규화를 수행하지 않는다. 
	if (0.f == Len)
		return;

	if (1.f == Len)
		return;

	//this->x /= Len;
	//this->y /= Len;
	//this->z /= Len;

	*this = DirectX::XMVector3Normalize(*this);
}
_tagVEC3::_tagVEC3(const DirectX::XMVECTOR& _XMVector)
{
	DirectX::XMStoreFloat3(&XMF3,_XMVector);
}


///////////////////////////////////////////////////			Vec4			///////////////////////////////////


_tagVEC4 _tagVEC4::Zero = _tagVEC4(0.f, 0.f, 0.f, 0.f);
_tagVEC4 _tagVEC4::One = _tagVEC4(1.f, 1.f, 1.f, 1.f);
_tagVEC4 _tagVEC4::White = _tagVEC4(1.f, 1.f, 1.f, 1.f);
_tagVEC4 _tagVEC4::Black = _tagVEC4(0.f, 0.f, 0.f, 1.f);
_tagVEC4 _tagVEC4::Red = _tagVEC4(1.f, 0.f, 0.f, 1.f);
_tagVEC4 _tagVEC4::Green = _tagVEC4(0.f, 1.f, 0.f, 1.f);
_tagVEC4 _tagVEC4::Blue = _tagVEC4(0.f, 0.f, 1.f, 1.f);
_tagVEC4 _tagVEC4::Yellow = _tagVEC4(1.f, 1.f, 0.f, 1.f);
_tagVEC4 _tagVEC4::Cyan = _tagVEC4(0.f, 1.f, 1.f, 1.f);
_tagVEC4 _tagVEC4::Magenta = _tagVEC4(1.f, 0.f, 1.f, 1.f);
_tagVEC4 _tagVEC4::Pink = _tagVEC4(0.94f, 0.72f, 0.78f, 1.f);
_tagVEC4 _tagVEC4::PastelRed = _tagVEC4(1.f, 0.43f, 0.43f, 1.f);
_tagVEC4 _tagVEC4::Mint = _tagVEC4(0.81f, 1.f, 0.89f, 1.f);
_tagVEC4 _tagVEC4::LightYellow = _tagVEC4(1.f, 0.89f, 0.45f, 1.f);
_tagVEC4 _tagVEC4::PastelGreen = _tagVEC4(0.71f, 1.f, 0.61f, 1.f);
_tagVEC4 _tagVEC4::Lavender = _tagVEC4(0.58f, 0.62f, 0.74f, 1.f);
_tagVEC4 _tagVEC4::PastelMag = _tagVEC4(0.90f, 0.68f, 0.9f, 1.f);
_tagVEC4 _tagVEC4::PastelBlue = _tagVEC4(0.28f, 0.64f, 1.f, 1.f);


_tagVEC4 _tagVEC4::operator-() const
{
	_tagVEC4 ReturnVec;
	ReturnVec.x = -x;
	ReturnVec.y = -y;
	ReturnVec.z = -z;
	ReturnVec.w = -w;
	return ReturnVec;
}
_tagVEC4 _tagVEC4::operator=(const _tagVEC4& _Other)
{
	x = _Other.x;
	y = _Other.y;
	z = _Other.z;
	w = _Other.w;
	return *this;
}
_tagVEC4 _tagVEC4::operator=(const _tagVEC3& _Vec3) 
{
	x = _Vec3.x;
	y = _Vec3.y;
	z = _Vec3.z;
	w = 0.f;
	return *this;
}
_tagVEC4 _tagVEC4::operator=(const _tagVEC2& _Vec2) 
{
	x = _Vec2.x;
	y = _Vec2.y;
	z = 0.f;
	w = 0.f;
	return *this;
}
_tagVEC4 _tagVEC4::operator=(const DirectX::XMVECTOR& _XMVec)
{
	DirectX::XMStoreFloat4(&XMF4, _XMVec);
	return *this;
}
_tagVEC4 _tagVEC4::operator+(const _tagVEC4& _Other) const
{
	_tagVEC4 Result;
	Result.x = x + _Other.x;
	Result.y = y + _Other.y;
	Result.z = z + _Other.z;
	Result.w = w + _Other.w;
	return Result;
}
_tagVEC4 _tagVEC4::operator+=(const _tagVEC4& _Other)
{
	x += _Other.x;
	y += _Other.y;
	z += _Other.z;
	w += _Other.w;
	return *this;
}
_tagVEC4 _tagVEC4::operator-(const _tagVEC4& _Other) const
{
	_tagVEC4 Result;
	Result.x = x - _Other.x;
	Result.y = y - _Other.y;
	Result.z = z - _Other.z;
	Result.w = w - _Other.w;
	return Result;
}
_tagVEC4 _tagVEC4::operator-=(const _tagVEC4& _Other)
{
	x -= _Other.x;
	y -= _Other.y;
	z -= _Other.z;
	w -= _Other.w;
	return *this;
}
_tagVEC4 _tagVEC4::operator*(const _tagVEC4& _Other) const
{
	_tagVEC4 Result;
	Result.x = x * _Other.x;
	Result.y = y * _Other.y;
	Result.z = z * _Other.z;
	Result.w = w * _Other.w;
	return Result;
}
_tagVEC4 _tagVEC4::operator*=(const _tagVEC4& _Other)
{
	x *= _Other.x;
	y *= _Other.y;
	z *= _Other.z;
	w *= _Other.w;
	return *this;
}
_tagVEC4 _tagVEC4::operator*(const float& _Scala) const 
{
	return _tagVEC4(x*_Scala, y*_Scala, z*_Scala, w*_Scala);
}
_tagVEC4 _tagVEC4::operator*=(const float& _Scala) 
{
	x *= _Scala;
	y *= _Scala;
	z *= _Scala;
	w *= _Scala;
	return *this;
}

_tagVEC4 _tagVEC4::operator/(const float& _Scala) const
{
	return _tagVEC4(x / _Scala, y / _Scala, z / _Scala, w / _Scala);
}

const bool _tagVEC4::operator==(const _tagVEC4& _Other) const 
{
	return (_Other.x == x && _Other.y == y && _Other.z == z && _Other.w == w);
}
const bool _tagVEC4::operator!=(const _tagVEC4& _Other) const
{
	return (_Other.x != x || _Other.y != y || _Other.z != z || _Other.w != w);
}
_tagVEC4 _tagVEC4::operator*(const struct _tagMAT& _Mat) const
{
	_tagVEC4 Result;
	
	for (size_t i = 0; i < 4; i++)
		Result.pf[i] =
				  x * _Mat.M[0][i]
				+ y * _Mat.M[1][i]
				+ z * _Mat.M[2][i]
				+ w * _Mat.M[3][i];

	if (1.f != Result.w && 0.f != Result.w) 
	{
		for (size_t i = 0; i < 3; i++)
			Result.pf[i] /= Result.w;
		
		Result.w = 1.f;
	}



	return Result;
}
_tagVEC4 _tagVEC4::operator*=(const struct _tagMAT& _Mat)
{
	_tagVEC4 Temp = *this;

	for (size_t i = 0; i < 4; i++)
		pf[i] = Temp.x * _Mat.M[0][i]
				+ Temp.y * _Mat.M[1][i]
				+ Temp.z * _Mat.M[2][i]
				+ Temp.w * _Mat.M[3][i];

	if (1.f != w && 0.f != w)
	{
		for (size_t i = 0; i < 3; i++)
			pf[i] /= w;

		w = 1.f;
	}

	return *this;
}

const _tagMAT _tagVEC4::GetQuatRotationMat() 
{
	_tagMAT ReturnMat;
	DirectX::XMStoreFloat4x4(&ReturnMat.XMF4x4, DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&XMF4) ));
	return ReturnMat;
}
IVEC4 _tagVEC4::GetIVec4() const
{
	IVEC4 Result;
	Result.ix = (int)x;
	Result.iy = (int)y;
	Result.iz = (int)z;
	Result.iw = (int)w;
	return Result;
}

IVEC4 _tagVEC4::ConversionIVec4()
{
	ix = (int)x;
	iy = (int)y;
	iz = (int)z;
	iw = (int)w;
	return *this;
}

const float _tagVEC4::Length_Vec3() const 
{
	return DirectX::XMVector3Length(*this).m128_f32[0];
}
const Vec3  _tagVEC4::GetNormalize_Vec3() const 
{
	return Vec3(DirectX::XMVector3Normalize(*this));
}
void  _tagVEC4::Normalize_Vec3()
{
	*this =  Vec3(DirectX::XMVector3Normalize(*this));
}
const float _tagVEC4::Length() const 
{
	return DirectX::XMVector4Length(*this).m128_f32[0];
}
const Vec4  _tagVEC4::GetNormalizeVec() const 
{
	return Vec4(DirectX::XMVector4Normalize(*this));
}
void  _tagVEC4::Normalize() 
{
	*this = DirectX::XMVector4Normalize(*this);
}
const POINT _tagVEC4::GetPoint() const
{
	return POINT{ (LONG)x,(LONG)y };
}
const int _tagVEC4::GetARGB() const
{
	IVEC4 Color = IVEC4((int)(r*255), (int)(g * 255), (int)(b * 255), (int)(a * 255));
	
	return Color.ia << 24 | Color.ir << 16 | Color.ig << 8 | Color.ib  ;
}

const int _tagVEC4::GetRGBA() const
{
	IVEC4 Color = IVEC4((int)(r * 255), (int)(g * 255), (int)(b * 255), (int)(a * 255));

	return Color.ir << 24 | Color.ig << 16 | Color.ib << 8 | Color.ia;
}

const int _tagVEC4::GetABGR() const
{
	IVEC4 Color = IVEC4((int)(r * 255), (int)(g * 255), (int)(b * 255), (int)(a * 255));

	return Color.ia << 24 | Color.ib << 16 | Color.ig << 8 | Color.ir;
}
_tagVEC4::_tagVEC4(const DirectX::XMVECTOR& _XMVec)
{
	DirectX::XMStoreFloat4(&XMF4, _XMVec);
}

_tagVEC4::_tagVEC4(const _tagVEC3& _Axis, const float& _Angle)
{
	DirectX::XMStoreFloat4(&XMF4, DirectX::XMQuaternionRotationNormal(_Axis, _Angle));
	//DirectX::XMStoreFloat4(&XMF4, DirectX::XMVector3Normalize(_Axis.GetXMVector()));
	//(*this) *= sinf(_Angle*0.5f);
	//w = cosf(_Angle*0.5f);
}
_tagVEC4::_tagVEC4(const _tagVEC3& _Vec3) 
	:x(_Vec3.x), y(_Vec3.y), z(_Vec3.z), w(0.f)
{

}
_tagVEC4::_tagVEC4(const _tagVEC2& _Vec2)
	: x(_Vec2.x), y(_Vec2.y), z(0.f), w(0.f)
{

}

////////////////////////////////////////////			Matrix				////////////////////////////////////////////

void _tagMAT::DivisionWorldMat(DirectX::XMVECTOR* _Scale, DirectX::XMVECTOR* _Rot, DirectX::XMVECTOR* _Pos, const _tagMAT& _World)
{
	DirectX::XMMatrixDecompose(_Scale, _Rot, _Pos, _World);
}

void _tagMAT::DivisionWorldMat(_tagVEC3* _Scale, _tagVEC4* _Rot, _tagVEC3* _Pos, const _tagMAT& _World)
{
	*_Scale = _World.GetScaleFactor().vec3;
	*_Rot = _World.GetAxisQuaternion();
	*_Pos = _World.Row[3].vec3;
}

const bool _tagMAT::operator==(const _tagMAT& _Other)
{
	for (size_t j = 0; j < 4; j++)
	{
		for (size_t i = 0; i < 4; i++)
		{
			if (M[j][i] != _Other.M[j][i])
				return false;
		}
	}

	return true;
}
void _tagMAT::operator=(const _tagMAT& _Other)
{
	memcpy_s(M, sizeof(M), _Other.M, sizeof(_Other.M));
}
void _tagMAT::operator=(const DirectX::XMFLOAT4X4& _Mat)
{
	XMF4x4 = _Mat;
}

const _tagMAT _tagMAT::operator*(const _tagMAT& _Other) const
{
	_tagMAT Result;

	for (size_t j = 0; j < 4; j++)
	{
		for (size_t i = 0; i < 4; i++)
		{
			Result.M[j][i] =
				  M[j][0] * _Other.M[0][i]
				+ M[j][1] * _Other.M[1][i]
				+ M[j][2] * _Other.M[2][i]
				+ M[j][3] * _Other.M[3][i];
		}
	}

	return Result;
}

const _tagMAT _tagMAT::operator*=(const _tagMAT& _Other)
{
	_tagMAT Temp;
	for (size_t j = 0; j < 4; j++)
	{
		for (size_t i = 0; i < 4; i++)
		{
			Temp.M[j][i] =
				  M[j][0] * _Other.M[0][i]
				+ M[j][1] * _Other.M[1][i]
				+ M[j][2] * _Other.M[2][i]
				+ M[j][3] * _Other.M[3][i];
		}
	}

	(*this) = Temp;

	return *this;
}

const Vec3 _tagMAT::Mul_Normal(const Vec3& _Vec) const 
{
	return Vec4(_Vec.x, _Vec.y, _Vec.z, 0.f) * (*this);
}
const Vec3 _tagMAT::Mul_Coord(const Vec3& _Vec)const 
{
	return Vec4(_Vec.x, _Vec.y, _Vec.z, 1.f) * (*this);
}
const Vec4 _tagMAT::MulVec4(const Vec4& _Vec) const
{
	_tagVEC4 Result;

	for (size_t i = 0; i < 4; i++)
		Result.pf[i] =	  _Vec.x *M[0][i]
						+ _Vec.y *M[1][i]
						+ _Vec.z * M[2][i]
						+ _Vec.w * M[3][i];
	return Result;
}

void _tagMAT::SetZeroMat()
{
	memset(M, 0, sizeof(_tagMAT));
}
void _tagMAT::SetIdentityMat() 
{
	// XMMARIX를 클래스내부에 그대로 받으면
	// 해당 클래스를 16바이트 정렬을 해야한다.
	// SIMD를 이용하여 연산한 이후 4바이트 정렬 매트릭스로 변환한 후
	// 결과를 저장한다.
	DirectX::XMStoreFloat4x4(&XMF4x4,DirectX::XMMatrixIdentity());

}
void _tagMAT::SetScalingMat(const float& _X, const float& _Y, const float& _Z)
{
	DirectX::XMStoreFloat4x4(&XMF4x4, DirectX::XMMatrixScaling(_X, _Y, _Z));
}
void _tagMAT::SetScalingMat(const _tagVEC4& _Scale)
{
	DirectX::XMStoreFloat4x4(&XMF4x4, DirectX::XMMatrixScaling(_Scale.x, _Scale.y, _Scale.z));
}
void _tagMAT::SetScalingMat(const _tagVEC3& _Scale)
{
	DirectX::XMStoreFloat4x4(&XMF4x4, DirectX::XMMatrixScaling(_Scale.x, _Scale.y, _Scale.z));
}
void _tagMAT::SetRotateXMat(const float& _Rad)
{
	DirectX::XMStoreFloat4x4(&XMF4x4, DirectX::XMMatrixRotationX(_Rad));
}
void _tagMAT::SetRotateYMat(const float& _Rad)
{
	DirectX::XMStoreFloat4x4(&XMF4x4, DirectX::XMMatrixRotationY(_Rad));
}
void _tagMAT::SetRotateZMat(const float& _Rad)
{
	DirectX::XMStoreFloat4x4(&XMF4x4, DirectX::XMMatrixRotationZ(_Rad));
}
void _tagMAT::SetRotateXMat_ForDegree(const float& _Deg)
{
	DirectX::XMStoreFloat4x4(&XMF4x4, DirectX::XMMatrixRotationX(_Deg*D2R));
}
void _tagMAT::SetRotateYMat_ForDegree(const float& _Deg)
{
	DirectX::XMStoreFloat4x4(&XMF4x4, DirectX::XMMatrixRotationY(_Deg*D2R));
}
void _tagMAT::SetRotateZMat_ForDegree(const float& _Deg)
{
	DirectX::XMStoreFloat4x4(&XMF4x4, DirectX::XMMatrixRotationZ(_Deg*D2R));
}
void _tagMAT::SetPosMat(const float& _X, const float& _Y, const float& _Z)
{
	DirectX::XMStoreFloat4x4(&XMF4x4, DirectX::XMMatrixTranslation(_X,_Y,_Z));
}
void _tagMAT::SetPosMat(const _tagVEC4& _Pos)
{
	DirectX::XMStoreFloat4x4(&XMF4x4, DirectX::XMMatrixTranslation(_Pos.x, _Pos.y, _Pos.z));
}
void _tagMAT::SetPosMat(const _tagVEC3& _Pos)
{
	DirectX::XMStoreFloat4x4(&XMF4x4, DirectX::XMMatrixTranslation(_Pos.x, _Pos.y, _Pos.z));
}
void _tagMAT::SetViewAtMat(const _tagVEC4& _EyePos, const _tagVEC4& _AtPos, const _tagVEC4& _UpDir)
{
	DirectX::XMStoreFloat4x4(&XMF4x4, DirectX::XMMatrixLookAtLH(_EyePos,_AtPos,_UpDir));
}

void _tagMAT::SetViewAtMat(const DirectX::XMVECTOR& _EyePos, const DirectX::XMVECTOR& _AtPos, const DirectX::XMVECTOR& _UpDir)
{
	DirectX::XMStoreFloat4x4(&XMF4x4, DirectX::XMMatrixLookAtLH(_EyePos, _AtPos, _UpDir));
}
void _tagMAT::SetViewToMat(const _tagVEC4& _EyePos, const _tagVEC4& _AtDir, const _tagVEC4& _UpDir)
{
	DirectX::XMStoreFloat4x4(&XMF4x4, DirectX::XMMatrixLookToLH(_EyePos, _AtDir, _UpDir));
}
void _tagMAT::SetViewToMat(const DirectX::XMVECTOR&& _Eye, const DirectX::XMVECTOR&& _AtDir, const DirectX::XMVECTOR&& _UpDir)
{
	DirectX::XMStoreFloat4x4(&XMF4x4, DirectX::XMMatrixLookToLH(_Eye, _AtDir, _UpDir));
}

void _tagMAT::SetOrthoProjMat(const float& _Width,const float& _Height,const float& _Near,const float& _Far)
{
	DirectX::XMStoreFloat4x4(&XMF4x4, DirectX::XMMatrixOrthographicLH(_Width, _Height, _Near, _Far));
}
void _tagMAT::SetFovPersProjMat(const float& _Fovy, const float& _Aspect, const float& _Near, const float& _Far)
{
	DirectX::XMStoreFloat4x4(&XMF4x4, DirectX::XMMatrixPerspectiveFovLH(_Fovy, _Aspect, _Near, _Far));
}
void _tagMAT::SetFovPersProjMat_Angle(const float& _FovAng, const float& _Aspect, const float& _Near, const float& _Far)
{
	DirectX::XMStoreFloat4x4(&XMF4x4, DirectX::XMMatrixPerspectiveFovLH(_FovAng * D2R, _Aspect, _Near, _Far));
}
void _tagMAT::SetPersProjMat(const float& _Width, const float& _Height, const float& _Near, const float& _Far)
{
	DirectX::XMStoreFloat4x4(&XMF4x4, DirectX::XMMatrixPerspectiveLH(_Width, _Height, _Near, _Far));
}

const _tagVEC4 _tagMAT::GetColmVec(const int& _Colm) const 
{
	switch (_Colm)
	{
	case 0:
		return _tagVEC4(_11, _21, _31, _41);
	case 1:
		return _tagVEC4(_12, _22, _32, _42);
	case 2:
		return _tagVEC4(_13, _23, _33, _43);
	case 3:
		return _tagVEC4(_14, _24, _34, _44);
	}
	
	return _tagVEC4(MAXINT32, MAXINT32, MAXINT32, MAXINT32);
}

//const _tagVEC4 _tagMAT::GetBasisVec4(const AXIS_TYPE& Axis) const
//{
//	switch (Axis)
//	{
//	case AXIS_TYPE::AXIS_X:
//		return _tagVEC4(_11, _21, _31, 0.f);
//	case AXIS_TYPE::AXIS_Y:
//		return _tagVEC4(_12, _22, _32, 0.f);
//	case AXIS_TYPE::AXIS_Z:
//		return _tagVEC4(_13, _23, _33, 0.f);
//	default :
//		return  _tagVEC4(MAXINT32, MAXINT32, MAXINT32, MAXINT32);
//	}
//
//	return _tagVEC4(MAXINT32, MAXINT32, MAXINT32, MAXINT32);
//}
//
//const _tagVEC3 _tagMAT::GetBasisVec(const AXIS_TYPE& Axis) const
//{
//	switch (Axis)
//	{
//	case AXIS_TYPE::AXIS_X:
//		return _tagVEC3(_11, _21, _31);
//	case AXIS_TYPE::AXIS_Y:
//		return _tagVEC3(_12, _22, _32);
//	case AXIS_TYPE::AXIS_Z:
//		return _tagVEC3(_13, _23, _33);
//	default:
//		return  _tagVEC3(MAXINT32, MAXINT32, MAXINT32);
//	}
//
//	return _tagVEC3(MAXINT32, MAXINT32, MAXINT32);
//}