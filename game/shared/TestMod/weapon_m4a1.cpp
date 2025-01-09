//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "weapon_csbase.h"
#include "fx_cs_shared.h"
#include "projectile.h"

#if defined( CLIENT_DLL )

	#define CWeaponM4A1 C_WeaponM4A1
	#include "c_cs_player.h"
#else

	#include "cs_player.h"
	#include "datacache/imdlcache.h"
#endif

class CWeaponM4A1 : public CWeaponCSBase
{
public:
	DECLARE_CLASS( CWeaponM4A1, CWeaponCSBase );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
	
	CWeaponM4A1();

	virtual void Precache(void);

	virtual void PrimaryAttack();
	virtual void SecondaryAttack();

 	virtual float GetInaccuracy() const;
	virtual float GetSpread() const;

	virtual CSWeaponID GetWeaponID( void ) const		{ return WEAPON_M4A1; }
	virtual int GetSlot(void) const { return WEAPON_SLOT_RIFLE; }

private:
	CWeaponM4A1( const CWeaponM4A1 & );

	float m_flCycleTime;

	bool b;
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
	b = true;
}

void CWeaponM4A1::Precache(void)
{
	BaseClass::Precache();
}

float CWeaponM4A1::GetInaccuracy() const
{
	return 0;
}

float CWeaponM4A1::GetSpread() const
{
	return 0;
}

/*
void CWeaponM4A1::PrimaryAttack()
{
	CCSPlayer *pPlayer = GetPlayerOwner();
	if ( !pPlayer )
		return;

	pPlayer->m_iShotsFired++;

	//m_iClip1--;

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

	Vector forward;
	pPlayer->EyeVectors(&forward);

	trace_t tr;
	UTIL_TraceLine(pPlayer->Weapon_ShootPosition(),
		pPlayer->Weapon_ShootPosition() + forward * MAX_TRACE_LENGTH, MASK_NPCSOLID,
		pPlayer, COLLISION_GROUP_NONE, &tr);
	CProjectile *pProp = dynamic_cast<CProjectile *>(tr.m_pEnt);
	if (pProp)
	{
		Msg("Hit CPropjectile: %s\n", pProp->GetModelName());
		pProp->SetPhysicsActive(!pProp->IsPhysicsActive());
	}

	#endif

	// CSBaseGunFire can kill us, forcing us to drop our weapon, if we shoot something that explodes
	pPlayer = GetPlayerOwner();
	if (!pPlayer)
		return;
}
*/

void CWeaponM4A1::PrimaryAttack()
{
	CCSPlayer *pPlayer = GetPlayerOwner();
	Vector forward;
	pPlayer->EyeVectors(&forward);
	Vector vTraceStart, vTraceEnd;
	vTraceStart = pPlayer->Weapon_ShootPosition();
	vTraceEnd = pPlayer->Weapon_ShootPosition() + forward * MAX_TRACE_LENGTH;

	trace_t tr;
	UTIL_TraceLine(vTraceStart, vTraceEnd, MASK_NPCSOLID, pPlayer, COLLISION_GROUP_NONE, &tr);
	CProjectile *target = dynamic_cast<CProjectile *>(tr.m_pEnt);
	//CDynamicProp *target = dynamic_cast<CDynamicProp *>(tr.m_pEnt);
	if (target)
	{
		IPhysicsObject *pObject = target->VPhysicsGetObject();

		#ifdef CLIENT_DLL
		Msg("Client: %fl\n", 1);
		#else
		Msg("Server: %fl\n", 1);
		#endif
	}
	else Msg("No hit\n");
}

void CWeaponM4A1::SecondaryAttack()
{
	CCSPlayer *pPlayer = GetPlayerOwner();
	if (!pPlayer)
		return;

	pPlayer->m_iShotsFired++;

	//m_iClip1--;

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

	m_flNextSecondaryAttack = gpGlobals->curtime + m_flCycleTime;

	SendWeaponAnim(ACT_VM_PRIMARYATTACK);

#ifndef CLIENT_DLL

	MDLCACHE_CRITICAL_SECTION();

	MDLHandle_t h = mdlcache->FindMDL(PROJECTILE_BLOCK);
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
	//UTIL_TraceHull(vTraceStart, vTraceEnd, vecSweepMins, vecSweepMaxs, MASK_NPCSOLID, pPlayer, COLLISION_GROUP_NONE, &tr);
	UTIL_TraceLine(vTraceStart, vTraceEnd, MASK_NPCSOLID, pPlayer, COLLISION_GROUP_NONE, &tr);

	// No space?
	if (tr.allsolid)
		return;

	VectorMA(tr.endpos, 1.0f, tr.plane.normal, tr.endpos);

	bool bAllowPrecache = CBaseEntity::IsPrecacheAllowed();
	CBaseEntity::SetAllowPrecache(true);

	// Try to create entity
	CProjectile *pProp = dynamic_cast<CProjectile *>(CreateEntityByName("prop_projectile"));
	//CDynamicProp *pProp = dynamic_cast<CDynamicProp *>(CreateEntityByName("prop_dynamic"));
	if (pProp)
	{
		char buf[512];
		// Pass in standard key values
		Q_snprintf(buf, sizeof(buf), "%.10f %.10f %.10f", tr.endpos.x, tr.endpos.y, tr.endpos.z);
		pProp->KeyValue("origin", buf);
		Q_snprintf(buf, sizeof(buf), "%.10f %.10f %.10f", angles.x, angles.y, angles.z);
		pProp->KeyValue("angles", buf);
		pProp->KeyValue("model", PROJECTILE_BLOCK);
		pProp->KeyValue("fademindist", "-1");
		pProp->KeyValue("fademaxdist", "0");
		pProp->KeyValue("fadescale", "1");
		pProp->KeyValue("solid", "6");
		pProp->KeyValue("inertiaScale", "1.0");
		pProp->KeyValue("physdamagescale", "0.1");

		pProp->Precache();
		DispatchSpawn(pProp);

		pProp->SetPhysicsActive(false);

		if (b)
		{
			// physics absbox wrong
			// dynamic collision box correct but player stuck on prop
			pProp->Activate();
			pProp->SetModelScale(0.5f);
			b = !b;
		}
		else
		{
			// physics correct
			// dynamic no collision box
			pProp->SetModelScale(0.5f);
			pProp->Activate();
			b = !b;
		}

		//pProp->SetCollisionBoundsFromModel();
	}
	CBaseEntity::SetAllowPrecache(bAllowPrecache);

#endif

	// CSBaseGunFire can kill us, forcing us to drop our weapon, if we shoot something that explodes
	pPlayer = GetPlayerOwner();
	if (!pPlayer)
		return;
}
