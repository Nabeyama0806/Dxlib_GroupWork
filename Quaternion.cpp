#include "Vector3.h"
#include "Quaternion.h"

Quaternion::operator MATRIX() const
{
	MATRIX mat;
	float xx2 = 2 * x * x;
	float yy2 = 2 * y * y;
	float zz2 = 2 * z * z;
	float xy2 = 2 * x * y;
	float xz2 = 2 * x * z;
	float yz2 = 2 * y * z;
	float wx2 = 2 * w * x;
	float wy2 = 2 * w * y;
	float wz2 = 2 * w * z;

	// 行列への設定
	mat.m[0][0] = 1 - yy2 - zz2;
	mat.m[0][1] = xy2 + wz2;
	mat.m[0][2] = xz2 - wy2;
	mat.m[0][3] = 0;
	mat.m[1][0] = xy2 - wz2;
	mat.m[1][1] = 1 - xx2 - zz2;
	mat.m[1][2] = yz2 + wx2;
	mat.m[1][3] = 0;
	mat.m[2][0] = xz2 + wy2;
	mat.m[2][1] = yz2 - wx2;
	mat.m[2][2] = 1 - xx2 - yy2;
	mat.m[2][3] = 0;
	mat.m[3][0] = 0;
	mat.m[3][1] = 0;
	mat.m[3][2] = 0;
	mat.m[3][3] = 1;

	return mat;
}

//球面線形補間を行う(tは0～1)
Quaternion Quaternion::Slerp(const Quaternion &from, Quaternion to, float t)
{
	// 2つの回転の内積を求める
	float dot = Dot(from, to);

	// 内積が負の時、最短距離での補間を得るために片方を負にする
	if (dot < 0)
	{
		to = Quaternion(-to.x, -to.y, -to.z, -to.w);
		dot = -dot;
	}

	if (dot < 0.9995f)
	{
		float angle = std::acosf(dot);
		float s = 1.0f / std::sinf(angle);
		float w1 = std::sinf(angle * (1.0f - t)) * s;
		float w2 = std::sinf(angle * t) * s;

		return Quaternion(
			from.x * w1 + to.x * w2,
			from.y * w1 + to.y * w2,
			from.z * w1 + to.z * w2,
			from.w * w1 + to.w * w2
		).Normalized();
	}
	return to;
}

//任意軸回転の四元数の作成
Quaternion Quaternion::AngleAxis(float rad, Vector3 axis)
{
	// ベクトルの正規化
	axis.Normalize();

	// 定義に従って成分を設定する
	return Quaternion(
		axis.x * std::sinf(rad / 2),
		axis.y * std::sinf(rad / 2),
		axis.z * std::sinf(rad / 2),
		std::cosf(rad / 2)
	);
}

//向きを変更するための回転の四元数の作成(2つのベクトルが180度の場合は未定義)
Quaternion Quaternion::FromToRotation(const Vector3& from, const Vector3& to)
{
	// 外積を用いて、軸ベクトルを求める
	Vector3 axis = Vector3::Cross(from, to);

	// 外積が(0,0,0)の時は、無回転のクォータニオン(0,0,0,1)にする
	if (axis.IsZero())
	{
		if (Vector3::Dot(from, to) < 0)
		{
			// Y軸で真反対を向く
			return AngleAxis(Math::DegToRad(180.0f), Vector3(0, 1, 0));
		}
		else
		{
			return Quaternion();
		}
	}

	// 内積の定義から回転量を求める a・b = |a||b|cosθ なので
	float rad = std::acosf(Vector3::Dot(from, to) / (from.Magnitude() * to.Magnitude()));

	// 求めた軸と回転量でクォータニオンの生成
	return AngleAxis(rad, axis);
}

//任意軸回転の四元数の設定
void Quaternion::SetRotationAxis(float rad, const Vector3& axis)
{
	*this = AngleAxis(rad, axis);
}

//向きを変更するための回転の四元数の設定(2つのベクトルが180度の場合は未定義)
void Quaternion::SetRotationDirection(Vector3 from, Vector3 to)
{
	*this = FromToRotation(from, to);
}

Quaternion Quaternion::LookRotation(const Vector3& forward)
{
	// オブジェクトの正面からforwardに向ける回転を取得
	Quaternion lookRotation = FromToRotation(Vector3(0, 0, -1), forward);
	// 外積を用いてupwardsとforwardに垂直なベクトル(赤軸)を得る
	Vector3 xAxisHorizontal = Vector3::Cross(Vector3(0, 1, 0), forward);
	// 回転後のy軸(緑軸)を求める
	Vector3 yAxisAfterRotate = Vector3::Cross(forward, xAxisHorizontal);

	// Look後のy軸(緑) から 回転後のy軸(緑) へ修正する回転を求める
	Vector3 yAxisBeforeModify = lookRotation * Vector3(0, 1, 0);
	Quaternion modifyRotation = FromToRotation(yAxisBeforeModify, yAxisAfterRotate);

	// 回転を合成して返す
	return modifyRotation * lookRotation;
}
