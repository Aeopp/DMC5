#ifndef __ENGINE_ENUM_H__
#define __ENGINE_ENUM_H__
BEGIN(ENGINE)

enum MOUSEAXIS
{
	DIM_X,
	DIM_Y,
	DIM_Z,
	MOUSEAXIS_END
};
enum MOUSEBUTTON
{
	DIM_L,
	DIM_R,
	DIM_M,
	MOUSEBUTTON_END
};
enum ATTACKTYPE
{
	Attack_Front, // 윽
	Attack_Back,
	Attack_L,
	Attack_R,
	Attack_Down,
	Attack_Stun,
	Attack_KnocBack, //뒹구르르
	Attack_Hard, //으으윽
	Attack_Air_Start, //올라가기 시작
	Attack_Air,	//올라가있을때
	Attack_END
};



END
#endif // !__ENGINE_ENUM_H__