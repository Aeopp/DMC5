#ifndef __LOGO_SCENE_H__
#define __LOGO_SCENE_H__
#include "Scene.h"

class LogoScene : public Scene
{
private:
	std::weak_ptr<class LogoPanel> _LogoPanel{};
	
	bool _LoadNextScene = false; 

private:
	explicit LogoScene() = default;
	virtual ~LogoScene() = default;
	// Scene을(를) 통해 상속됨
	virtual void Free() override;
public:
	static LogoScene* Create();
public:
	// Scene을(를) 통해 상속됨
	virtual HRESULT LoadScene()							override;
	virtual HRESULT Awake()								override;
	virtual HRESULT Start()								override;
	virtual HRESULT Update(const float _fDeltaTime)		override;
	virtual HRESULT LateUpdate(const float _fDeltaTime) override;
};
#endif // !__LOGO_SCENE_H__