#include "Bottle.h"

void Bottle::Update()
{
	m_transform.position.y -= Gravity;
}