#ifndef JudgementShadow3_h__
#define JudgementShadow3_h__

#include "GameObject.h"
#include "RenderInterface.h"
#include "DissolveInfo.h"

class Nero;
class JudgementShadow3 :
	public GameObject,
	public ENGINE::RenderInterface
{
private:
    explicit JudgementShadow3();
    virtual ~JudgementShadow3() = default;
    // GameObject을(를) 통해 상속됨
    virtual void Free() override;
public:
    static JudgementShadow3* Create();
    virtual HRESULT Ready() override;
    virtual HRESULT Awake() override;
    virtual HRESULT Start() override;
    virtual UINT Update(const float _fDeltaTime) override;
    virtual UINT LateUpdate(const float _fDeltaTime) override;
    virtual void OnEnable() override;
    virtual void OnDisable() override;
public:
	void ChangeAnimation(const std::string& InitAnimName, const bool  bLoop, const AnimNotify& _Notify = {});
public:
	virtual void RenderReady() override;
	virtual void Editor()override;
    virtual std::string GetName() override;
public:
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);
    void RenderAlphaBlendEffect(const DrawInfo& _Info);
	void RenderInit();
private:
    std::shared_ptr<ENGINE::SkeletonMesh> m_pMesh;
    std::weak_ptr<Nero>					  m_pNero;
    DissolveInfo                          m_DissolveInfo;
};



#endif // JudgementShadow3_h__