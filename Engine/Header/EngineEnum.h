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
	Attack_Normal,
	Attack_Down,
	Attack_Stun,
	Attack_KnocBack,
	Attack_END
};



END
#endif // !__ENGINE_ENUM_H__