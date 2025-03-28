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


#ifdef _DEBUG
	// 描画
	virtual void Draw(const Transform& transform) const override
	{
		// 上の四角
		DrawLine3D(
			VGet(transform.position.x - m_size.x / 2, transform.position.y + m_size.y / 2, transform.position.z - m_size.z / 2),
			VGet(transform.position.x + m_size.x / 2, transform.position.y + m_size.y / 2, transform.position.z - m_size.z / 2),
			GetColor(255, 255, 255));

		DrawLine3D(
			VGet(transform.position.x + m_size.x / 2, transform.position.y + m_size.y / 2, transform.position.z - m_size.z / 2),
			VGet(transform.position.x + m_size.x / 2, transform.position.y + m_size.y / 2, transform.position.z + m_size.z / 2),
			GetColor(255, 255, 255));

		DrawLine3D(
			VGet(transform.position.x + m_size.x / 2, transform.position.y + m_size.y / 2, transform.position.z + m_size.z / 2),
			VGet(transform.position.x - m_size.x / 2, transform.position.y + m_size.y / 2, transform.position.z + m_size.z / 2),
			GetColor(255, 255, 255));

		DrawLine3D(
			VGet(transform.position.x - m_size.x / 2, transform.position.y + m_size.y / 2, transform.position.z + m_size.z / 2),
			VGet(transform.position.x - m_size.x / 2, transform.position.y + m_size.y / 2, transform.position.z - m_size.z / 2),
			GetColor(255, 255, 255));

		// 縦の棒
		DrawLine3D(
			VGet(transform.position.x - m_size.x / 2, transform.position.y + m_size.y / 2, transform.position.z - m_size.z / 2),
			VGet(transform.position.x - m_size.x / 2, transform.position.y - m_size.y / 2, transform.position.z - m_size.z / 2),
			GetColor(255, 255, 255));

		DrawLine3D(
			VGet(transform.position.x + m_size.x / 2, transform.position.y + m_size.y / 2, transform.position.z - m_size.z / 2),
			VGet(transform.position.x + m_size.x / 2, transform.position.y - m_size.y / 2, transform.position.z - m_size.z / 2),
			GetColor(255, 255, 255));

		DrawLine3D(
			VGet(transform.position.x - m_size.x / 2, transform.position.y + m_size.y / 2, transform.position.z + m_size.z / 2),
			VGet(transform.position.x - m_size.x / 2, transform.position.y - m_size.y / 2, transform.position.z + m_size.z / 2),
			GetColor(255, 255, 255));

		DrawLine3D(
			VGet(transform.position.x + m_size.x / 2, transform.position.y + m_size.y / 2, transform.position.z + m_size.z / 2),
			VGet(transform.position.x + m_size.x / 2, transform.position.y - m_size.y / 2, transform.position.z + m_size.z / 2),
			GetColor(255, 255, 255));

		// 下の四角
		DrawLine3D(
			VGet(transform.position.x - m_size.x / 2, transform.position.y - m_size.y / 2, transform.position.z - m_size.z / 2),
			VGet(transform.position.x + m_size.x / 2, transform.position.y - m_size.y / 2, transform.position.z - m_size.z / 2),
			GetColor(255, 255, 255));

		DrawLine3D(
			VGet(transform.position.x + m_size.x / 2, transform.position.y - m_size.y / 2, transform.position.z - m_size.z / 2),
			VGet(transform.position.x + m_size.x / 2, transform.position.y - m_size.y / 2, transform.position.z + m_size.z / 2),
			GetColor(255, 255, 255));

		DrawLine3D(
			VGet(transform.position.x + m_size.x / 2, transform.position.y - m_size.y / 2, transform.position.z + m_size.z / 2),
			VGet(transform.position.x - m_size.x / 2, transform.position.y - m_size.y / 2, transform.position.z + m_size.z / 2),
			GetColor(255, 255, 255));

		DrawLine3D(
			VGet(transform.position.x - m_size.x / 2, transform.position.y - m_size.y / 2, transform.position.z + m_size.z / 2),
			VGet(transform.position.x - m_size.x / 2, transform.position.y - m_size.y / 2, transform.position.z - m_size.z / 2),
			GetColor(255, 255, 255));

	}
#endif
};