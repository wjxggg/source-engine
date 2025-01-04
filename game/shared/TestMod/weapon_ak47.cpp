//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "weapon_csbase.h"
#include "fx_cs_shared.h"

#if defined( CLIENT_DLL )

	#define CAK47 C_AK47
	#include "c_cs_player.h"

#else

	#include "cs_player.h"

#endif


class CAK47 : public CWeaponCSBase
{
public:
	DECLARE_CLASS( CAK47, CWeaponCSBase);
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();
	
	CAK47();

	virtual void Precache(void);

	virtual void PrimaryAttack();

	virtual float GetInaccuracy() const;
	virtual float GetSpread() const;

	virtual CSWeaponID GetWeaponID( void ) const		{ return WEAPON_AK47; }

private:
	CAK47( const CAK47 & );

	float m_flCycleTime;
};

IMPLEMENT_NETWORKCLASS_ALIASED( AK47, DT_WeaponAK47 )

BEGIN_NETWORK_TABLE( CAK47, DT_WeaponAK47 )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CAK47 )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_ak47, CAK47 );
PRECACHE_WEAPON_REGISTER( weapon_ak47 );

// ---------------------------------------------------------------------------- //
// CAK47 implementation.
// ---------------------------------------------------------------------------- //

CAK47::CAK47()
{
	m_flCycleTime = 0.16f;
}

void CAK47::Precache(void)
{
	BaseClass::Precache();
}

float CAK47::GetInaccuracy() const
{
	return 0;
}

float CAK47::GetSpread() const
{
	return 0;
}

void CAK47::PrimaryAttack()
{
	CCSPlayer *pPlayer = GetPlayerOwner();
	if (!pPlayer)
		return;

	pPlayer->m_iShotsFired++;

	m_iClip1--;

	// player "shoot" animation
	pPlayer->SetAnimation(PLAYER_ATTACK1);

	FX_FireBullets(
		pPlayer->entindex(),
		pPlayer->Weapon_ShootPosition(),
		pPlayer->EyeAngles() + 2.0f * pPlayer->GetPunchAngle(),
		GetWeaponID(),
		Primary_Mode,
		CBaseEntity::GetPredictionRandomSeed() & 255, // wrap it for network traffic so it's the same between client and server
		GetInaccuracy(),
		GetSpread(),
		gpGlobals->curtime);

	m_flNextPrimaryAttack = gpGlobals->curtime + m_flCycleTime;

	SendWeaponAnim(ACT_VM_PRIMARYATTACK);

	// CSBaseGunFire can kill us, forcing us to drop our weapon, if we shoot something that explodes
	pPlayer = GetPlayerOwner();
	if (!pPlayer)
		return;
}
