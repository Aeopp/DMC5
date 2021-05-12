#ifndef __LOADING_SCENE_H__
#define __LOADING_SCENE_H__
#include "Scene.h"

class LoadingScene : public Scene
{
private:
	SCENE_ID _NextSceneID = SCENE_ID::SCENE_ID_END;

	std::weak_ptr<class LoadingPanel> _LoadingPanel{};

	bool _LoadNextScene = false;
	float _CheckLoadingTick = 0.f;

private:
	explicit LoadingScene() = default;
	virtual ~LoadingScene() = default;
	// Scene을(를) 통해 상속됨
	virtual void Free() override;
public:
	static LoadingScene* Create(SCENE_ID NextSceneID);
public:
	// Scene을(를) 통해 상속됨
	virtual HRESULT LoadScene()							override;
	virtual HRESULT Awake()								override;
	virtual HRESULT Start()								override;
	virtual HRESULT Update(const float _fDeltaTime)		override;
	virtual HRESULT LateUpdate(const float _fDeltaTime) override;

};
#endif // !__LOADING_SCENE_H__