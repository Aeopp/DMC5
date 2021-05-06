#pragma once
#ifndef WingArm_Right_h__
#define WingArm_Right_h__

#include "GameObject.h"
#include "RenderInterface.h"
class Nero;
class WingArm_Right : public GameObject,
	public ENGINE::RenderInterface
{
private:
	explicit WingArm_Right();
	virtual ~WingArm_Right() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
public:
	static WingArm_Right* Create();
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
	virtual std::string GetName() override;
	float Get_PlayingTime();
	float Get_PlayingAccTime();
public:
public:
	void RenderInit();
	void RenderAlphaBlendEffect(const DrawInfo& _Info);
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);
public:
	// RenderInterface을(를) 통해 상속됨
	virtual void RenderReady() override;
	virtual void Editor()override;

private:
	std::shared_ptr<ENGINE::SkeletonMesh> m_pMesh;
	std::weak_ptr<Nero>					  m_pNero;

	Matrix*								m_pParentBoneMat;
	bool								m_bIsRender;
	bool								m_bLoop;

	std::shared_ptr<ENGINE::Texture>		m_NRMRTex{};
	std::shared_ptr<ENGINE::Texture>		m_ATOSTex{};
	std::shared_ptr<ENGINE::Texture>		m_GradationTex{};

	float	m_fAccTime = 0.f;

};



#endif // WingArm_Right_h__