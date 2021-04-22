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
class Wire_Arm;
class WIngArm_Left;
class WingArm_Right;
class MainCamera;
class BtlPanel;
class GT_Overture;
class GT_Rockman;
class Monster;
class Effect;
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
		ANI_END
	};

	enum WeaponList
	{
		RQ, // 검
		Cbs // 삼절곤

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
		Dir_Right
	};

	enum EffDircetion
	{
		EffDir_Front,
		EffDir_Up,
		EffDir_Down
	};

private:
	explicit Nero();
	virtual ~Nero() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
public:

	static Nero* Create();
public:
	virtual void RenderReady() override;
	virtual void Editor()override;
public:
	/// <For RedQueen>
	void Set_RQ_State(UINT _StateIndex);
	void Set_RQ_AttType(ATTACKTYPE _eAttDir);
	void Set_RQ_Coll(bool _ActiveOrNot);
	void Set_PlayingTime(float NewTime);

public:
	// <For Overture Eff>
	void CreateOvertureEff(EffDircetion eDir = EffDir_Front);

public:
	std::weak_ptr<NeroFSM> GetFsm() { return m_pFSM; }
	std::list<std::weak_ptr<Monster>> GetAllMonster();
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
	bool Get_IsMajinMode() { return m_IsMajin; }
public:
	void Reset_JumpCount() { m_iJumpCount = 1; }
	void Reset_RotationAngle() { m_fRotationAngle = 0.f; }
	void Reset_RootRotation() { vAccumlatonDegree = { 0.f,0.f,0.f }; }
	void Set_JumpDir(UINT _iJumpDir) { m_iJumpDirIndex = _iJumpDir; }
	void SetActive_Wings(bool ActiveOrNot);
	void SetActive_Wing_Left(bool ActiveOrNot);
	void SetActive_Wing_Right(bool ActiveOrNot);
	void SetActive_Buster_Arm(bool ActiveOrNot);
	void SetActive_Wire_Arm(bool ActiveOrNot);
	void SetActive_WingArm_Right(bool ActiveOrNot);
	void SetActive_WingArm_Left(bool ActiveOrNot);
	void SetAngleFromCamera(float _fAddAngle = 0.f);
	void SetRotationAngle(float _fAngle) { m_fRotationAngle += _fAngle; }
	void SetColl_Monsters(bool _AcitveOrNot);
public:
	void CheckAutoRotate();
public:
	void DecreaseJumpCount() { --m_iJumpCount; }
	//카메라
	void DecreaseDistance(float _GoalDis, float _fDeltaTime);
	void IncreaseDistance(float _GoalDis, float _fDeltaTime);
	//테스트

public:
	//UI관련
	//EX게이지
	float Get_ExGauge();
	uint32 Get_ExGaugeCount();
	void Add_ExGauge(float ExGauge);
	void Use_ExGauge(const uint32 Count);
	//변신게이지
	float Get_TDTGauge();
	void AccumulateTDTGauge(const float Amount);
	void ConsumeTDTGauge(const float Speed = 1.f);
	//랭크 스코어
	void AddRankScore(float Score);
public:
	//애니메이션 관련
	void  StopAnimation();
	void  ContinueAnimiation();
	bool  IsAnimationEnd();
public:
	void ChangeNeroDirection(UINT _NeroDirection);
	void Change_To_MajinMode() { m_IsMajin = true; }
	void ChangeAnimation(const std::string& InitAnimName, const bool  bLoop, const UINT AnimationIndex, const AnimNotify& _Notify = {});
	void ChangeAnimationIndex(const UINT AnimationIndex);
	void ChangeWeapon(UINT _iWeaponIndex);
	void Change_BusterArm_Animation(const std::string& InitAnimName, const bool  bLoop, const AnimNotify& _Notify = {});
	void Change_WireArm_Animation(const std::string& InitAnimName, const bool  bLoop, const AnimNotify& _Notify = {});
	void Change_WingArm_Left_Animation(const std::string& InitAnimName, const bool  bLoop, const AnimNotify& _Notify = {});
	void Change_WingArm_Right_Animation(const std::string& InitAnimName, const bool  bLoop, const AnimNotify& _Notify = {});
	void Change_Overture_Animation(const std::string& InitAnimName, const bool  bLoop, const AnimNotify& _Notify = {});
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
	virtual void	OnTriggerEnter(std::weak_ptr<GameObject> _pOther);
	virtual void	OnTriggerExit(std::weak_ptr<GameObject> _pOther);
public:
	// 렌더링 함수....
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);
	void RenderInit();

private:
	void Update_Majin(float _fDeltaTime);

private:
	std::shared_ptr<ENGINE::SkeletonMesh> m_pMesh;
	std::shared_ptr<NeroFSM> m_pFSM;
	std::weak_ptr<RedQueen> m_pRedQueen;
	std::weak_ptr<Nero_LWing>	m_pLWing;
	std::weak_ptr<Nero_RWing>	m_pRWing;
	std::weak_ptr<Buster_Arm>	m_pBusterArm;
	std::weak_ptr<Wire_Arm>	m_pWireArm;
	std::weak_ptr<WIngArm_Left> m_pWingArm_Left;
	std::weak_ptr<WingArm_Right> m_pWingArm_Right;
	std::weak_ptr<MainCamera> m_pCamera;
	std::weak_ptr<CapsuleCollider> m_pCollider;
	std::weak_ptr<BtlPanel>			m_pBtlPanel;
	std::weak_ptr<GT_Overture>		m_pOverture;
	std::weak_ptr<GT_Rockman>		m_pRockman;
	std::weak_ptr<Effect>			m_pEffOverture;

	UINT	m_iCurAnimationIndex;
	UINT	m_iPreAnimationIndex;
	UINT	m_iCurWeaponIndex;
	UINT	m_iJumpDirIndex;
	UINT	m_iCurDirIndex;
	UINT	m_iPreDirIndex;

	bool	m_bDebugButton = true;
	UINT	m_iJumpCount = 0;

	float	m_fRedQueenGage = 0.f;
	float	m_fAngle = 0.f;
	float	m_fRotationAngle = 0.f;

	bool	m_IsMajin = false;

	//
	D3DXVECTOR3 vDegree;
	D3DXVECTOR3 vRotationDegree;
	D3DXVECTOR3 vAccumlatonDegree;

	
};


#endif // Player_h__
