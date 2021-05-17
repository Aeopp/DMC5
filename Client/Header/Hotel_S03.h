#ifndef __M01_HOTEL_S03_H__
#define __M01_HOTEL_S03_H__
#include "Scene.h"

class Hotel_S03 : public Scene
{
private:
	std::weak_ptr<class Nero> _Player{};
	bool _LateInit = false;
private:
	explicit Hotel_S03();
	virtual ~Hotel_S03() = default;
	// Scene��(��) ���� ��ӵ�
	virtual void Free() override;
public:
	static Hotel_S03* Create();
public:
	// Scene��(��) ���� ��ӵ�
	virtual HRESULT LoadScene()							override;
	virtual HRESULT Awake()								override;
	virtual HRESULT Start()								override;
	virtual HRESULT Update(const float _fDeltaTime)		override;
	virtual HRESULT LateUpdate(const float _fDeltaTime) override;
private:
	void LoadObjects(const std::filesystem::path& path);
	void RenderDataSetUp(const  bool bTest);
	void TriggerSetUp();
	void TriggerUpGround();
	void TriggerNextScene();
	void LateInit();
};
#endif // !__M01_HOTEL_S03_H__