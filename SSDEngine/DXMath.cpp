#include "DXMath.h"
#include "DXMacro.h"
#include <algorithm>

const float DXMath::FPI = 3.14159265358979323846f;
const float DXMath::DPI = 3.14159265358979323846f * 2.0f;
const float DXMath::DtR = DXMath::FPI / 180.0f;
const float DXMath::RtD = 180.0f / DXMath::FPI;
const float DXMath::Epsilon = 0.0000002f;



////////////////////////////////////////////				Gaussian Function				///////////////////////////


const float Gaussian::operator()(const float _X) 
{
	float Result = exp(-(_X * _X) / (2.f * Variance));
	Result /= (Sigma * sqrtf(2.f *  DXMath::FPI));
	return DXMath::ExceptSmallNumber(Result * Max);
}
const float Gaussian::operator()(const float _X, const float _Y) 
{
	float Result = exp(-((_X * _X )+( _Y * _Y)) / (2.f * Variance));
	Result /= (Sigma * sqrtf(2.f *  DXMath::FPI));
	return  DXMath::ExceptSmallNumber( Result * Max);
}


















///////////////////////////////////////////////				DXMath						/////////////////////////////////
const float DXMath::ExceptSmallNumber(const float& _Value)
{
	if (fabsf(_Value) <= Epsilon)
		return 0.f;

	return _Value;
}
const float DXMath::Cos(const float& _Radian) 
{
	return ExceptSmallNumber(cosf(_Radian));
}
const float DXMath::CosA(const float& _Degree) 
{
	return ExceptSmallNumber(cosf(_Degree*D2R));
}
const float DXMath::Sin(const float& _Radian) 
{
	return ExceptSmallNumber(sinf(_Radian));
}
const float DXMath::SinA(const float& _Degree) 
{
	return ExceptSmallNumber(sinf(_Degree*D2R));
}
const float DXMath::RadToDeg(const float _Radian)
{
	return _Radian*RtD;
}
const float DXMath::DegToRad(const float _Degree)
{
	return _Degree*DtR;
}
const Vec2 DXMath::GetDir(const float& _Radian)
{
	float x = ExceptSmallNumber(cosf(_Radian));
	float y = ExceptSmallNumber(sinf(_Radian));

	return Vec2(x, y);
}
//const float DXMath::ABS(const float _Value)
//{
//	if (_Value <= 0)
//	{
//		return _Value * -1;
//	}
//	return _Value;
//}
//const int DXMath::ABS(const int _Value)
//{
//	if (_Value <= 0)
//	{
//		return _Value * -1;
//	}
//	return _Value;
//}
const float DXMath::Base(const Vec3& Start, const Vec3& End)
{
	return End.x - Start.x;
}
const float DXMath::Base(const Vec2& Start, const Vec2& End)
{
	return End.x - Start.x;
}
const float DXMath::Hight(const Vec3& Start, const Vec3& End)
{
	return End.y - Start.y;
}
const float DXMath::Hight(const Vec2& Start, const Vec2& End)
{
	return End.y - Start.y;
}

const float DXMath::Square(float _Value)
{
	return (_Value*_Value);
}
const float DXMath::Length(const Vec2& _Start, const Vec2& _End)
{
	float x = _End.x - _Start.x;
	float y = _End.y - _Start.y;

	return sqrt((x*x) + (y*y));
}
const float DXMath::Length(const Vec3& _Start, const Vec3& _End)
{
	float x = _End.x - _Start.x;
	float y = _End.y - _Start.y;
	float z = _End.z - _Start.z;

	return sqrt((x*x) + (y*y) + (z*z));
}

// 벡터의 길이
const float DXMath::Length(const Vec2& _Point)
{
	return sqrt((_Point.x*_Point.x) + (_Point.y*_Point.y));
}
const float DXMath::Length(const Vec3& _Point)
{
	return sqrt((_Point.x*_Point.x) + (_Point.y*_Point.y) + (_Point.z*_Point.z));
}

const float DXMath::DirToRadian(const Vec2& _Start, const Vec2& _End)
{
	float tempBase = _End.x - _Start.x;
	float tempGradian = Length(_End, _Start);

	float tempRad = acosf(tempBase / tempGradian);

	if (0 > (_End.y - _Start.y))
	{
		tempRad = FPI + tempRad;
	}

	return tempRad;
}
const float DXMath::DirToRadian(const Vec2& _Point)
{
	float tempGradian = Length(_Point);
	float tempRad = acosf(_Point.x / tempGradian);

	if (_Point.y < 0)
	{
		tempRad = FPI + tempRad;
	}

	return tempRad;
}
const float DXMath::DirToDegree(const Vec2& _Start, const Vec2 _End)
{
	return RtD * DirToRadian(_Start, _End);
}
const float DXMath::DirToDegree(const Vec2& _Point)
{
	return RtD * DirToRadian(_Point);
}
const Vec2 DXMath::AbsPoint(const Vec2& _Point)
{
	float X = _Point.x;
	float Y = _Point.y;

	if (X <= 0)
	{
		X *= -1.0f;
	}
	if (Y <= 0)
	{
		Y *= -1.0f;
	}

	return Vec2(X, Y);
}
const Vec3 DXMath::AbsPoint(const Vec3& _Point)
{
	float X = _Point.x;
	float Y = _Point.y;
	float Z = _Point.z;

	if (X <= 0)
	{
		X *= -1.0f;
	}
	if (Y <= 0)
	{
		Y *= -1.0f;
	}
	if (Z <= 0)
	{
		Z *= -1.0f;
	}
	return Vec3(X, Y, Z);
}


const Vec2 DXMath::Vec2Lerp(Vec2* _Start,Vec2* _End, const float& _Weight)
{
	if (0.0f >= _Weight) 
	{
		return *_Start;
	}

	if (1.0f <= _Weight) 
	{
		return *_End;
	}

	Vec2 LerpVec;

	LerpVec = (*_Start) * (1.f - _Weight) + (*_End) * _Weight;
	return LerpVec;
}
const Vec3 DXMath::Vec3Lerp(Vec3* _Start,Vec3* _End, const float& _Weight)
{
	if (0.0f >= _Weight)
	{
		return *_Start;
	}

	if (1.0f <= _Weight)
	{
		return *_End;
	}

	Vec3 LerpVec;
	LerpVec = (*_Start) * (1.f - _Weight) + (*_End) * _Weight;
	return LerpVec;
}
const Vec4 DXMath::Vec4Lerp(Vec4* _Start, Vec4* _End, const float& _Weight)
{
	if (0.0f >= _Weight)
	{
		return *_Start;
	}

	if (1.0f <= _Weight)
	{
		return *_End;
	}

	Vec4 LerpVec;
	LerpVec = (*_Start) * (1.f - _Weight) + (*_End) * _Weight;

	return LerpVec;
}
const float DXMath::FloatLerp(const float& _Start, const float& _End, const float& _Weight)
{
	if (0.f >= _Weight)
		return _Start;

	if (1.f <= _Weight)
		return _End;

	return _Start * (1.f - _Weight) + _End * _Weight;
}

const Vec2 DXMath::Vec2Lerp(const Vec2& _Start, const Vec2& _End, const float& _Weight)
{
	if (0.0f >= _Weight)
		return _Start;

	if (1.0f <= _Weight)
		return _End;

	Vec2 LerpVec;
	LerpVec = (_Start * (1.f - _Weight)) + (_End * _Weight);

	return LerpVec;
}
const Vec3 DXMath::Vec3Lerp(const Vec3& _Start, const Vec3& _End, const float& _Weight)
{
	if (0.0f >= _Weight)
		return _Start;

	if (1.0f <= _Weight)
		return _End;

	Vec3 LerpVec;
	LerpVec = (_Start * (1.f - _Weight)) + (_End * _Weight);

	return LerpVec;
}
const Vec4 DXMath::Vec4Lerp(const Vec4& _Start, const Vec4& _End, const float& _Weight)
{
	if (0.0f >= _Weight)
		return _Start;

	if (1.0f <= _Weight)
		return _End;
	
	Vec4 LerpVec;
	LerpVec = (_Start * (1.f - _Weight)) + (_End * _Weight);

	return LerpVec;
}
const Vec2 DXMath::Vec2Slerp(const Vec2& _Start, const Vec2& _End, const float& _Weight)
{
	float T = DXMath::Clamp(_Weight, 0.f, 1.f);

	Vec2 Start = _Start;
	Vec2 End = _End;
	Vec2 SlerpVec;

	float Seta = Vector2Angle(_Start, _End);
	float SinValue = sinf(Seta);

	if (Seta == 0.f)
		return Start;


	// 두 벡터의 각이 180도 일때의 예외처리 수행 X
	SlerpVec = Start * (sinf(Seta*(1.f - T)) / SinValue) + _End * (sinf(Seta * T) / SinValue);
	SlerpVec.Normalize();

	if (isnan(SlerpVec.x) || isnan(SlerpVec.y))
		TASSERT(true);

	return SlerpVec;
}

const Vec3 DXMath::Vec3Slerp(const Vec3& _Start, const Vec3& _End, const float& _Weight)
{
	float T = DXMath::Clamp(_Weight, 0.f, 1.f);

	Vec3 Start = _Start;
	Vec3 End = _End;
	Vec3 SlerpVec;

	float Seta = Vector3Angle(_Start, _End);
	float SinValue = sinf(Seta);

	if (Seta == 0.f)
		return Start;


	// 두 벡터의 각이 180도 일때의 예외처리 수행 X
	SlerpVec = Start * (sinf(Seta*(1.f - T)) / SinValue) + _End * (sinf(Seta * T) / SinValue);
	SlerpVec.Normalize();

	if (isnan(SlerpVec.x) || isnan(SlerpVec.y) || isnan(SlerpVec.z))
		TASSERT(true);

	return SlerpVec;
}
const float DXMath::Vector2Dot(const Vec2& _Left, const Vec2& _Right)
{	
	//return _Left.x*_Right.x + _Left.y*_Right.y;
	return DirectX::XMVector2Dot(_Left, _Right).m128_f32[0];
}
const float DXMath::Vector3Dot(const Vec3& _Left, const Vec3& _Right)
{
	//return _Left.x*_Right.x + _Left.y*_Right.y + _Left.z*_Right.z;
	return DirectX::XMVector3Dot(_Left, _Right).m128_f32[0];
}
const float DXMath::Vector4Dot(const Vec4& _Left, const Vec4& _Right)
{
	//return _Left.x*_Right.x + _Left.y*_Right.y + _Left.z*_Right.z + _Left.w*_Right.w;
	return DirectX::XMVector4Dot(_Left, _Right).m128_f32[0];
}
const float DXMath::Vector2Cross(const Vec2& _Left, const Vec2& _Right)
{
	return ExceptSmallNumber( DirectX::XMVector2Cross(_Left, _Right).m128_f32[0]);
}
const Vec3 DXMath::Vector3Cross(const Vec3& _Left, const Vec3& _Right)
{
	Vec3 ReturnVec = Vec3(DirectX::XMVector3Cross(_Left, _Right));

	return EpsilonCheck(ReturnVec);
}
const Vec4 DXMath::Vector4Cross(const Vec4& _Left, const Vec4& _Right)
{
	Vec4 ReturnVec; 
	ReturnVec.vec3 = Vec3(DirectX::XMVector3Cross(_Left, _Right));
	ReturnVec.w = 0.f;

	return EpsilonCheck(ReturnVec);
}
const float DXMath::Vector2Angle(const Vec2& _Left, const Vec2& _Right)
{
	Vec2 Left = _Left.GetNormalizeVec();
	Vec2 Right = _Right.GetNormalizeVec();

	//float Dot = _Left.x*_Right.x + _Left.y*_Right.y;
	float Dot = Vector2Dot(_Left,_Right);
	return acosf(DXMath::Clamp( Dot,-1.f,1.f));
}
const float DXMath::Vector3Angle(const Vec3& _Left, const Vec3& _Right)
{
	Vec3 Left = _Left.GetNormalizeVec();
	Vec3 Right = _Right.GetNormalizeVec();

	//float Dot = _Left.x*_Right.x + _Left.y*_Right.y + _Left.z*_Right.z;
	float Dot = Vector3Dot(_Left, _Right);
	return acosf(Clamp(Dot,-1.f,1.f));
}

const float DXMath::Vector2Angle(const Vec2& _Vec) 
{
	Vec2 Vec = _Vec.GetNormalizeVec();

	float Seta = acosf(DXMath::Clamp(Vec.x,-1.f,1.f));

	if (_Vec.y >= 0.f)
		return Seta;

	return DXMath::DPI - Seta;

}


const Vec2 DXMath::EpsilonCheck(const Vec2& _Vec2)
{
	Vec2 Result =_Vec2;
	Result.x = ExceptSmallNumber(Result.x);
	Result.y = ExceptSmallNumber(Result.y);

	return Result;
}
const Vec3 DXMath::EpsilonCheck(const Vec3& _Vec3)
{
	Vec3 Result = _Vec3;
	Result.x = ExceptSmallNumber(Result.x);
	Result.y = ExceptSmallNumber(Result.y);
	Result.z = ExceptSmallNumber(Result.z);
	return Result;
}

const Vec4 DXMath::EpsilonCheck(const Vec4& _Vec4)
{
	Vec4 Result = _Vec4;
	Result.x = ExceptSmallNumber(Result.x);
	Result.y = ExceptSmallNumber(Result.y);
	Result.z = ExceptSmallNumber(Result.z);
	Result.w = ExceptSmallNumber(Result.w);
	return Result;
}

const float DXMath::AngleFromXY(float _X, float _Y)
{
	float theta = 0.0f;

	// Quadrant I or IV
	if (_X >= 0.0f)
	{
		// If x = 0, then atanf(y/x) = +pi/2 if y > 0
		//                atanf(y/x) = -pi/2 if y < 0
		theta = atanf(_Y / _X); // in [-pi/2, +pi/2]

		if (theta < 0.0f)
			theta += 2.0f * FPI; // in [0, 2*pi).
	}

	// Quadrant II or III
	else
		theta = atanf(_Y / _X) + FPI; // in [0, 2*pi).

	return theta;
}
//const float DXMath::Clamp(const float& _Input, const float& _Min, const float& _Max)
//{
//	std::clamp(_Input, _Min, _Max);
//	float Min = _Min;
//	float Max = _Max;
//
//	if (Min >= Max)
//	{
//		float Temp = Min;
//		Min = Max;
//		Max = Temp;
//	}
//
//	if (Min > _Input)
//		return Min;
//
//	if (Max < _Input)
//		return Max;
//	
//	return _Input;
//}
//////////////////////////////////////				Collision Function					/////////////////////////
const bool CollisionFunc::PointToPoint(const VEC4& _Left, const VEC4& _Right)
{
	if(_Left.Pos == _Right.Pos)
		return true;

	return false;
}

const bool CollisionFunc::PointToRect(const VEC4& _Left, const VEC4& _Right) 
{
	Vec2 Pos = _Left.Pos - _Right.Pos;
	Pos = DXMath::AbsPoint(Pos);

	if (_Right.HXSize() < Pos.x)
		return false;

	if (_Right.HYSize() < Pos.y)
		return false;
	
	return true;
}

const bool CollisionFunc::PointToCircle(const VEC4& _Left, const VEC4& _Right)
{
	float Dis = DXMath::Length(_Left.Pos, _Right.Pos);

	if (Dis <= _Right.Radius())
		return true;

	return false;
}

const bool CollisionFunc::RectToRect(const VEC4& _Left, const VEC4& _Right) 
{
	if (_Left.Left() > _Right.Right())
		return false;

	if (_Left.Right() < _Right.Left())
		return false;

	if (_Left.Top() < _Right.Bottom())
		return false;

	if (_Left.Bottom() > _Right.Top())
		return false;

	return true;
}
const bool CollisionFunc::RectToCircle(const VEC4& _Left, const VEC4& _Right) 
{
	Vec2 CirclePos = _Right.Pos - _Left.Pos;
	CirclePos = DXMath::AbsPoint(CirclePos);

	Vec2 WRect = Vec2(_Left.HXSize() + _Right.Radius(), _Left.HYSize());
	Vec2 HRect = Vec2(_Left.HXSize(), _Left.HYSize() + _Right.Radius());
	Vec2 RightTop = _Left.RightTop() - _Left.Pos;

	
	if (WRect.x > CirclePos.x && WRect.y > CirclePos.y)
		return true;

	if (HRect.x > CirclePos.x && HRect.y > CirclePos.y)
		return true;

	float Dis = DXMath::Length(CirclePos, RightTop);
	
	if (Dis < _Right.Radius())
		return true;
	
	return false;
}

const bool CollisionFunc::CircleToCircle(const VEC4& _Left, const VEC4& _Right) 
{
	float Dis = DXMath::Length(_Left.Pos, _Right.Pos);

	if (Dis <= _Left.Radius() + _Right.Radius())
		return true;

	return false;
}
const bool CollisionFunc::Ray2DToLinesegment2D(const Vec2& _RayPos, const Vec2& _RayDir, const Vec2& _SegStart, const Vec2& _SegEnd)
{
	// t : 반직선의 벡터 방정식의 매개변수 , s : 선분의 벡터방정식의 매개변수
	float t, s;
	Vec2 LineDir = (_SegEnd - _SegStart).GetNormalizeVec();

	// 두 직선의 방향 벡터가 평행(사잇각이 0 or 180 도) 인 경우
	if (0 == DXMath::Vector2Cross(LineDir, _RayDir))
	{
		if (_RayDir.y == 0.f)
		{
			if (_RayPos.y == _SegStart.y)
				return true;
			
			return false;
		}
		if (_RayDir.x == 0.f)
		{
			if (_RayPos.x == _SegStart.x)
				return true;

			return false;
		}

		float c1, c2;
		c1 = (_SegStart.x - _RayPos.x) / _RayDir.x;
		c2 = (_SegStart.y - _RayPos.y) / _RayDir.y;
		
		if (c1 == c2)
			return true;

		return false;
	}

	s = (DXMath::Vector2Cross(_RayDir, _SegStart) - DXMath::Vector2Cross(_RayDir, _RayPos)) / DXMath::Vector2Cross(LineDir, _RayDir);

	if (_RayDir.x != 0.f)
		t = (_SegStart.x - _RayPos.x + s * LineDir.x) / _RayDir.x;
	else if (_RayDir.y != 0.f)
		t = (_SegStart.y - _RayPos.y + s * LineDir.y) / _RayDir.y;
	else
		t = -1.f;

	if (t >= 0.f && (s >= 0.f && s <= DXMath::Length(_SegEnd - _SegStart)))
		return true;

	return false;
}
const bool CollisionFunc::Ray2DToLinesegment2D(const Vec2& _RayPos, const Vec2& _RayDir, const Vec4& _LineSeg)
{
	return Ray2DToLinesegment2D(_RayPos, _RayDir, _LineSeg.LineStart, _LineSeg.LineEnd);
}
const bool CollisionFunc::Linesegment2DToLinesegment2D(const Vec2& _LeftStart, const Vec2& _LeftEnd, const Vec2& _RightStart, const Vec2& _RightEnd) 
{
	float t, s;
	Vec2 LeftDir = (_LeftEnd - _LeftStart).GetNormalizeVec();
	Vec2 RightDir = (_RightEnd - _RightStart).GetNormalizeVec();

	if (0 == DXMath::Vector2Cross(LeftDir, RightDir))
	{
		if (LeftDir.y == 0.f)
		{
			if (_LeftStart.y == _RightStart.y)
				return true;

			return false;
		}
		if (LeftDir.x == 0.f)
		{
			if (_LeftStart.x == _RightStart.x)
				return true;

			return false;
		}

		float c1, c2;
		c1 = (_RightStart.x - _LeftStart.x) / LeftDir.x;
		c2 = (_RightStart.y - _LeftStart.y) / LeftDir.y;

		if (c1 == c2)
			return true;

		return false;
	}


	s = (DXMath::Vector2Cross(LeftDir, _LeftStart) - DXMath::Vector2Cross(LeftDir, _RightStart)) / DXMath::Vector2Cross(LeftDir, RightDir);

	if (LeftDir.x != 0.f)
		t = (_RightStart.x - _LeftStart.x + s * RightDir.x) / LeftDir.x;
	else if (LeftDir.y != 0.f)
		t = (_RightStart.y - _LeftStart.y + s * RightDir.y) / LeftDir.y;
	else
		t = -1.f;

	if ((t >= 0.f && t <= DXMath::Length(_LeftEnd - _LeftStart)) && (s >= 0.f && s <= DXMath::Length(_RightEnd - _RightStart)))
		return true;

	return false;
}
const bool CollisionFunc::Linesegment2DToLinesegment2D(const Vec4& _LeftSeg, const Vec4& _RightSeg) 
{
	return Linesegment2DToLinesegment2D(_LeftSeg.LineStart, _LeftSeg.LineEnd, _RightSeg.LineStart, _RightSeg.LineEnd);
}

const bool CollisionFunc::TriangleToTriangle(const Vec3& _LTri0, const Vec3& _LTri1, const Vec3& _LTri2
	, const Vec3& _RTri0, const Vec3& _RTri1, const Vec3& _RTri2) 
{
	return DirectX::TriangleTests::Intersects(_LTri0, _LTri1, _LTri2, _RTri0, _RTri1, _RTri2);
}

const bool CollisionFunc::TriangleToTriangle(const Vec2& _LTri0, const Vec2& _LTri1, const Vec2& _LTri2
	, const Vec2& _RTri0, const Vec2& _RTri1, const Vec2& _RTri2)
{
	return DirectX::TriangleTests::Intersects(_LTri0, _LTri1, _LTri2, _RTri0, _RTri1, _RTri2);
}

const bool CollisionFunc::TriangleToPoint(const Vec2& _LTri0, const Vec2& _LTri1, const Vec2& _LTri2, const Vec2& _Point)
{
	Vec2 TriDir1 = (_LTri1 - _LTri0).GetNormalizeVec();
	Vec2 TriDir2 = (_LTri2 - _LTri0).GetNormalizeVec();
	Vec2 Dir = (_Point - _LTri0).GetNormalizeVec();

	float A, B;
	A = DXMath::Vector2Dot(Dir, TriDir1);
	B = DXMath::Vector2Dot(Dir, TriDir2);

	if (A >= 0.f && B >= 0.f && A + B <= 1.f)
		return true;

	return false;
}

const bool CollisionFunc::RotateOrthoRectToPoint(const Vec2& _Origin, const Vec2& _Axis0, const Vec2& _Axis1, const Vec2& _Point)
{
	Vec2 Dir = _Point - _Origin;


	float Axis0Len, Axis1Len, Len0, Len1;
	Axis0Len = _Axis0.Length();
	Axis1Len = _Axis1.Length();
	Len0 = DXMath::Vector2Dot(Dir, _Axis0.GetNormalizeVec());
	Len1 = DXMath::Vector2Dot(Dir, _Axis1.GetNormalizeVec());

	if (Len0 < 0.f || Len0 > Axis0Len)
		return  false;

	if (Len1 < 0.f || Len1 > Axis1Len)
		return  false;

	return true;
}