#pragma once
#include "DxLib.h"
#include "Math.h"

//クラスの前方宣言
class Vector2;

class Vector3
{
public:
	float x;
	float y;
	float z;

	//コンストラクタ
	constexpr Vector3():
		x(0),
		y(0),
		z(0) {}

	template<typename T1, typename T2, typename T3>
	constexpr Vector3(T1 x, T2 y, T3 z) :
		x(static_cast<float>(x)),
		y(static_cast<float>(y)),
		z(static_cast<float>(z)){}

	//Vector2へのキャスト
	operator Vector2() const;

	//VECTORへのキャスト
	operator  VECTOR() const;
	constexpr friend Vector3 operator == (const Vector3& v, const VECTOR& other)
	{
		return Vector3(other.x, other.y, other.z);
	}

	//加算
	constexpr Vector3 operator + (const Vector3& other) const	//Vector3 + Vector3
	{
		return Vector3(x + other.x, y + other.y, z + other.z);
	}
	constexpr Vector3& operator += (const Vector3& other)		//Vector3 += Vector3
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	//減算
	constexpr Vector3 operator - (const Vector3& other) const	//Vector3 - Vector3
	{
		return Vector3(x - other.x, y - other.y, z - other.z);
	}
	constexpr Vector3& operator -= (const Vector3& other)		//Vector3 -= Vector3
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	//乗算
	constexpr Vector3 operator * (const float& scalar) const //Vector3 * scalar
	{
		return Vector3(x * scalar, y * scalar, z * scalar);
	}
	constexpr friend Vector3 operator * (const float& scalar, const Vector3& v)//scalar * Vector3
	{
		return v * scalar;
	}
	constexpr Vector3& operator *= (const float& scalar)		//Vector3 *= scalar
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	//除算
	const Vector3 operator / (const float& scalar) const	//Vector3 / scalar
	{
		return Vector3(x / scalar, y / scalar, z / scalar);
	}
	constexpr Vector3& operator /= (const float& scalar)	//Vector3 /= scalar
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}

	//負(単項)
	constexpr Vector3 operator - () const	//-Vector3
	{
		return Vector3(-x, -y, -z);
	}

	//等価
	constexpr bool operator == (const Vector3& other) const	//Vector3 == Vector3
	{
		return (x == other.x) && (y == other.y) && (z == other.z);
	}

	//非等価
	constexpr bool operator != (const Vector3& other) const	//Vector3 != Vector3
	{
		return !(*this == other);
	}

	//ゼロ判定(不完全)
	static constexpr bool IsZero(const Vector3& v)
	{
		return (v.x == 0) && (v.y == 0) && (v.z == 0);
	}
	constexpr bool IsZero() const
	{
		return IsZero(*this);
	}

	//長さの2乗を取得
	static constexpr float SqrMagnitude(const Vector3& v)
	{
		return v.x * v.x + v.y * v.y + v.z * v.z;
	}
	constexpr float SqrMagnitude() const
	{
		return SqrMagnitude(*this);
	}

	//2つのベクトルの内積を取得
	static constexpr float Dot(const Vector3& lhs, const Vector3& rhs)
	{
		return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
	}
	constexpr float Dot(const Vector3& v) const
	{
		return Dot(*this, v);
	}

	//2つのベクトルの外積を取得
	static constexpr Vector3 Cross(const Vector3& lhs, const Vector3& rhs)
	{
		return Vector3(lhs.y * rhs.z - lhs.z * rhs.y, lhs.z * rhs.x - lhs.x * rhs.z, lhs.x * rhs.y - lhs.y * rhs.x);
	}
	constexpr Vector3 Cross(const Vector3& v)
	{
		return Cross(*this, v);
	}

	//2点間の角度を取得
	static float Angles(Vector3 from, Vector3 to)
	{
		float num = static_cast<float>(std::sqrt(from.SqrMagnitude() * to.SqrMagnitude()));
		if (num < 1E-15f)
		{
			return 0.0f;
		}

		float num2 = Math::Clamp(Dot(from, to) / num, -1.0f, 1.0f);
		return (float)std::acos(num2);
	}

	//長さを取得
	static float Magnitude(const Vector3& v)
	{
		return std::sqrtf(SqrMagnitude(v));
	}
	float Magnitude() const
	{
		return Magnitude(*this);
	}

	//スケールの取得
	static Vector3 Scale(const Vector3& v1, const Vector3& v2)
	{
		return Vector3(v1.x * v2.x, v1.y * v2.y, v1.z * v2.z);
	}
	Vector3 Scale(const Vector3& v) const
	{
		return Scale(*this, v);
	}

	//正規化
	static Vector3 Normalize(const Vector3& v)
	{
		Vector3 tmp = v;

		float m = Magnitude(tmp);
		tmp.x /= m;
		tmp.y /= m;
		tmp.z /= m;

		return tmp;
	}
	Vector3& Normalize()
	{
		*this = Normalize(*this);
		return *this;
	}

	//正規化
	Vector3 Normalized() const
	{
		return Normalize(*this);
	}
};