#pragma once

class Transform;
class BoxCollider;
class CircleCollider;

class Collision
{
public:
	static bool Check(
		const Transform& transform1, const BoxCollider* box1,
		const Transform& transform2, const BoxCollider* box2
	);

	static bool Check(
		const Transform& transform1, const CircleCollider* box2,
		const Transform& transform2, const BoxCollider* box1
	);

	static bool Check(
		const Transform& transform1, const CircleCollider* box1,
		const Transform& transform2, const CircleCollider* box2
	);
};