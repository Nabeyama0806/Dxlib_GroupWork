#include "Bottle.h"

Bottle::Bottle(Vector3& position) :
	ModelActor("Bottle", nullptr, position),
	m_type(Bottle::Type::Fire)
{

}

void Bottle::Throw(Vector3& position, Vector3& addForce)
{

}