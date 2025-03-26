#pragma once
#include "Vector3.h"
#include "Quaternion.h"

class Transform
{
public:
	Vector3 position;		//座標
	Quaternion rotation;	//角度(ラジアン)
	Vector3 scale;			//拡縮

	Transform() :
		scale(Vector3(1, 1, 1)) {}
};
