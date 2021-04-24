#ifndef Em100_h__
#define Em100_h__

#include "Monster.h"


class RedQueen;
class Em100Hand;
class Nero;
class Em100 final : public Monster
{
private:
	enum Em100_State
	{
		Air_End,
		Air_Loop,
		Air_Start,
		Attack_A,
		Attack_D,
		Attack_Hard,
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
		Walk_Left_End,
		Walk_Left_Loop,
		Walk_Left_Start,
		Walk_Right_Stop,
		Walk_Right_Loop,
		Walk_Right_Start,
		Idle,
		Idle2,
		Idle3,
		Idle4,		
		Hit_Buster_Start,
		Hit_Buster_Loop,
		Hit_Buster_End,
		Downword_Down_StandUp,
		Downword_Damage,
		State_END
	};

private:
	explicit Em100() = default;
	virtual ~Em100() = default;

	virtual void Free() override;
	virtual std::string GetName() override;

public:
	static Em100* Create();
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
public:
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
	virtual void Update_Angle(const float _fDeltaTime, bool _bTest = false);
	virtual void Update_Angle()override;
private:
	//몬스터 상태
	Em100_State	m_eState =State_END;		
	//Player 받아옴.
	std::weak_ptr<Nero>					m_pPlayer;
	std::weak_ptr<ENGINE::Transform>	m_pPlayerTrans;
	std::weak_ptr<RedQueen>				m_pRedQueen;

	//공격 및 이동 관련
	bool		m_bMove = false;
	float		m_fMoveTime = 0.f;

	bool		m_bAttack = false;	
	float		m_fAttackTime = 0.f;

	bool		m_bHardAttack = false;
	float		m_fHardAttackTime = 0.f;

	//전투 시작 테스트 용
	bool		m_bTest = false;

	weak_ptr<Em100Hand>		m_pHand[2];
	weak_ptr<CapsuleCollider> m_pCollider;

	float		m_fAngleTime = 0.f;


	//////////버스터 용////////////////
	Matrix*								  m_pPlayerBone;
	Matrix								  m_PlayerWorld;
	Matrix								  m_Result;
	Matrix								  m_TempMatrix;
	////////////////

};

#endif // Em100_h__