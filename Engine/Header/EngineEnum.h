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
	Attack_Front, // ��
	Attack_Back,
	Attack_L,
	Attack_R,
	Attack_Down,
	Attack_Stun,
	Attack_KnocBack, //�߱�����
	Attack_Homerun,  //��¥ Ȩ��
	Attack_Hard, //������
	Attack_Air_Start, //�ö󰡱� ����
	Attack_Air,	//�ö�������
	Attack_Buster_Start,
	Attack_Buster_Loop,
	Attack_Buster_End,
	Attack_END
};



END
#endif // !__ENGINE_ENUM_H__