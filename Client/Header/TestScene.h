#ifndef __TEST_SCENE_H__
#define __TEST_SCENE_H__
#include "Scene.h"

class TestScene : public Scene
{
private:
	physx::PxRigidStatic* pPlane;
	std::weak_ptr<class Nero> _Player{};
	std::weak_ptr<class Camera> _Camera{};
	std::weak_ptr<class MainCamera> _MainCamera{};
	std::weak_ptr<class BtlPanel> _BtlPanel{};
	std::weak_ptr<class ShopPanel> _ShopPanel{};
	std::weak_ptr<class FadeOut> _ShopFadeOut{};
private:
	explicit TestScene();
	virtual ~TestScene() = default;
	// Scene��(��) ���� ��ӵ�
	virtual void Free() override;
public:
	static TestScene* Create();
public:
	// Scene��(��) ���� ��ӵ�
	virtual HRESULT LoadScene()							override;
	virtual HRESULT Awake()								override;
	virtual HRESULT Start()								override;
	virtual HRESULT Update(const float _fDeltaTime)		override;
	virtual HRESULT LateUpdate(const float _fDeltaTime) override;
private:
	void LoadMap();
	void RenderDataSetUp(const bool bTestLight);
	void MonsterWaveTriggerSetUp();
	void TriggerSetUp();
	void ApplyShopUpgradeDesc();
	void CheckShopAvailable();
};
#endif // !__TEST_SCENE_H__