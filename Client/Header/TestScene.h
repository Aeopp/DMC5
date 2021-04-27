#ifndef __TEST_SCENE_H__
#define __TEST_SCENE_H__
#include "Scene.h"
class TestScene : public Scene
{
private:
	physx::PxRigidStatic* pPlane;


	// 임시 트리거 
	std::weak_ptr<class Nero> _Player{};
	std::vector<weak_ptr<GameObject>> Wavefirst{};
	Vector3 WavefirstTriggerPos { -0.47842f , 0.02297f ,-2.57254f};
	bool bfirst = false;
	std::vector<weak_ptr<GameObject>> Wavesecond{};
	Vector3 WavesecondTriggerPos { -1.80262f,0.01168f,1.4464f};
	bool bsecond = false;



	/*vector<weak_ptr<GameObject>> m_vecEm100;
	vector<weak_ptr<GameObject>> m_vecEm0000;*/
	vector<weak_ptr<class Effect>> m_vecQliphothBlock;

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

public:
	void LoadMap();
};
#endif // !__TEST_SCENE_H__