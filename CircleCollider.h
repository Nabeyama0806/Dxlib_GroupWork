#pragma once
#include "Collider.h"
#include "Collision.h"
#include "DxLib.h"

class CircleCollider : public Collider
{
public:
	int m_radius;

	//コンストラクタ
	CircleCollider(int radius, const Vector3& offset = Vector3()) :
		Collider(offset),
		m_radius(radius) {
	}

	//衝突判定
	virtual bool CheckCollision(const Transform& transform1, const Transform& transform2, const Collider* collider) const override
	{
		//ダブルディスパッチによる形状識別
		return collider->CheckCollision(transform1, transform2, this);
	}

	//矩形と円形の当たり判定
	virtual bool CheckCollision(const Transform& transform1, const Transform& transform2, const BoxCollider* collider) const override
	{
		return Collision::Check(transform1, collider, transform2, this);
	}

	//円形と円形の当たり判定
	virtual bool CheckCollision(const Transform& transform1, const Transform& transform2, const CircleCollider* collider) const override
	{
		return Collision::Check(transform1, this, transform2, collider);
	}
};