#ifndef __TEST_SCENE_H__
#define __TEST_SCENE_H__
#include "Scene.h"
class TestScene : public Scene
{
private:
	physx::PxRigidStatic* pPlane;

	vector<weak_ptr<GameObject>> m_vecEm100;
	vector<weak_ptr<GameObject>> m_vecEm0000;
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

public:
	void LoadMap();
};
#endif // !__TEST_SCENE_H__