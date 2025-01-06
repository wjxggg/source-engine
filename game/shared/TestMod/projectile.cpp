#include "projectile.h"
#include "util.h"

LINK_ENTITY_TO_CLASS(prop_projectile, CProjectile);

BEGIN_DATADESC(CProjectile)
END_DATADESC()

void CProjectile::SetPhysicsActive(bool enable)
{
	m_bVPhysicsActive = enable;
	IPhysicsObject *pPhysics = VPhysicsGetObject();
	pPhysics->EnableMotion(m_bVPhysicsActive);
	if (m_bVPhysicsActive)
	{
		pPhysics->Wake();
	}
}

bool CProjectile::IsPhysicsActive(void)
{
	return m_bVPhysicsActive;
}

void CProjectile::VPhysicsUpdate(IPhysicsObject *pPhysics)
{
	if (m_bVPhysicsActive)
	{
		BaseClass::VPhysicsUpdate(pPhysics);
	}
}

void CProjectile::VPhysicsCollision(int index, gamevcollisionevent_t *pEvent)
{
	if (m_bVPhysicsActive)
	{
		BaseClass::VPhysicsCollision(index, pEvent);
	}
}
