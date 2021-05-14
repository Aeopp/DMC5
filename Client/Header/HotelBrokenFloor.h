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
	// GameObject��(��) ���� ��ӵ�
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



#endif // HotelBrokenFloor_h__