#pragma once
#include "Collider.h"
#include "Collision.h"
#include "DxLib.h"

class BoxCollider : public Collider
{
public:
	Vector3 m_size;

	//コンストラクタ
	BoxCollider(const Vector3& size, const Vector3& offset = Vector3()) : 
		Collider(offset),
		m_size(size) { }

	//衝突判定
	virtual bool CheckCollision(const Transform& transform1, const Transform& transform2, const Collider* collider) const override
	{
		//ダブルディスパッチによる形状識別
		return collider->CheckCollision(transform1, transform2, this);
	}
	
	//矩形と矩形の当たり判定
	virtual bool CheckCollision(const Transform& transform1, const Transform& transform2, const BoxCollider* collider) const override
	{
		return Collision::Check(transform1, this, transform2, collider);
	}

	//矩形と円形の当たり判定
	virtual bool CheckCollision(const Transform& transform1, const Transform& transform2, const CircleCollider* collider) const override
	{
		return Collision::Check(transform1, this, transform2, collider);
	}
};