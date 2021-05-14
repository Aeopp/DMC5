#ifndef HotelAnimationWall_h__
#define HotelAnimationWall_h__

#include "GameObject.h"
#include "RenderInterface.h"
class HotelAnimationWall :	public GameObject,
	public ENGINE::RenderInterface
{
private:
	explicit HotelAnimationWall();
	virtual ~HotelAnimationWall() = default;
	// GameObject��(��) ���� ��ӵ�
	virtual void Free() override;
public:
	static HotelAnimationWall* Create();
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
	// RenderInterface��(��) ���� ��ӵ�
	virtual void RenderReady() override;
	void RenderGBufferSK(const DrawInfo& _Info);
	void RenderShadowSK(const DrawInfo& _Info);
	void RenderDebugBone(const DrawInfo& _Info);
	void RenderDebugSK(const DrawInfo& _Info);
	void RenderInit();
private:
	std::shared_ptr<ENGINE::SkeletonMesh> m_pMesh;
};



#endif // HotelAnimationWall_h__