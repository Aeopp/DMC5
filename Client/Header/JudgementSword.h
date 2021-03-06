#ifndef JudgementSword_h__
#define JudgementSword_h__

#include "GameObject.h"
#include "RenderInterface.h"
class Nero;
class JudgementSwordCollider;
class JudgementSword :
	public GameObject,
	public ENGINE::RenderInterface
{
private:
    explicit JudgementSword();
    virtual ~JudgementSword() = default;
    // GameObject을(를) 통해 상속됨
    virtual void Free() override;
public:
    static JudgementSword* Create();
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
public:
    /* 2021 05 08 트레일 본 위치를 구하기 위해서 이호준 RedQueen 의 함수와 똑같이 만들었음 !! */
    Matrix* Get_BoneMatrixPtr(std::string _BoneName);
private:
    std::shared_ptr<ENGINE::SkeletonMesh> m_pMesh;
    std::weak_ptr<Nero>					  m_pNero;
    std::vector<std::weak_ptr<JudgementSwordCollider>> m_vecColliders;

    Matrix* m_pParentBoneMat;

    std::shared_ptr<ENGINE::Texture>		m_NRMRTex{};
    std::shared_ptr<ENGINE::Texture>		m_ATOSTex{};
    std::shared_ptr<ENGINE::Texture>		m_GradationTex{};

    float	m_fAccTime = 0.f;

};



#endif // JudgementSword_h__