#ifndef __SCENE_SYSTEM_H__
#define __SCENE_SYSTEM_H__
#include "Object.h"
BEGIN(ENGINE)
class Scene;
class SceneSystem final : public Object
{
	DECLARE_SINGLETON(SceneSystem)
private:
	enum LOADINGSTATE { NONE, LOADING, FINISH };
private:
	std::shared_ptr<Scene>	m_pCurrentScene;
	std::shared_ptr<Scene>	m_pNextScene;
	LOADINGSTATE			m_eLoadingState;
	HANDLE					m_hThread;
	bool					m_bSceneActivation;
private:
	explicit SceneSystem();
	virtual	~SceneSystem() = default;
	// Object을(를) 통해 상속됨
	virtual void Free() override;
public:
	HRESULT ReadySceneSystem();
	HRESULT	UpdateSceneSystem(const float _fDeltaTime);
	void    EditUpdateSceneSystem();
public:
	HRESULT LoadScene(Scene* const _pScene, const bool _bSceneActivation = true);
	bool	IsLoaded();
	void	ActivateScene();
private:
	void	CheckThread();
	void	SwitchScene();
private:
	static unsigned int CALLBACK LoadingThread(void* _pArg);
};
END
#endif // !__SCENE_SYSTEM_H__