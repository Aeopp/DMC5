#ifndef HotelBrokenFloor_h__
#define HotelBrokenFloor_h__

#include "GameObject.h"
#include "RenderInterface.h"
class HotelBrokenFloor : public GameObject,
	public ENGINE::RenderInterface
{
private:
	explicit HotelBrokenFloor();
	virtual ~HotelBrokenFloor() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
public:
	static HotelBrokenFloor* Create();
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
	virtual void	OnTriggerEnter(std::weak_ptr<GameObject> _pOther);
	virtual void	OnTriggerExit(std::weak_ptr<GameObject> _pOther);
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
	std::weak_ptr<BoxCollider> m_pCollider;

	UINT m_iCollCount = 0;

	std::weak_ptr<class Smoke> m_pSmoke[2];
};

#endif // HotelBrokenFloor_h__