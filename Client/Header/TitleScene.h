#ifndef __TITLE_SCENE_H__
#define __TITLE_SCENE_H__
#include "Scene.h"

class TitleScene : public Scene
{
private:
	std::weak_ptr<class TitlePanel> _TitlePanel{};

	bool _LoadNextScene = false;

private:
	explicit TitleScene() = default;
	virtual ~TitleScene() = default;
	// Scene을(를) 통해 상속됨
	virtual void Free() override;
public:
	static TitleScene* Create();
public:
	// Scene을(를) 통해 상속됨
	virtual HRESULT LoadScene()							override;
	virtual HRESULT Awake()								override;
	virtual HRESULT Start()								override;
	virtual HRESULT Update(const float _fDeltaTime)		override;
	virtual HRESULT LateUpdate(const float _fDeltaTime) override;

};
#endif // !__TITLE_SCENE_H__