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
		DrawBoxLine(transform);
	}
#endif

private:

	void DrawBoxLine(const Transform& transform) const
	{
		for (int i = -1; i <= 1; ++i)
		{
			for (int j = -1; j <= 1; ++j)
			{
				if (i == 0 || j == 0) continue;

				// X
				DrawLine3D(
					VGet(transform.position.x - m_size.x / 2 + m_offset.x, transform.position.y + m_size.y / 2 * i + m_offset.y, transform.position.z + m_size.z / 2 * j + m_offset.z),
					VGet(transform.position.x + m_size.x / 2 + m_offset.x, transform.position.y + m_size.y / 2 * i + m_offset.y, transform.position.z + m_size.z / 2 * j + m_offset.z),
					GetColor(255, 255, 0));

				// Y
				DrawLine3D(
					VGet(transform.position.x + m_size.x / 2 * j + m_offset.x, transform.position.y + m_size.y / 2 + m_offset.y, transform.position.z + m_size.z / 2 * i + m_offset.z),
					VGet(transform.position.x + m_size.x / 2 * j + m_offset.x, transform.position.y - m_size.y / 2 + m_offset.y, transform.position.z + m_size.z / 2 * i + m_offset.z),
					GetColor(255, 255, 0));

				// Z
				DrawLine3D(
					VGet(transform.position.x + m_size.x / 2 * i + m_offset.x, transform.position.y + m_size.y / 2 * j + m_offset.y, transform.position.z + m_size.z / 2 + m_offset.z),
					VGet(transform.position.x + m_size.x / 2 * i + m_offset.x, transform.position.y + m_size.y / 2 * j + m_offset.y, transform.position.z - m_size.z / 2 + m_offset.z),
					GetColor(255, 255, 0));
			}
		}
	}
};