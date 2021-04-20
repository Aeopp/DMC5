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
		Hit_End,
		Hit_Finish,
		Hit_Front,
		Hit_L,
		Hit_R,
		Walk_Front_End,
		Walk_Front_Loop,
		Walk_Front_Start,
		Walk_Left_End,
		Walk_Left_Loop,
		Walk_Left_Start,
		Walk_Right_Stop,
		Walk_Right_Loop,
		Walk_Right_Start,
		idle,
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
	virtual void	OnTriggerEnter(std::weak_ptr<GameObject> _pOther);
	// ������ �Լ�....
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
	//���� ����
	Em100_State	m_eState =State_END;		
	//Player �޾ƿ�.
	std::weak_ptr<Nero>				 m_pPlayer;
	std::weak_ptr<ENGINE::Transform> m_pPlayerTrans;
	std::weak_ptr<RedQueen>			 m_pRedQueen;

	//���� �� �̵� ����
	bool		m_bMove = false;
	float		m_fMoveTime = 0.f;

	bool		m_bAttack = false;	
	float		m_fAttackTime = 0.f;

	bool		m_bHardAttack = false;
	float		m_fHardAttackTime = 0.f;

	//���� ���� �׽�Ʈ ��
	bool		m_bTest = false;

	weak_ptr<Em100Hand>		m_pHand[2];
	weak_ptr<CapsuleCollider> m_pCollider;

	float		m_fAngleTime = 0.f;
};

#endif // Em100_h__