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


USING(ENGINE)
using namespace std;
// TODO: ���α׷��� �ʿ��� �߰� ����� ���⿡�� �����մϴ�.


enum GAMEOBJECTTAG
{
	Player = 1,
	Monster100,
	Monster101,
	Monster0000,
	Monster5000,
	UI_BtlPanel,
	Eff_Glint,
	Eff_OvertureHand,
	Eff_Liquid,
	ThrowCar,
	TAG_Camera,
	TAG_RedQueen,
	TAG_BusterArm_Right,
	TAG_WireArm,
	Tag_END
};

#define OGDistance 2.4f
#define MaxDistance 3.7f