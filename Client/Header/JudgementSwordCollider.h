#ifndef JudgementSwordCollider_h__
#define JudgementSwordCollider_h__

#include "Unit.h"
#include "RenderInterface.h"
class Nero;
class JudgementSwordCollider :
	public Unit,
	public ENGINE::RenderInterface
{
private:
    explicit JudgementSwordCollider();
    virtual ~JudgementSwordCollider() = default;
    // GameObject을(를) 통해 상속됨
    virtual void Free() override;
public:
    static JudgementSwordCollider* Create();
    virtual HRESULT Ready() override;
    virtual HRESULT Awake() override;
    virtual HRESULT Start() override;
    virtual UINT Update(const float _fDeltaTime) override;
    virtual UINT LateUpdate(const float _fDeltaTime) override;
    virtual void OnEnable() override;
    virtual void OnDisable() override;
public:
    void SetParentBoneMat(Matrix* _pBoneMat) { m_pParentBoneMat = _pBoneMat; }
public:
	virtual void RenderReady() override;
	virtual void Editor()override;
    virtual std::string GetName() override;
    virtual void Hit(BT_INFO _BattleInfo, void* pArg = nullptr) override;
public:
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);
    void RenderAlphaBlendEffect(const DrawInfo& _Info);
	void RenderInit();
private:
    std::weak_ptr<Nero> m_pNero;
    Matrix* m_pParentBoneMat;
    std::weak_ptr<CapsuleCollider>	m_pCollider;
};



#endif // JudgementSwordCollider_h__