#include "Lerp.h"
#define _USE_MATH_DEFINES	//M_PIを使うためのdefine
#include <cmath>

//メンバー関数を関数ポインタにセット
float(*Lerp::m_funcTable[])(float) =
{
	//enum Modelと同じ順番
	&Lerp::Linear,
	&Lerp::EaseInOutQuad,
};

float Lerp::Linear(float t)
{
	return t;
}

float Lerp::EaseInOutQuad(float t)
{
	return t < 0.5f ?
		2 * t * t :
		1 - std::powf(-2 * t + 2, 2.0f) / 2.0f;
}