#ifndef Cbs_Long_h__
#define Cbs_Long_h__

#include "Unit.h"
#include "RenderInterface.h"
class Nero;
class Cbs_Long : public Unit,
	public ENGINE::RenderInterface
{
private:
	explicit Cbs_Long();
	virtual ~Cbs_Long() = default;
	// Unit을(를) 통해 상속됨
	virtual void Free() override;
public:
	static Cbs_Long* Create();
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT Update(const float _fDeltaTime) override;
	virtual UINT LateUpdate(const float _fDeltaTime) override;
	virtual void OnEnable() override;
virtual void OnDisable() override; public:
	//충돌
	virtual void OnTriggerEnter(std::weak_ptr<GameObject> _pOther) override;
	virtual void OnTriggerExit(std::weak_ptr<GameObject> _pOther) override;
	virtual void Hit(BT_INFO _BattleInfo, void* pArg = nullptr) override; 
public:
	virtual std::string GetName() override;
public:
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
	std::weak_ptr<CapsuleCollider> m_pCollider;
	Matrix* m_pParentMat;
};



#endif // Cbs_Long_h__