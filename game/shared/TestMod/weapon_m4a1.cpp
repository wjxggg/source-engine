//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "weapon_csbase.h"
#include "fx_cs_shared.h"
#ifndef CLIENT_DLL
	#include "func_break.h"
#endif

#if defined( CLIENT_DLL )

	#define CWeaponM4A1 C_WeaponM4A1
	#include "c_cs_player.h"

#else

	#include "cs_player.h"

#endif

//#define BULLET_MODEL "models/bullets/cube.mdl"
#define BULLET_MODEL "models/props_c17/oildrum001.mdl"

class CWeaponM4A1 : public CWeaponCSBase
{
public:
	DECLARE_CLASS( CWeaponM4A1, CWeaponCSBase );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	
	CWeaponM4A1();

	virtual void Precache(void);

	virtual void PrimaryAttack();

 	virtual float GetInaccuracy() const;
	virtual float GetSpread() const;

	virtual CSWeaponID GetWeaponID( void ) const		{ return WEAPON_M4A1; }
	virtual int GetSlot(void) const { return WEAPON_SLOT_PISTOL; }

private:
	CWeaponM4A1( const CWeaponM4A1 & );

	float m_flCycleTime;
};

IMPLEMENT_NETWORKCLASS_ALIASED( WeaponM4A1, DT_WeaponM4A1 )

BEGIN_NETWORK_TABLE( CWeaponM4A1, DT_WeaponM4A1 )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CWeaponM4A1 )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( weapon_m4a1, CWeaponM4A1 );
PRECACHE_WEAPON_REGISTER( weapon_m4a1 );

// ---------------------------------------------------------------------------- //
// CWeaponM4A1 implementation.
// ---------------------------------------------------------------------------- //

CWeaponM4A1::CWeaponM4A1()
{
	m_flCycleTime = 0.16f;
}

void CWeaponM4A1::Precache(void)
{
	BaseClass::Precache();

	PrecacheModel(BULLET_MODEL);
}

float CWeaponM4A1::GetInaccuracy() const
{
	return 0;
}

float CWeaponM4A1::GetSpread() const
{
	return 0;
}

void CWeaponM4A1::PrimaryAttack()
{
	CCSPlayer *pPlayer = GetPlayerOwner();
	if ( !pPlayer )
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

	#ifndef CLIENT_DLL
		
	Vector forward, offset;
	pPlayer->EyeVectors(&forward);
	offset = forward * 60;

	CBaseEntity *pEntity = CBaseEntity::CreateNoSpawn("prop_physics_multiplayer", pPlayer->Weapon_ShootPosition() + offset, pPlayer->EyeAngles(), pPlayer);

	//pEntity->AddSpawnFlags(pPlayer->GetSpawnFlags());

	// We never want to be motion disabled
	//pEntity->RemoveSpawnFlags(SF_PHYSPROP_MOTIONDISABLED);
	// Inherit the base object's damage modifiers
	//pEntity->SetDmgModBullet(100);
	//pEntity->SetDmgModClub(100);
	//pEntity->SetDmgModExplosive(100);

	pEntity->SetModelName(AllocPooledString(BULLET_MODEL));
	pEntity->SetModel(STRING(pEntity->GetModelName()));
	pEntity->SetCollisionGroup(COLLISION_GROUP_NONE);

	pEntity->SetSolid(SOLID_BBOX);
	pEntity->AddSolidFlags(FSOLID_NOT_SOLID);

	//pEntity->SetFadeDistance(fadeMinDist, fadeMaxDist);

	//pEntity->AddSpawnFlags(SF_PHYSPROP_IS_GIB);

	pEntity->Spawn();

	#endif

	m_flNextPrimaryAttack = gpGlobals->curtime + m_flCycleTime;

	SendWeaponAnim(ACT_VM_PRIMARYATTACK);

	// CSBaseGunFire can kill us, forcing us to drop our weapon, if we shoot something that explodes
	pPlayer = GetPlayerOwner();
	if (!pPlayer)
		return;
}
