#ifndef RedQueen_h__
#define RedQueen_h__

#include "Unit.h"
#include "RenderInterface.h"
class Nero;
class Glint;
class RedQueen : public Unit,
	public ENGINE::RenderInterface
{
private:
	explicit RedQueen();
	virtual ~RedQueen() = default;
	// GameObject¿ª(∏¶) ≈Î«ÿ ªÛº”µ 
	virtual void Free() override;
public:
	static RedQueen* Create();
public:
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT Update(const float _fDeltaTime) override;
	virtual UINT LateUpdate(const float _fDeltaTime) override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
public:
	virtual void	OnTriggerEnter(std::weak_ptr<GameObject> _pOther);
	virtual void	OnTriggerExit(std::weak_ptr<GameObject> _pOther);
public:
	virtual void Hit(BT_INFO _BattleInfo, void* pArg = nullptr) override;
	virtual std::string GetName() override;
public:
	void	SetWeaponState(UINT _StateIndex) { m_iStateIndex = _StateIndex; }
	void	SetAttType(ATTACKTYPE _eAttDir) { m_BattleInfo.eAttackType = _eAttDir; }
public:
	// RenderInterface¿ª(∏¶) ≈Î«ÿ ªÛº”µ 
	virtual void RenderReady() override;
	// ∑ª¥ı∏µ «‘ºˆ ...
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);
	void RenderInit();

	virtual void Editor()override;

	Matrix* Get_BoneMatrixPtr(std::string _BoneName);

private:
	std::weak_ptr<Nero>					m_pNero;
	std::shared_ptr<ENGINE::SkeletonMesh> m_pMesh;
	std::weak_ptr<CapsuleCollider> m_pCollider;
	std::weak_ptr<Glint>			m_pGlint; // ∂ß∑»¿ª∂ß π›¬¶¿Ã¥¬ ¿Ã∆Â∆Æ


	std::vector<Matrix*>					m_vecParentMat;
	UINT								m_iStateIndex = 0;
};



#endif // RedQueen_h__