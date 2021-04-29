#ifndef Em0000_h__
#define Em0000_h__

#include "Monster.h"

class Em0000Weapon;
class Nero;
class RedQueen;
class Em0000 final : public Monster
{
private:
	enum Em0000_State
	{
		Attack_1,
		Attack_2,
		Attack_Hard,
		Dead,
		Guard_End,
		Guard_Hit1,
		Guard_Hit2,
		Guard_Loop,
		Guard_Start,
		Hit_Back,
		Hit_Front,
		Hit_L,
		Hit_R,
		Hit_KnocBack,
		Hit_Buster_Start,
		Hit_Buster_Loop,
		Hit_Buster_End,
		Hit_End_Front,
		Hit_End_Back,
		Hit_Air_Start,
		Hit_Air_Loop,
		Hit_Snatch_Start,
		Hit_Snatch_End,
		Hit_Split_Start,
		Hit_Split_End,
		Hit_Air,
		Hit_Air_Snatch_Start,
		Lie_Getup,//뒤로넘어졌을떄 일어나기
		Prone_Getup, // 앞으로넘어졌을때 일어나기
		Move_Front_End,
		Move_Front_Loop,
		Move_Front_Start,
		Step_Back,
		Stun_End,
		Stun_Start,
		idle,
		Downword_Damage,
		Enter_Ground,
		State_END
	};

private:
	explicit Em0000() = default;
	virtual ~Em0000() = default;

	virtual void Free() override;
	virtual std::string GetName() override;

public:
	static Em0000* Create();
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
	// 렌더링 함수....
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);
	void RenderInit();
public:
	virtual void Rotate(const float _fDeltaTime) override;
	virtual void Update_Angle() override;
	void		 Set_Rotate();
	void		 Test();
public:
	virtual void	OnTriggerEnter(std::weak_ptr<GameObject> _pOther);
	virtual void	OnTriggerExit(std::weak_ptr<GameObject> _pOther);

	virtual void	OnCollisionEnter(std::weak_ptr<GameObject> _pOther);
	virtual void	SetGravity(bool _bActiveOrNot);

private:
	//몬스터 상태
	Em0000_State	m_eState =State_END;		
	//TestPlayer 받아옴.
	

	//공격 및 이동 관련
	bool		m_bMove = false;
	float		m_fMoveTime = 0.f;

	bool		m_bAttack = false;	
	float		m_fAttackTime = 0.f;

	//잠깐, 막기!
	bool		m_bGuard = false;
	float		m_fGuardTime = 0.f;

	//전투 시작 테스트 용
	bool		m_bTest = false;
	
	weak_ptr<Em0000Weapon>    m_pWeapon;
	weak_ptr<CapsuleCollider> m_pCollider;


	//////////버스터 용////////////////
	Matrix*								  m_pPlayerBone;
	Matrix								  m_PlayerBone; // 월드
	Matrix								  m_PlayerWorld;
	Matrix								  m_Result;
	Matrix								  m_TempMatrix;
	////////////////

	weak_ptr<class Liquid>				  m_pBlood;
	weak_ptr<class AppearGroundMonster>   m_pAppear;
};

#endif // Em0000_h__