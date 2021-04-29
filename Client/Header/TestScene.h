#ifndef __TEST_SCENE_H__
#define __TEST_SCENE_H__
#include "Scene.h"
class TestScene : public Scene
{
private:
	physx::PxRigidStatic* pPlane;


	// �ӽ� Ʈ���� 
	std::weak_ptr<class Nero> _Player{};
	std::weak_ptr<class Em1000> _Em1000{};
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