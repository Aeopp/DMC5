#ifndef __M01_HOTEL_S02_H__
#define __M01_HOTEL_S02_H__
#include "Scene.h"

class Hotel_S02 : public Scene
{
private:
	std::weak_ptr<class Nero> _Player{};
	std::vector<std::weak_ptr<class MakaiButterfly>> _MakaiButterflyVec{};
	bool _LateInit = false;
private:
	explicit Hotel_S02();
	virtual ~Hotel_S02() = default;
	// Scene을(를) 통해 상속됨
	virtual void Free() override;
public:
	static Hotel_S02* Create();
public:
	// Scene을(를) 통해 상속됨
	virtual HRESULT LoadScene()							override;
	virtual HRESULT Awake()								override;
	virtual HRESULT Start()								override;
	virtual HRESULT Update(const float _fDeltaTime)		override;
	virtual HRESULT LateUpdate(const float _fDeltaTime) override;

private:
	void LoadObjects(const std::filesystem::path& path);
	void RenderDataSetUp(const bool bTest);
	void TriggerSetUp();
	void TriggerWallSmash();
	void TriggerFirstButterFlyMeet();
	void TriggerNextScene();

	void LateInit();
};
#endif // !__M01_HOTEL_S02_H__