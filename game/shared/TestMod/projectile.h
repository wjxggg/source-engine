#pragma once

#include "cbase.h"

#ifdef CLIENT_DLL
	#include "c_props.h"
	#include "c_physicsprop.h"
	#define CPhysicsProp C_PhysicsProp
#else
	#include "props.h"
#endif

#define SHOOT_POSITION_OFFSET_FORWARD 200
#define PROJECTILE_BLOCK "models/props/de_dust/du_crate_64x64.mdl"

#ifdef CLIENT_DLL

class CProjectile : public CPhysicsProp
{
	DECLARE_CLASS(CProjectile, CPhysicsProp);
	DECLARE_CLIENTCLASS();

public:
	//~CProjectile();
	CProjectile(void)
	{
		m_bVPhysicsActive = true;
	}

	void SetPhysicsActive(bool enable);
	bool IsPhysicsActive(void);

	//virtual void VPhysicsUpdate(IPhysicsObject *pPhysics);
	//virtual void VPhysicsCollision(int index, gamevcollisionevent_t *pEvent);

	//virtual int OnTakeDamage(const CTakeDamageInfo &info);

private:
	CNetworkVar(bool, m_bVPhysicsActive);
};

#else
class CProjectile : public CPhysicsProp
{
	DECLARE_CLASS(CProjectile, CPhysicsProp);
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

public:
	//~CProjectile();
	CProjectile(void)
	{
		m_bVPhysicsActive = true;
	}

	void SetPhysicsActive(bool enable);
	bool IsPhysicsActive(void);

	//virtual void VPhysicsUpdate(IPhysicsObject *pPhysics);
	//virtual void VPhysicsCollision(int index, gamevcollisionevent_t *pEvent);

	//virtual int OnTakeDamage(const CTakeDamageInfo &info);

private:
	CNetworkVar(bool, m_bVPhysicsActive);
};

#endif
