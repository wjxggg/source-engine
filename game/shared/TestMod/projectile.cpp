#include "projectile.h"

#ifdef CLIENT_DLL

IMPLEMENT_CLIENTCLASS_DT(CProjectile, DT_Projectile, CProjectile)
RecvPropBool(RECVINFO(m_bVPhysicsActive)),
END_RECV_TABLE()

void CProjectile::SetPhysicsActive(bool enable)
{
	// Do nothing on client side
}

bool CProjectile::IsPhysicsActive(void)
{
	return m_bVPhysicsActive;
}

#else

LINK_ENTITY_TO_CLASS(prop_projectile, CProjectile);

BEGIN_DATADESC(CProjectile)
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST(CProjectile, DT_Projectile)
SendPropBool(SENDINFO(m_bVPhysicsActive)),
END_SEND_TABLE()

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

#endif