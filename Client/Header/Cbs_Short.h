#ifndef Cbs_Short_h__
#define Cbs_Short_h__

#include "Unit.h"
#include "RenderInterface.h"
class Nero;
class Cbs_Short : public Unit,
	public ENGINE::RenderInterface
{
private:
	explicit Cbs_Short();
	virtual ~Cbs_Short() = default;
	// Unit을(를) 통해 상속됨
	virtual void Free() override;
public:
	static Cbs_Short* Create();
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT Update(const float _fDeltaTime) override;
	virtual UINT LateUpdate(const float _fDeltaTime) override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
public:
	//충돌
	virtual void OnTriggerEnter(std::weak_ptr<GameObject> _pOther) override;
	virtual void OnTriggerExit(std::weak_ptr<GameObject> _pOther) override;
	virtual void Hit(BT_INFO _BattleInfo, void* pArg = nullptr) override;
public:
	void ChangeAnimation(const std::string& InitAnimName, const bool  bLoop, const AnimNotify& _Notify = {},const bool _bOverlap = false);
	void StopAnimation() { m_pMesh->StopAnimation(); }
	void ContinueAnimation() { m_pMesh->ContinueAnimation(); }
public:
	virtual std::string GetName() override;
	float Get_PlayingTime();
	float Get_PlayingAccTime();
public:
	void	SetWeaponState(UINT _StateIndex) { m_iStateIndex = _StateIndex; }
	void	SetAttType(ATTACKTYPE _eAttDir) { m_BattleInfo.eAttackType = _eAttDir; }

public:
	// RenderInterface을(를) 통해 상속됨
	virtual void RenderReady() override;
	virtual void Editor()override;

	// 렌더링 함수 ..
	void RenderInit();
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);

private:
	std::shared_ptr<ENGINE::SkeletonMesh> m_pMesh;
	std::weak_ptr<Nero>					  m_pNero;
	std::weak_ptr<SphereCollider> m_pCollider;

	std::vector<Matrix*>					m_vecParentMat;
	UINT									m_iStateIndex = 0;
	Vector3					m_vTest;
};



#endif // Cbs_Short_h__