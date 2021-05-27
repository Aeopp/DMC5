#ifndef Player_h__
#define Player_h__

#pragma once
#include "Unit.h"
#include "RenderInterface.h"
class NeroFSM;
class RedQueen;
class Nero_LWing;
class Nero_RWing;
class Buster_Arm;
class Buster_Arm_Left;
class Wire_Arm;
class WIngArm_Left;
class WingArm_Right;
class MainCamera;
class BtlPanel;
class GT_Overture;
class GT_Rockman;
class Monster;
class Effect;
class Liquid;
class Cbs_Short;
class Cbs_Middle;
class Cbs_Long;
class AirHike;
class CircleWave;
class FireCircle;
class IceAge;
class Trail;
class ShapeParticle;
class JudgementSword;
class JudgementShadow1;
class JudgementShadow2;
class JudgementShadow3;
class CbsTrail;
class CbsMidTrail;
class CbsLongTrail;
class BiAttack;
class BlitzAttack;
class LongBarrel;
class Satellite;
class WhirlWind;
class NewWingSword;
class Change;
class ShockWave;
class StoneDebrisMulti;
/*Coat Test*/
class NeroCoat;
class Nero : public Unit,
	public ENGINE::RenderInterface

{
public:
	enum Animation_List
	{
		ANI_AIR_DIVE_SLASH_FINISH,
		ANI_AIR_DIVE_SLASH_LOOP,
		ANI_AIR_DIVE_SLASH_START,
		ANI_CALIBER_END,
		ANI_CALIBER_START,
		ANI_CBS_COMBOA1,
		ANI_CBS_COMBOA2,
		ANI_CBS_COMBOA3,
		ANI_CBS_COMBOA4,
		ANI_CBS_COMBOA5,
		ANI_CBS_IDLE,
		ANI_CBS_IDLE_TO_IDLE,
		ANI_CBS_SKILL_CRYSTAL,
		ANI_CBS_SKILL_ICEAGE_END,
		ANI_CBS_SKILL_ICEAGE_LOOP,
		ANI_CBS_SKILL_ICEAGE_START,
		ANI_CBS_SKILL_REVOLVER_END,
		ANI_CBS_SKILL_REVOLVER_LOOP,
		ANI_CBS_SKILL_REVOLVER_START,
		ANI_CBS_SKILL_SWING,
		ANI_COMBOA1,
		ANI_COMBOA1_DASH,
		ANI_COMBOA2,
		ANI_COMBOA3,
		ANI_COMBOA4,
		ANI_COMBOA_AIR1,
		ANI_COMBOA_AIR2,
		ANI_COMBOA_AIR3,
		ANI_COMBOB,
		ANI_COMBOB_AIR,
		ANI_COMBOC1,
		ANI_COMBOC2,
		ANI_COMBOC3,
		ANI_COMBOC4,
		ANI_COMBOC_L,
		ANI_COMBOC_R,
		ANI_COMBOD1,
		ANI_COMBOD2,
		ANI_COMBOD3,
		ANI_COMBOD4,
		ANI_DASHLOOP,
		ANI_DASHLOOP2,
		ANI_DASHSTOP,
		ANI_DASHTURN,
		ANI_DIVE_SLASH_FINISH,
		ANI_DIVE_SLASH_LOOP,
		ANI_DIVE_SLASH_MOVE,
		ANI_DIVE_SLASH_START,
		ANI_EARTHQUAKEEND,
		ANI_EARTHQUAKELOOP,
		ANI_EARTHQUAKESTART,
		ANI_EVADE_L,
		ANI_EVADE_R,
		ANI_GT_AIR_CRASH_JUST,
		ANI_GT_AIR_CRASH_RELOAD,
		ANI_GT_BOMB,
		ANI_GT_CRASH_JUST,
		ANI_GT_CRASH_RELOAD,
		ANI_GT_PICKUP,
		ANI_GT_RELOAD,
		ANI_GT_RELOAD_STYLISH,
		ANI_GT_REPELLED,
		ANI_HITFRONT,
		ANI_HITFRONT_BIG,
		ANI_HIT_AIR,
		ANI_HIT_AIR_AWAY,
		ANI_HIT_GROUND_AWAY,
		ANI_HR_AIR,
		ANI_HR_EX_2,
		ANI_HR_EX_3,
		ANI_HR_EX_AIR_FINISH,
		ANI_HR_EX_AIR_ROLL_END,
		ANI_HR_EX_AIR_ROLL_LOOP,
		ANI_HR_EX_AIR_ROLL_START,
		ANI_HR_EX_FINISH,
		ANI_HR_EX_START,
		ANI_HR_GROUND,
		ANI_IDLE_BATTLE,
		ANI_IDLE_FROM_COMBOA1,
		ANI_IDLE_FROM_COMBOA1_END,
		ANI_IDLE_FROM_COMBOA1_LOOP,
		ANI_IDLE_FROM_COMBOA2,
		ANI_IDLE_FROM_COMBOA2_END,
		ANI_IDLE_FROM_COMBOA2_LOOP,
		ANI_IDLE_NORMAL,
		ANI_JUMP,
		ANI_JUMP_BACK,
		ANI_JUMP_BACK_TWICE,
		ANI_JUMP_FRONT,
		ANI_JUMP_FRONT_LANDING,
		ANI_JUMP_FRONT_TWICE,
		ANI_JUMP_LANDING,
		ANI_JUMP_LANDING_HIGH,
		ANI_JUMP_TWICE,
		ANI_CBS_JUMP,
		ANI_CBS_JUMP_BACK,
		ANI_CBS_JUMP_BACK_TWICE,
		ANI_CBS_JUMP_FRONT,
		ANI_CBS_JUMP_FRONT_LANDING,
		ANI_CBS_JUMP_FRONT_TWICE,
		ANI_CBS_JUMP_LANDING,
		ANI_CBS_JUMP_LANDING_HIGH,
		ANI_CBS_JUMP_TWICE,
		ANI_MIDDLE_CBS_BIATTACK,
		ANI_MIDDLE_CBS_BLITZATTACK,
		ANI_MIDDLE_CBS_SATELLITE,
		ANI_MIDDLE_CBS_SATELLITE_AIR,
		ANI_MIDDLE_CBS_STRIKE,
		ANI_MIDDLE_CBS_STRIKE_AIR,
		ANI_MIDDLE_CBS_STRIKE_AIR_DOWN,
		ANI_MIDDLE_CBS_STRIKE_AIR_UP,
		ANI_MIDDLE_CBS_STRIKE_DOWN,
		ANI_MIDDLE_CBS_STRIKE_UP,
		ANI_MIDDLE_CBS_THUNDERBULLET,
		ANI_OVERTURE_SHOOT,
		ANI_OVERTURE_SHOOT_AIR,
		ANI_OVERTURE_SHOOT_AIR_DOWN,
		ANI_OVERTURE_SHOOT_AIR_UP,
		ANI_OVERTURE_SHOOT_DOWN,
		ANI_OVERTURE_SHOOT_UP,
		ANI_PARRYING,
		ANI_PARRYING_AIR,
		ANI_POLE_COMBOA1,
		ANI_POLE_COMBOA2,
		ANI_POLE_COMBOA3,
		ANI_POLE_COMBOB1,
		ANI_POLE_COMBOB2,
		ANI_POLE_COMBOB3,
		ANI_POLE_COMBOB4,
		ANI_POLE_WHIRLWIND_END,
		ANI_POLE_WHIRLWIND_LOOP,
		ANI_POLE_WHIRLWIND_START,
		ANI_RUNLOOP,
		ANI_RUNLOOP_FROM_COMBOA1,
		ANI_RUNSTART0,
		ANI_RUNSTART180,
		ANI_RUNSTART270,
		ANI_RUNSTART90,
		ANI_RUNSTART_FROM_COMBOA1,
		ANI_RUNSTART_FROM_COMBOA2,
		ANI_RUNSTOP,
		ANI_RUNTURN,
		ANI_RUNTURN_L,
		ANI_SHUFFLE,
		ANI_SHUFFLE_EX,
		ANI_SPLITEX_LANDING,
		ANI_SPLITEX_LOOP,
		ANI_SPLITEX_START,
		ANI_SPLIT_LANDING,
		ANI_SPLIT_LOOP,
		ANI_SPLIT_START,
		ANI_STREAK_END,
		ANI_STREAK_EX_ROLL_END,
		ANI_STREAK_EX_ROLL_START,
		ANI_STREAK_EX_RUSH,
		ANI_STREAK_EX_RUSH_LOOP,
		ANI_STREAK_EX_START,
		ANI_STREAK_LOOP,
		ANI_STREAK_START,
		ANI_STUN_END,
		ANI_STUN_LOOP,
		ANI_STUN_START,
		ANI_WIRE_HELLHOUND_AIR_END,
		ANI_WIRE_HELLHOUND_AIR_LOOP,
		ANI_WIRE_HELLHOUND_AIR_F,
		ANI_WIRE_HELLHOUND_END,
		ANI_WIRE_HELLHOUND_LOOP,
		ANI_WIRE_HELLHOUND_F,
		ANI_WIRE_SNATCH_PULL,
		ANI_WIRE_SNATCH_PULL_AIR,
		ANI_WIRE_SNATCH_PULL_AIR_D,
		ANI_WIRE_SNATCH_PULL_AIR_U,
		ANI_WIRE_SNATCH_PULL_D,
		ANI_WIRE_SNATCH_PULL_U,
		ANI_CBS_RUNSTOP,
		ANI_CBS_DASHSTOP,
		ANI_JUMP_LOOP,
		ANI_OVERTURE_IDLE,
		ANI_BUSTER_START,
		ANI_TO_MAJIN,
		ANI_TO_MAJIN2,
		ANI_JOG_LOOP,
		ANI_JOG_STOP,
		ANI_JOG_TURN_180,
		ANI_JOG_TURN_180_L,
		ANI_BUSTER_AIR_CATCH,
		ANI_BUSTER_STRIKE_COMMON,
		ANI_BUSTER_STRIKE_COMMON_AIR,
		ANI_M_BUSTER_STRIKE_COMMON_START,
		ANI_M_BUSTER_STRIKE_COMMON_LOOP,
		ANI_EM0000_BUSTER_START,
		ANI_EM0000_BUSTER_FINISH,
		ANI_EM0000_M_BUSTER,
		ANI_EM0000_BUSTER_AIR,
		ANI_EM5000_BUSTER_START,
		ANI_EM5000_BUSTER_SWING,
		ANI_EM5000_BUSTER_SWING_LOOP,
		ANI_EM5000_BUSTER_FINISH,
		ANI_CBS_RUNLOOP,
		ANI_CBS_RUNSTART90,
		ANI_CBS_RUNSTART180,
		ANI_CBS_RUNSTART270,
		ANI_CBS_RUNSTART0,
		ANI_CBS_DASH,
		ANI_EM200_BUSTER_START,
		ANI_EM200_BUSTER_LOOP,
		ANI_EM200_BUSTER_FINISH,
		ANI_EM200_BUSTER_AIR_START,
		ANI_EM200_BUSTER_AIR_LOOP,
		ANI_EM200_BUSTER_AIR_FINISH,
		ANI_TRANSFORM_TO_SHINMAJIN,
		ANI_TRANSFORM_TO_ORIGIN_DANTE,
		ANI_SHINMAJIN_ENTER,
		ANI_SHINMAJIN_JUDGEMENT,
		ANI_WINDPRESSURE,
		ANI_WINDPRESSURE_END,
		ANI_PROVOKE1,
		ANI_PROVOKE3,
		ANI_PROVOKE9,
		ANI_PROVOKE10,
		ANI_END
	};

	enum WeaponList
	{
		RQ, // ��
		Cbs // ������

	};

	enum JumpDir
	{
		Basic,
		Front,
		Back,
	};

	enum WeaponState
	{
		WS_Idle,
		WS_Battle
	};

	enum NeroDirection
	{
		Dir_Front,
		Dir_Back,
		Dir_Left,
		Dir_Right,
		Dir_Front_Down,
		Dir_Up,
		Dir_Down,
		Dir_End
	};

	enum EffDircetion
	{
		EffDir_Front,
		EffDir_Up,
		EffDir_Down
	};

	enum NeroComponentID
	{
		NeroCom_Wings,
		NeroCom_LWing,
		NeroCom_RWing,
		NeroCom_BusterArm,
		NeroCom_BusterArm_Left,
		NeroCom_WireArm,
		NeroCom_WIngArm_Left,
		NeroCom_WingArm_Right,
		NeroCom_Overture,
		NeroCom_RedQueen,
		NeroCom_Cbs_Short,
		NeroCom_Cbs_Middle,
		NeroCom_Cbs_Long,
		NeroCom_JudgementSword,
		NeroCom_All_Weapon,
		NeroCom_JudgementShadow1,
		NeroCom_JudgementShadow2,
		NeroCom_JudgementShadow3,
		NeroCom_NewWingSword,
		NeroCom_End
	};

	enum MeshIndex
	{
		ORIGIN_DANTE,
		SHINMAJIN_DANTE,
		MESH_END
	};

private:
	explicit Nero();
	virtual ~Nero() = default;
	// GameObject��(��) ���� ��ӵ�
	virtual void Free() override;
public:

	static Nero* Create();
public:
	virtual void RenderReady() override;
	virtual void Editor()override;
public:
	/// <For Weapon>
	void Set_Weapon_Coll(NeroComponentID _eNeroComID, bool _ActiveOrNot);
	void Set_Weapon_AttType(NeroComponentID _eNeroComID, ATTACKTYPE _eAttDir);
	void Set_Weapon_State(NeroComponentID _eNeroComID, UINT _StateIndex);

public:
	// <For Overture Eff>
	void CreateOvertureEff(EffDircetion eDir = EffDir_Front);

public:
	std::weak_ptr<NeroFSM> GetFsm() { return m_pFSM; }
	std::list<std::weak_ptr<Monster>> GetAllMonster();
	std::list<std::weak_ptr<Monster>> GetAliveMonster();
	virtual std::string GetName() override;
	float Get_PlayingTime();
	float Get_PlayingAccTime();
	UINT Get_CurAnimationIndex() { return m_iCurAnimationIndex; }
	UINT Get_PreAnimationIndex() { return m_iPreAnimationIndex; }
	UINT Get_CurDirIndex() { return m_iCurDirIndex; }
	UINT Get_PreDirIndex() { return m_iPreDirIndex; }
	UINT Get_CurWeaponIndex() { return m_iCurWeaponIndex; }
	UINT Get_JumpCount() { return m_iJumpCount; }
	UINT Get_JumpDir() { return m_iJumpDirIndex; }
	std::optional<Matrix> Get_BoneMatrix_ByName(std::string _BoneName);
	Matrix* Get_BoneMatrixPtr(std::string _BoneName);
	Matrix Get_NeroWorldMatrix() { return m_pTransform.lock()->GetWorldMatrix(); }
	Matrix Get_NeroBoneWorldMatrix(std::string _BoneName);
	Vector3 Get_NeroBoneWorldPos(std::string _BoneName);
	bool Get_IsMajinMode() { return m_IsMajin; }
	int  GetDashLoopDir() { return m_iDashLoopDir; }
	std::string GetAniname() { return m_pMesh[m_iMeshIndex]->AnimName; }
	bool Get_IsHaveCbsMiddle() { return m_bIsHaveCbsMiddle; }
	bool Get_IsHaveCbsLong() { return m_bIsHaveCbsLong; }
public:
	void Reset_JumpCount() { m_iJumpCount = 1; }
	void Reset_RotationAngle() { m_fRotationAngle = 0.f; }
	void Reset_RootRotation() { vAccumlatonDegree = { 0.f,0.f,0.f }; }
	void Reset_LerfAmount() { m_fLerfAmount = 0.f; }

	void Set_JumpDir(UINT _iJumpDir) { m_iJumpDirIndex = _iJumpDir; }
	void SetActive_NeroComponent(NeroComponentID _eNeroComID, bool ActiveOrNot);
	void SetAngleFromCamera(float _fAddAngle = 0.f);
	void SetRotationAngle(float _fAngle) { m_fRotationAngle += _fAngle; }
	void SetAddForce(Vector3 _vJumpPos);
	void SetAddForce_Dir(NeroDirection _eDir, float _fPower);
	void SetLockOnMonster();
	void SetOffLockOnMonster();
	void SetDashLoopDir();
	void SetGravity(bool _ActiveOrNot) { m_pCollider.lock()->SetGravity(_ActiveOrNot); }
	void SetLinearVelocity(const D3DXVECTOR3 _vLinearVelocity = D3DXVECTOR3(0.f, 0.f, 0.f));
	void Set_GrabEnd(bool _bGrabEnd);
	void SetCbsIdle();
	void SetLetMeFlyMonster(std::weak_ptr<Monster> _pMonster);
	void SetFly(bool _ActiveOrNot) { m_IsFly = _ActiveOrNot; }
	void SetPosFireCircle();
	void SetAngle(float _fAngle);
	void SetNoHit(bool _Nohit) { m_Nohit = _Nohit; }
public:
	void CheckAutoRotate();
	bool CheckIsGround();
	void Locking();
	NeroDirection RotateToTargetMonster();
	void RotateToHitMonster(std::weak_ptr<GameObject> _pMonster);
	void NeroMove(NeroDirection _eDir, float _fPower);
	void NeroMoveLerf(NeroDirection _eDir, float _fPower,float _fMax);
	void WireFly();
public:
	void DecreaseJumpCount() { --m_iJumpCount; }
	//ī�޶�
	void DecreaseDistance(float _GoalDis, float _fDeltaTime);
	void IncreaseDistance(float _GoalDis, float _fDeltaTime);
	void IncreaseHp(int _Hp);
	void IncreaseMaxHp(const int _Amount);
	//�׽�Ʈ

public:
	//UI����
	//EX������
	float Get_ExGauge();
	uint32 Get_ExGaugeCount();
	void Add_ExGauge(float ExGauge);
	void Use_ExGauge(const uint32 Count);
	//���Ű�����
	float Get_TDTGauge();
	void AccumulateTDTGauge(const float Amount);
	void ConsumeTDTGauge(const float Speed = 1.f);
	//��ũ ���ھ�
	void AddRankScore(float Score);
public:
	void BuyUpgradedOverture();
	void BuyCbsMiddle() { m_bIsHaveCbsMiddle = true; }
	void BuyCbsLong() { m_bIsHaveCbsLong = true; }
public:
	//�ִϸ��̼� ����
	void  StopAnimation();
	void  ContinueAnimiation();
	bool  IsAnimationEnd();
	void  Set_PlayingTime(float NewTime);
public:
	void ChangeNeroDirection(UINT _NeroDirection);
	void Change_To_MajinMode();
	void ChangeAnimation(const std::string& InitAnimName, const bool  bLoop, const UINT AnimationIndex, const AnimNotify& _Notify = {}, const bool bOverlap = false);
	void ChangeAnimation_Weapon(NeroComponentID _eNeroComID, const std::string& InitAnimName, const bool  bLoop, const AnimNotify& _Notify = {},const bool bOverlap = false);
	void ChangeWeapon(NeroComponentID _iWeaponIndex);
	void ChangeWeaponUI(NeroComponentID _iWeaponIndex);
	void ChangeMeshIndex(UINT _iMeshIndex) { m_iMeshIndex = _iMeshIndex; }
	void ChangeWeaponCollSize(float _fSize = 0.08f);
	void ChangeNewSword(UINT _eAniList, bool _bLoop, bool _Overlap = false);
	//Effect
	void PlayEffect(GAMEOBJECTTAG _eTag,
		const Vector3& Rotation = { 0.f,0.f,0.f }, 
		const float CurRoll = 0.0f,
		const int32 StartSpriteRow = 3, 
		const float PlayTime = 1.f
	, const Vector3& Scale = { 0.004f, 0.004f,0.004f });
	void StopEffect(GAMEOBJECTTAG _eTag);
	void PlayStone(GAMEOBJECTTAG _eTag);
public:

public:
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT Update(const float _fDeltaTime) override;
	virtual UINT LateUpdate(const float _fDeltaTime) override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
public:
	virtual void Hit(BT_INFO _BattleInfo, void* pArg = nullptr) override;
public:
	virtual void	OnCollisionEnter(std::weak_ptr<GameObject> _pOther);
	virtual void	OnCollisionStay(std::weak_ptr<GameObject> _pOther);
public:
	virtual void	OnTriggerEnter(std::weak_ptr<GameObject> _pOther);
	virtual void	OnTriggerExit(std::weak_ptr<GameObject> _pOther);
public:
	// ������ �Լ�....
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);
	void RenderInit();

private:
	void Update_Majin(float _fDeltaTime);

private:
	std::shared_ptr<ENGINE::SkeletonMesh> m_pMesh[MESH_END];
	std::shared_ptr<NeroFSM> m_pFSM;
	std::weak_ptr<RedQueen> m_pRedQueen;
	std::weak_ptr<Nero_LWing>	m_pLWing;
	std::weak_ptr<Nero_RWing>	m_pRWing;
	std::weak_ptr<Buster_Arm>	m_pBusterArm;
	std::weak_ptr<Buster_Arm_Left>	m_pBusterArmLeft;
	std::weak_ptr<Wire_Arm>	m_pWireArm;
	std::weak_ptr<WIngArm_Left> m_pWingArm_Left;
	std::weak_ptr<WingArm_Right> m_pWingArm_Right;
	std::weak_ptr<MainCamera> m_pCamera;
	std::weak_ptr<CapsuleCollider> m_pCollider;
	std::weak_ptr<BtlPanel>			m_pBtlPanel;
	std::weak_ptr<GT_Overture>		m_pOverture;
	std::weak_ptr<GT_Rockman>		m_pRockman;
	std::weak_ptr<Effect>			m_pEffOverture;
	std::weak_ptr<Liquid>			m_pBlood;
	std::weak_ptr<Cbs_Short>		m_pCbsShort;
	std::weak_ptr<Cbs_Middle>		m_pCbsMiddle;
	std::weak_ptr<Cbs_Long>			m_pCbsLong;
	std::weak_ptr<Monster>			m_pTargetMonster;
	std::weak_ptr<Monster>			m_pLetMeFlyMonster;
	std::weak_ptr<AirHike>			m_pAirHike;
	std::weak_ptr<Trail>			m_pTrail;
	std::weak_ptr<IceAge>			m_pIceAge;
	std::weak_ptr<FireCircle>		m_pFireCircle[3];
	std::weak_ptr<CircleWave>		m_pCircleWave;
	std::weak_ptr<CbsTrail>			m_pCbsTrail;
	std::weak_ptr<NewWingSword>		m_pNewWingSword;
	std::weak_ptr<CbsMidTrail>		m_pCbsMidTrail;
	std::weak_ptr<CbsLongTrail>		m_pCbsLongTrail;
	std::weak_ptr<BiAttack>			m_pBiAttack;
	std::weak_ptr<BlitzAttack>		m_pBlitzAttack;
	std::weak_ptr<LongBarrel>		m_pLongBarrel;
	std::weak_ptr<Satellite>		m_pSatellite;
	std::weak_ptr<WhirlWind>		m_pWhirlWind;
	std::weak_ptr<ShockWave>		m_pShockWave;
	std::weak_ptr<StoneDebrisMulti> m_pStone[8];
	enum { SP_RED = 0, SP_GREEN, SP_WHITE, SP_END };	// ShapeParticle
	std::weak_ptr<ShapeParticle>	m_pShapeParticle[SP_END];
	std::weak_ptr<Change>			m_pChange;

	std::weak_ptr<JudgementSword>	m_pJudgementSword;
	std::weak_ptr<JudgementShadow1> m_pJudgementShadow1;
	std::weak_ptr<JudgementShadow2> m_pJudgementShadow2;
	std::weak_ptr<JudgementShadow3> m_pJudgementShadow3;

	UINT	m_iCurAnimationIndex;
	UINT	m_iPreAnimationIndex;
	UINT	m_iCurWeaponIndex;
	UINT	m_iJumpDirIndex;
	UINT	m_iCurDirIndex;
	UINT	m_iPreDirIndex;
	UINT	m_iMeshIndex = 0;

	bool	m_bDebugButton = true;
	UINT	m_iJumpCount = 0;

	float	m_fRedQueenGage = 0.f;
	float	m_fAngle = 0.f;
	float	m_fRotationAngle = 0.f;
	float	m_fFlySpeed = 0.f;
	float   m_fLerfAmount = 0.f;

	bool	m_IsMajin = false;
	bool	m_IsFly = false;
	bool	m_Nohit = false;
	int		m_iDashLoopDir = 1;

	//
	D3DXVECTOR3 vDegree;
	D3DXVECTOR3 vRotationDegree;
	D3DXVECTOR3 vAccumlatonDegree;

	bool	m_bIsHaveCbsMiddle = false;
	bool	m_bIsHaveCbsLong = false;

	/*Coat Test*/
	weak_ptr<NeroCoat> m_pNeroCoat;
};


#endif // Player_h__
