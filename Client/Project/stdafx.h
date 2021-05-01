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
	UI_BtlPanel,
	Eff_Glint,
	Eff_OvertureHand,
	Eff_Liquid,
	Eff_AppearGroundMonster,
	Eff_Smoke,
	Eff_QliphothBlock,
	Eff_AirHike,
	Eff_Trail,
	Eff_CircleWave,
	Eff_SpriteEffect,
	Eff_DashTrail,
	ThrowCar,
	TAG_Camera,
	TAG_RedQueen,
	TAG_BusterArm_Right,
	TAG_WireArm,
	TAG_WireArm_Grab,
	Overture,
	Tag_Cbs_Short,
	Tag_Cbs_Middle,
	Tag_Cbs_Long,
	Tag_END
};

#define OGDistance 0.33f
#define MaxDistance 0.48f
#define RotateDistance 0.7f