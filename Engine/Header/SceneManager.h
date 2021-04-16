#ifndef __SCENE_MANAGER_H__
#define __SCENE_MANAGER_H__
#include "EngineStdafx.h"
BEGIN(ENGINE)
class Scene;
class SceneSystem;
class ENGINE_DLL SceneManager abstract
{
private:
	static std::weak_ptr<SceneSystem> m_pSceneSystem;
public:
	static HRESULT	LoadScene(Scene* const _pScene, const bool _bSceneActivation = true);
	static bool		IsLoaded();
	static void		ActivateScene();
};
END
#endif // !__SCENE_MANAGER_H__