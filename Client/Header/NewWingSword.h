#ifndef NewWingSword_h__
#define NewWingSword_h__

#include "GameObject.h"
#include "RenderInterface.h"
class Nero;
class NewWingSword : public GameObject,
	public ENGINE::RenderInterface
{
private:
    explicit NewWingSword();
    virtual ~NewWingSword() = default;
    // GameObject을(를) 통해 상속됨
    virtual void Free() override;
public:
    static NewWingSword* Create();
    virtual HRESULT Ready() override;
    virtual HRESULT Awake() override;
    virtual HRESULT Start() override;
    virtual UINT Update(const float _fDeltaTime) override;
    virtual UINT LateUpdate(const float _fDeltaTime) override;
    virtual void OnEnable() override;
    virtual void OnDisable() override;
    virtual std::string GetName() override; 
public:
    void ChangeAnimation(UINT _eAniList, bool _bLoop, bool _Overlap = false);
public:
	virtual void RenderReady() override;
	virtual void Editor()override;
public:
	// ������ �Լ�....
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);
    void RenderAlphaBlendEffect(const DrawInfo& _Info);
	void RenderInit();

private:
    std::weak_ptr<Nero>     m_pNero;
    std::shared_ptr<ENGINE::SkeletonMesh> m_pMesh[WingSword_End];

    UINT	m_iMeshIndex = 0;
    Matrix* m_pParentBoneMat;

    std::shared_ptr<ENGINE::Texture>		m_NRMRTex{};
    std::shared_ptr<ENGINE::Texture>		m_ATOSTex{};
    std::shared_ptr<ENGINE::Texture>		m_GradationTex{};

    float	m_fAccTime = 0.f;

};



#endif // NewWingSword_h__