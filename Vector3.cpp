#include "DxLib.h"
#include "Vector3.h"
#include "Vector2.h"
#include <cmath>

//Vector2へのキャスト
Vector3::operator Vector2() const
{
	return Vector2(x, y);
}

//VECTORへのキャスト
Vector3::operator VECTOR() const
{
	VECTOR v;
	v.x = x;
	v.y = y;
	v.z = z;

	return v;
}