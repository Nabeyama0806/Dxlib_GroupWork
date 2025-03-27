#pragma once
#include "Vector3.h"
#include "Transform.h"

class BoxCollider;
class CircleCollider;

class Collider
{
public:

	//座標の調整
	Vector3 m_offset;

	//コンストラクタ
	Collider(const Vector3& offset) :
		m_offset(offset) { }

	//ダブルディスパッチによる形状識別で衝突判定を行う
	virtual bool CheckCollision(const Transform& transfrom1, const Transform& transform2, const Collider* collider) const = 0;
	virtual bool CheckCollision(const Transform& transfrom1, const Transform& transform2, const BoxCollider* collider) const = 0;
	virtual bool CheckCollision(const Transform& transfrom1, const Transform& transform2, const CircleCollider* collider) const = 0;
};