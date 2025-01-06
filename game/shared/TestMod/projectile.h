#include "cbase.h"
#include "props.h"

#define SHOOT_POSITION_OFFSET_FORWARD 200

#define PROJECTILE_BLOCK "models/props/de_dust/du_crate_64x64.mdl"

class CProjectile : public CPhysicsProp
{
	DECLARE_CLASS(CProjectile, CPhysicsProp);
	DECLARE_DATADESC();

public:
	//~CProjectile();
	CProjectile(void)
	{
		m_bVPhysicsActive = true;
	}

	void SetPhysicsActive(bool enable);
	bool IsPhysicsActive(void);

	virtual void VPhysicsUpdate(IPhysicsObject *pPhysics);
	virtual void VPhysicsCollision(int index, gamevcollisionevent_t *pEvent);

	//virtual int OnTakeDamage(const CTakeDamageInfo &info);

private:
	bool m_bVPhysicsActive;
};
