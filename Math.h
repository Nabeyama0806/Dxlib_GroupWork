#pragma once
#include "Vector2.h"
#include <cmath>

namespace Math
{
	//円周率
	static constexpr double Pi = 3.141592653589793;

	//デグリ（度数法）からラジアン（弧度法）に変換
	static constexpr double DegToRad(double deg)
	{
		return deg * Pi / 180.0;
	}

	//ラジアン（弧度法）からデグリ（度数法）に変換
	static constexpr double RadToDeg(double rad)
	{
		return rad / Pi * 180.0;
	}

	//角度から方向へ変換
	static constexpr Vector2 AngletoDirect(double deg)
	{
		double rad = DegToRad(deg);
		return Vector2(std::cos(rad), -std::sin(rad));
	}

	//与えられた最小値と最大値の範囲に値を制限
	static constexpr float Clamp(float value, float min, float max)
	{
		return value < min ? min :
			value > max ? max :
			value;		
	}
}