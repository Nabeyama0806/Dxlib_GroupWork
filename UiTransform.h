#pragma once
#include "Vector2.h"

class UiTransform
{
public:
	Vector2 position;	//座標
	float angle;			//(角度)ラジアン
	float scale;			//拡縮

	UiTransform() :
		angle(0),
		scale(1){}
};