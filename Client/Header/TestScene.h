#ifndef __TEST_SCENE_H__
#define __TEST_SCENE_H__
#include "Scene.h"
class TestScene : public Scene
{
private:
	explicit TestScene();
	virtual ~TestScene() = default;
	// Scene을(를) 통해 상속됨
	virtual void Free() override;
public:
	static TestScene* Create();
public:
	// Scene을(를) 통해 상속됨
	virtual HRESULT LoadScene()							override;
	virtual HRESULT Awake()								override;
	virtual HRESULT Start()								override;
	virtual HRESULT Update(const float _fDeltaTime)		override;
	virtual HRESULT LateUpdate(const float _fDeltaTime) override;
};
#endif // !__TEST_SCENE_H__