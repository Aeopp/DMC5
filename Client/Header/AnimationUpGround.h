#ifndef AnimationUpGround_h__
#define AnimationUpGround_h__

#include "GameObject.h"
#include "RenderInterface.h"
class AnimationUpGround : public GameObject,
	public ENGINE::RenderInterface
{
private:
	explicit AnimationUpGround();
	virtual ~AnimationUpGround() = default;

	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
public:
	static AnimationUpGround* Create();
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT Update(const float _fDeltaTime) override;
	virtual UINT LateUpdate(const float _fDeltaTime) override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	virtual void Editor()override;
	virtual std::string GetName() override;
public:
	void ContinueAnimation() { m_pMesh->ContinueAnimation(); }
public:
	// RenderInterface을(를) 통해 상속됨
	virtual void RenderReady() override;
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);
	void RenderInit();
private:
	std::shared_ptr<ENGINE::SkeletonMesh> m_pMesh;
};



#endif // AnimationUpGround_h__