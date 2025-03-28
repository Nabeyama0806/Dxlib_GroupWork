#include "Collision.h"
#include "Vector3.h"
#include "Transform.h"
#include "BoxCollider.h"
#include "CircleCollider.h"
#include <cmath>
using std::abs;
using std::powf;

bool Collision::Check(
	const Transform& transform1, const BoxCollider* box1,
	const Transform& transform2, const BoxCollider* box2
	)
{
	//各矩形の中心座標とサイズを算出
	Vector3 center1 = transform1.position + box1->m_offset.Scale(transform1.scale);
	Vector3 size1 = box1->m_size.Scale(transform1.scale);

	Vector3 center2 = transform2.position + box2->m_offset.Scale(transform2.scale);
	Vector3 size2 = box2->m_size.Scale(transform2.scale);

	//各軸の中心座標同士の距離と、サイズ/2の合計を比較
	if (abs(center1.x - center2.x) <= (size1.x + size2.x) / 2
	&&  abs(center1.y - center2.y) <= (size1.y + size2.y) / 2
	&&  abs(center1.z - center2.z) <= (size1.z + size2.z) / 2)
	{
		return true;
	}
	return false;
}

bool Collision::Check(
	const Transform& transform1, const BoxCollider* box,
	const Transform& transform2, const CircleCollider* circle
	)
{
	//矩形と円形の当たり判定(未実装)
	return false;
}

bool Collision::Check(
	const Transform& transform1, const CircleCollider* box,
	const Transform& transform2, const CircleCollider* circle
	)
{
	//円形と円形の当たり判定(未実装)
	return false;
}