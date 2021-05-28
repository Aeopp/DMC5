#ifndef JudgementShadow1_h__
#define JudgementShadow1_h__

#include "GameObject.h"
#include "RenderInterface.h"
class Nero;
class JudgementShadow1 :
	public GameObject,
	public ENGINE::RenderInterface
{
private:
    explicit JudgementShadow1();
    virtual ~JudgementShadow1() = default;
    // GameObject을(를) 통해 상속됨
    virtual void Free() override;
public:
    static JudgementShadow1* Create();
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
	void RenderInit();
private:
    std::shared_ptr<ENGINE::SkeletonMesh> m_pMesh;
    std::weak_ptr<Nero>					  m_pNero;

};



#endif // JudgementShadow1_h__