#ifndef __M01_HOTEL_S02_H__
#define __M01_HOTEL_S02_H__
#include "Scene.h"

class Trigger;

class Hotel_S02 : public Scene
{
private:
	std::weak_ptr<class Nero> _Player{};
	std::weak_ptr<class BtlPanel> _BtlPanel{};
	std::weak_ptr<class MainCamera> _MainCamera{};
	std::vector<std::weak_ptr<class MakaiButterfly>> _MakaiButterflyVec{};
	std::weak_ptr<class SecretVision> _SecretVision{};
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
	void LoadCollObjects(const std::filesystem::path& path);
	void LoadBreakablebjects(const std::filesystem::path& path);
private:
	void LateInit();
	void RenderDataSetUp(const bool bTest);
	void BgmPlay();

	void TriggerSetUp();
	void TriggerWallSmash();
	void TriggerFirstButterFlyMeetCamera(const std::weak_ptr<class Trigger>& _NextTrigger);
	std::weak_ptr<Trigger> TriggerFirstButterFlyMeet();
	void TriggerPuzzleStart();
	void TriggerLastRoomBattle(const std::weak_ptr<Trigger>& _NextSceneTrigger);
	std::weak_ptr<Trigger> TriggerNextScene();
};
#endif // !__M01_HOTEL_S02_H__