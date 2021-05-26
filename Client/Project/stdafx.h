// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �Ǵ� ������Ʈ ���� ���� ������
// ��� �ִ� ���� �����Դϴ�.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
// Windows ��� ����:

// C ��Ÿ�� ��� �����Դϴ�.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include "Engine.h"
#include <windows.h>

// default = 0.01

static constexpr float GScale = 0.001f;

USING(ENGINE)
using namespace std;
// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.


enum GAMEOBJECTTAG
{
	Player = 1,
	Monster100,
	Monster200,
	Monster0000,
	Monster5000,
	Monster5300,
	Monster1000,
	MonsterWeapon,
	MonsterSnatchPoint,
	UI_BtlPanel,
	UI_Font,
	UI_LogoPanel,
	UI_TitlePanel,
	UI_LoadingPanel,
	UI_ShopPanel,
	Tag_BrokenBookShelf,
	Tag_Flag,
	Tag_BrokenPeople,
	Tag_BrokenTable,
	Eff_Glint,
	Eff_OvertureHand,
	Eff_Liquid,
	Eff_AppearGroundMonster,
	Eff_Smoke,
	Eff_QliphothBlock,
	Eff_StoneDebrisMulti,
	Eff_StoneDebris,
	Eff_AppearEm1000,
	Eff_ShapeParticle,
	Eff_AirHike,
	Eff_FinalReady,
	Eff_LensFlare,
	Eff_Trail,
	Eff_CbsTrail,
	Eff_CbsMidTrail,
	Eff_CbsLongTrail,
	Eff_BlitzAttack,
	Eff_LongBarrel,
	Eff_BiAttack,
	Eff_Satellite,
	Eff_FireCircle,
	Eff_ElectricOccur,
	Eff_ThunderBolt,
	Eff_ShockWave,
	Eff_ThunderBoltSecond,
	Eff_ElectricBranch,
	Eff_ElectricVortex,
	Eff_IceAge,
	Eff_ArteMisMissile,
	Eff_Reverberation,
	Eff_Change,
	Eff_CircleWave,
	Eff_SpriteEffect,
	Eff_DashTrail,
	Eff_MakaiButterFly,
	Eff_SecretVisionMagicCircle,
	Eff_FadeOut,
	Eff_WhirlWind,
	ThrowCar,
	TAG_Camera,
	TAG_RedQueen,
	TAG_BusterArm_Right,
	TAG_BusterArm_Left,
	TAG_WireArm,
	TAG_WireArm_Grab,
	Overture,
	TAG_SecretVision,
	Tag_Cbs_Short,
	Tag_Cbs_Middle,
	Tag_Cbs_Long,
	TAG_NeroShinMajin,
	TAG_JudgementShadow1,
	TAG_JudgementShadow2,
	TAG_JudgementShadow3,
	TAG_Trigger,
	TAG_NhDoor,
	Eff_SnatchRush,
	Eff_Streak,
	Tag_END
};

enum WingSwordAnimationList
{
	WingSword_Ar1,
	WingSword_Ar2,
	WingSword_Ar3,
	WingSword_Ar4,
	WingSword_ComboA1,
	WingSword_ComboA2,
	WingSword_ComboA3,
	WingSword_ComboB1,
	WingSword_Stinger_End,
	WingSword_Stinger_Loop,
	WingSword_Stinger_Start,
	WingSword_HT,
	WingSword_ComboB2_Loop,
	WingSword_ComboB2_End,
	WingSword_Helm,
	WingSword_End
};


struct TrailDesc
{
	uint32 VtxSize{};
	uint32 VtxCnt{};
	uint32 TriCnt{};
	uint32 IdxSize{};
	D3DFORMAT IdxFmt{};

	int32 DrawTriCnt;
	uint32 NewVtxCnt;
	float UpdateCycle;
	float CurVtxUpdateCycle;
};


enum SCENE_ID
{
	HOTEL_S01,
	HOTEL_S02,
	HOTEL_S03,
	HOTEL_S04,
	LIBRARY_S05,
	LIBRARY_S06,

	SCENE_ID_END
};


#define OGDistance 0.53f
#define MaxDistance 0.78f
#define RotateDistance 0.7f
