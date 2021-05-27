#ifndef __ENDING_SCENE_H__
#define __ENDING_SCENE_H__
#include "Scene.h"

class EndingScene : public Scene
{
private:
	std::weak_ptr<class Font> Font0{};
	std::weak_ptr<class Font> Font1{};
	float _Tick = 0.f;
private:
	explicit EndingScene() = default;
	virtual ~EndingScene() = default;
	// Scene을(를) 통해 상속됨
	virtual void Free() override;
public:
	static EndingScene* Create();
public:
	// Scene을(를) 통해 상속됨
	virtual HRESULT LoadScene()							override;
	virtual HRESULT Awake()								override;
	virtual HRESULT Start()								override;
	virtual HRESULT Update(const float _fDeltaTime)		override;
	virtual HRESULT LateUpdate(const float _fDeltaTime) override;
};
#endif // !__ENDING_SCENE_H__