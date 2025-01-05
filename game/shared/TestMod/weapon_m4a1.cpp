//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "weapon_csbase.h"
#include "fx_cs_shared.h"
#include "datacache/imdlcache.h"
#include "props.h"

#if defined( CLIENT_DLL )

	#define CWeaponM4A1 C_WeaponM4A1
	#include "c_cs_player.h"

#else

	#include "cs_player.h"

#endif

#define PROJECTILE_MODEL "models/props_junk/wood_crate001a.mdl"
#define SHOOT_POSITION_OFFSET_FORWARD 200

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

	PrecacheModel(PROJECTILE_MODEL);
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

	m_flNextPrimaryAttack = gpGlobals->curtime + m_flCycleTime;

	SendWeaponAnim(ACT_VM_PRIMARYATTACK);

	#ifndef CLIENT_DLL

	MDLCACHE_CRITICAL_SECTION();

	MDLHandle_t h = mdlcache->FindMDL(PROJECTILE_MODEL);
	if (h == MDLHANDLE_INVALID)
		return;

	// Must have vphysics to place as a physics prop
	studiohdr_t *pStudioHdr = mdlcache->GetStudioHdr(h);
	if (!pStudioHdr)
		return;

	// Must have vphysics to place as a physics prop
	if (!mdlcache->GetVCollide(h))
		return;

	QAngle angles(0.0f, 0.0f, 0.0f);
	Vector vecSweepMins = pStudioHdr->hull_min;
	Vector vecSweepMaxs = pStudioHdr->hull_max;

	Vector forward;
	pPlayer->EyeVectors(&forward);

	Vector vTraceStart, vTraceEnd;
	vTraceStart = pPlayer->Weapon_ShootPosition();
	vTraceEnd = pPlayer->Weapon_ShootPosition() + forward * SHOOT_POSITION_OFFSET_FORWARD;

	trace_t tr;
	UTIL_TraceHull(vTraceStart, vTraceEnd,
		vecSweepMins, vecSweepMaxs, MASK_NPCSOLID, pPlayer, COLLISION_GROUP_NONE, &tr);

	// No space?
	if (tr.allsolid)
		return;

	VectorMA(tr.endpos, 1.0f, tr.plane.normal, tr.endpos);

	bool bAllowPrecache = CBaseEntity::IsPrecacheAllowed();
	CBaseEntity::SetAllowPrecache(true);

	// Try to create entity
	CPhysicsProp *pProp = dynamic_cast<CPhysicsProp *>(CreateEntityByName("physics_prop"));
	if (pProp)
	{
		char buf[512];
		// Pass in standard key values
		Q_snprintf(buf, sizeof(buf), "%.10f %.10f %.10f", tr.endpos.x, tr.endpos.y, tr.endpos.z);
		pProp->KeyValue("origin", buf);
		Q_snprintf(buf, sizeof(buf), "%.10f %.10f %.10f", angles.x, angles.y, angles.z);
		pProp->KeyValue("angles", buf);
		pProp->KeyValue("model", PROJECTILE_MODEL);
		pProp->KeyValue("fademindist", "-1");
		pProp->KeyValue("fademaxdist", "0");
		pProp->KeyValue("fadescale", "1");
		pProp->KeyValue("inertiaScale", "1.0");
		pProp->KeyValue("physdamagescale", "0.1");
		pProp->Precache();
		DispatchSpawn(pProp);
		pProp->Activate();
	}
	CBaseEntity::SetAllowPrecache(bAllowPrecache);

	#endif

	// CSBaseGunFire can kill us, forcing us to drop our weapon, if we shoot something that explodes
	pPlayer = GetPlayerOwner();
	if (!pPlayer)
		return;
}
