#ifndef Em200_h__
#define Em200_h__

#include "Monster.h"


class RedQueen;
class Em200Hand;
class Nero;
class Em200 final : public Monster
{
private:
	enum Em200_State
	{
		Air_End,
		Air_Loop,
		Air_Start,
		Attack_L,
		Attack_R,
		Attack_Hard_Start,
		Attack_Hard_Loop,
		Attack_Hard_End,
		Dead,
		Hit_Air,
		Hit_Back,
		Hit_End_Front,
		Hit_End_Back,
		Hit_Finish,
		Hit_Front,
		Hit_L,
		Hit_R,
		Hit_KnocBack,
		Hit_Air_Start,
		Hit_Air_Loop,
		Hit_Air_End,
		Walk_Front_End,
		Walk_Front_Loop,
		Walk_Front_Start,
		Idle,
		Idle2,
		Idle3,
		Hit_Buster_Start,
		Hit_Buster_Loop,
		Hit_Buster_End,
		Downword_Down_StandUp,
		Downword_Damage,
		Hit_Snatch_Start,
		Hit_Air_Snatch_Start,
		Hit_Snatch_End,
		Hit_Split_Start,
		Hit_Air_Buster_Start,
		Hit_Air_Buster_Loop,
		Hit_Air_Buster_End,
		Enter_Ground,
		State_END
	};

private:
	explicit Em200() = default;
	virtual ~Em200() = default;

	virtual void Free() override;
	virtual std::string GetName() override;

public:
	static Em200* Create();
public:
	virtual void Fight(const float _fDeltaTime)override;
	virtual void State_Change(const float _fDeltaTime)override;
	virtual void Skill_CoolTime(const float _fDeltaTime)override;

public:
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT Update(const float _fDeltaTime) override;
	virtual UINT LateUpdate(const float _fDeltaTime) override;
	virtual void Editor() override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
public:
	virtual void Hit(BT_INFO _BattleInfo, void* pArg = nullptr) override;
	virtual void Buster(BT_INFO _BattleInfo, void* pArg = nullptr) override;
	virtual void Snatch(BT_INFO _BattleInfo, void* pArg = nullptr) override;
	void		 Air_Hit(BT_INFO _BattleInfo, void* pArg = nullptr);
public:
	//형의 부하가 추가함
	virtual void	OnCollisionEnter(std::weak_ptr<GameObject> _pOther);

	virtual void	OnTriggerEnter(std::weak_ptr<GameObject> _pOther);
	virtual void	OnTriggerExit(std::weak_ptr<GameObject> _pOther);
	// 렌더링 함수....
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);
	void RenderInit();
public:
	virtual void Rotate(const float _fDeltaTime) override;
	virtual void Update_Angle()override;

	void		 Set_Rotate();
	virtual void SetGravity(bool _bActiveOrNot);
private:
	//몬스터 상태
	Em200_State	m_eState =State_END;		

	//공격 및 이동 관련
	bool		m_bMove = false;
	float		m_fMoveTime = 0.f;

	bool		m_bAttack = false;	
	float		m_fAttackTime = 0.f;

	bool		m_bHardAttack = false;
	float		m_fHardAttackTime = 0.f;

	//전투 시작 테스트 용
	bool		m_bTest = false;

	weak_ptr<Em200Hand>		m_pHand[2];
	weak_ptr<CapsuleCollider> m_pCollider;

	float		m_fAngleTime = 0.f;


	//////////버스터 용////////////////
	Matrix*								  m_pPlayerBone;
	Matrix								  m_PlayerWorld;
	Matrix								  m_Result;
	Matrix								  m_TempMatrix;

	bool								  m_bBuster = false;
	////////////////


	/*---이펙트 ---*/
	weak_ptr<class Liquid> m_pBlood;
	weak_ptr<class AppearGroundMonster>   m_pAppear;
	/*----------------*/
};

#endif // Em200_h__