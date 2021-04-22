// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
// Windows 헤더 파일:

// C 런타임 헤더 파일입니다.
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include "Engine.h"
#include <windows.h>


USING(ENGINE)
using namespace std;
// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.


enum GAMEOBJECTTAG
{
	Player = 1,
	Monster100,
	Monster101,
	Monster0000,
	Monster5000,
	MonsterWeapon,
	UI_BtlPanel,
	Eff_Glint,
	Eff_OvertureHand,
	ThrowCar,
	TAG_Camera,
	TAG_RedQueen,
	TAG_BusterArm_Right,
	TAG_WireArm,
	Overture,
	Tag_END
};

#define OGDistance 2.4f
#define MaxDistance 3.7f
#define RotateDistance 3.f