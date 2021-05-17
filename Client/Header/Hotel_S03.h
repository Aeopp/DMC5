#ifndef __M01_HOTEL_S03_H__
#define __M01_HOTEL_S03_H__
#include "Scene.h"

class Trigger;
class Hotel_S03 : public Scene
{
private:
	std::weak_ptr<class Nero> _Player{};
	std::weak_ptr<class BtlPanel> _BtlPanel{};
	bool _LateInit = false;
private:
	explicit Hotel_S03();
	virtual ~Hotel_S03() = default;
	// Scene을(를) 통해 상속됨
	virtual void Free() override;
public:
	static Hotel_S03* Create();
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
	void RenderDataSetUp(const bool bTest);
	void TriggerSetUp();
	void TriggerUpGround();
	void TriggerNextScene();
	void TriggerShop(const std::weak_ptr<Trigger>& _BattleTrigger);
	std::weak_ptr<Trigger> TriggerBattleStart();


	// 일단 폐기된 트리거 ..... 
	//   UpGround 오른쪽 위치 Battle
	void TriggerFirstBattle();
	//   상점 앞에서의 전투 . 
	void TriggerBattleInFrontShop();
	// 폐기된 트리거 ... 

	void LateInit();
};
#endif // !__M01_HOTEL_S03_H__