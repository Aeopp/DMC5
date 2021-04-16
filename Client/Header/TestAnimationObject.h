#ifndef _TESTANIMATIONOBJECT_H_
#define _TESTANIMATIONOBJECT_H_
#include "GameObject.h"
#include "RenderInterface.h"
#include "SkeletonMesh.h"

class TestAnimationObject : public ENGINE::GameObject ,
						    public ENGINE::RenderInterface				
{
private:
	std::shared_ptr<ENGINE::SkeletonMesh> _SkeletonMesh{};
private:
	explicit TestAnimationObject() = default;
	virtual ~TestAnimationObject() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
public:
	static TestAnimationObject* Create();
public:
	virtual void   RenderReady()override;
	void RenderInit();
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);
public:
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT    Update(const float _fDeltaTime) override;
	virtual UINT    LateUpdate(const float _fDeltaTime) override;
	virtual void    Editor()override;
	virtual void	OnEnable() override;
	virtual void    OnDisable() override;
};
#endif //