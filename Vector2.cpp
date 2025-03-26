#include "Vector2.h"
#include "Vector3.h"
#include <cmath>

//Vector3‚Ö‚ÌƒLƒƒƒXƒg
Vector2::operator Vector3() const
{
	return Vector3(x, y, 0);
}

//’·‚³‚ğæ“¾
float Vector2::Magnitude() const
{
	return std::sqrtf(SqrMagnitude());
}