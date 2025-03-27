#pragma once
#include "ModelActor.h"
#include "BoxCollider.h"

class HitBox : public ModelActor
{
public:
	HitBox(const char* name, const Vector3& position, const Vector3& size) :
		ModelActor(name, nullptr, position)
	{
		m_collider = new BoxCollider(size);
	}
};