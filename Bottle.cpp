#include "Bottle.h"

Bottle::Bottle(Vector3& position) :
	ModelActor("Bottle", nullptr, position),
	m_type(Bottle::Type::Fire)
{

}

