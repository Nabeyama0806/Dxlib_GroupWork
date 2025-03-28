#pragma once
#include "ModelActor.h"

class Bottle : public ModelActor
{
public:
	//ボトルの属性
	enum class  Type
	{
		Fire,		//炎
		Water,		//水
		Thunder,	//雷
		Wind,		//風

		Length
	};

	Type m_type;	//自身の属性

private:
	static constexpr float Gravity = 1.2f;	//重力

protected:
	void Throw();

public:
	//コンストラクタ
	Bottle(Vector3& position);
};