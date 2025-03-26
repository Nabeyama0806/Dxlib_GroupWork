#pragma once
#include "Vector3.h"
#include <cmath>

struct Quaternion
{
public:
	float x;
	float y;
	float z;
	float w;

	// コンストラクタ
	constexpr Quaternion() :
		x(0),
		y(0),
		z(0),
		w(1) {}
	constexpr Quaternion(float x, float y, float z, float w) :
		x(x),
		y(y),
		z(z),
		w(w) {}

	operator MATRIX() const;		// 行列への変換

	constexpr Vector3 operator*(const Vector3& v)
	{
		float x2 = x * 2.0f;
		float y2 = y * 2.0f;
		float z2 = z * 2.0f;
		float xx = x * x2;
		float yy = y * y2;
		float zz = z * z2;
		float xy = x * y2;
		float xz = x * z2;
		float yz = y * z2;
		float wx = w * x2;
		float wy = w * y2;
		float wz = w * z2;

		Vector3 res;
		res.x = (1.0f - (yy + zz)) * v.x + (xy - wz) * v.y + (xz + wy) * v.z;
		res.y = (xy + wz) * v.x + (1.0f - (xx + zz)) * v.y + (yz - wx) * v.z;
		res.z = (xz - wy) * v.x + (yz + wx) * v.y + (1.0f - (xx + yy)) * v.z;
		return res;
	}

	// 乗算
	constexpr Quaternion operator*(const Quaternion& other)
	{
		return Quaternion(
			w * other.x + x * other.w + y * other.z + -z * other.y,
			w * other.y + y * other.w + z * other.x + -x * other.z,
			w * other.z + z * other.w + x * other.y + -y * other.x,
			w * other.w + -x * other.x + -y * other.y + -z * other.z
		);
	}
	constexpr Quaternion& operator*=(const Quaternion& other)
	{
		*this = operator*(other);
		return *this;
	}

	Quaternion Conjugate() const;	

	// 長さの２乗を返す
	float LengthSquared() const;	

	// 長さを返す
	float Length() const;

	// 四元数を正規化する
	static Quaternion Normalized(const Quaternion& q)
	{
		float m = q.Length();
		return Quaternion(q.x / m, q.y / m, q.z / m, q.w / m);
	}
	Quaternion Normalized() const
	{
		return Normalized(*this);
	}
	void Normalize()
	{
		*this = Normalized();
	}

	// 内積を求める
	static float Dot(const Quaternion &q1, const Quaternion &q2);

	// 球面線形補間を行う(tは0～1)
	static Quaternion Slerp(const Quaternion &from, Quaternion to, float t);

	// 四元数の作成
	static Quaternion AngleAxis(float rad, Vector3 axis);			//任意軸回転の四元数の作成
	static Quaternion FromToRotation(const Vector3& from, const Vector3& to);			//向きを変更するための回転の四元数の作成(2つのベクトルが180度の場合は未定義)
	static Quaternion LookRotation(const Vector3& forward);

	// 四元数の設定
	void SetRotationAxis(float rad, const Vector3& axis);			//任意軸回転の四元数の設定
	void SetRotationDirection(Vector3 from, Vector3 to);		//向きを変更するための回転の四元数の設定(2つのベクトルが180度の場合は未定義)
};

// 共役四元数を返す
inline Quaternion Quaternion::Conjugate() const
{
	return Quaternion(-x, -y, -z, w);
}

// 四元数のスカラー倍
inline Quaternion operator*(const Quaternion &q, float f)
{
	return Quaternion(q.x * f, q.y * f, q.z * f, q.w * f);
}

// 長さの２乗を返す
inline float Quaternion::LengthSquared() const
{
	return x * x + y * y + z * z + w * w;
}

// 長さを返す
inline float Quaternion::Length() const
{
	return sqrt(LengthSquared());
}

// 2つの四元数の内積を求める
inline float Quaternion::Dot(const Quaternion &q1, const Quaternion &q2)
{
	return q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w;
}
