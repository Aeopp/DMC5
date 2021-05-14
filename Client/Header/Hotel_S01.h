#ifndef __M01_HOTEL_S01_H__
#define __M01_HOTEL_S01_H__
#include "Scene.h"

class Trigger;

class Hotel_S01 : public Scene
{
private:
	std::weak_ptr<class Nero> _Player{};
	bool _LateInit = false;
private:
	explicit Hotel_S01();
	virtual ~Hotel_S01() = default;
	// Scene을(를) 통해 상속됨
	virtual void Free() override;
public:
	static Hotel_S01* Create();
public:
	// Scene을(를) 통해 상속됨
	virtual HRESULT LoadScene()							override;
	virtual HRESULT Awake()								override;
	virtual HRESULT Start()								override;
	virtual HRESULT Update(const float _fDeltaTime)		override;
	virtual HRESULT LateUpdate(const float _fDeltaTime) override;
private:	
	void RenderDataSetUp(const bool bTest);
	void LoadObjects(const std::filesystem::path& path, const bool _bAni = false);
	void TriggerSetUp();
	void Trigger1st();
	void Trigger2nd();
	void Trigger3rd();
	void Trigger4st();

	void LateInit();
};
#endif // !__M01_HOTEL_S01_H__