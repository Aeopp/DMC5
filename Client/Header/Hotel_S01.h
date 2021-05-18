#ifndef __M01_HOTEL_S01_H__
#define __M01_HOTEL_S01_H__
#include "Scene.h"

class Trigger;
class Hotel_S01 : public Scene
{
private:
	std::weak_ptr<class Nero> _Player{};
	std::weak_ptr<class BtlPanel> _BtlPanel{};
	std::weak_ptr<class MainCamera> _MainCamera{};
	std::vector<weak_ptr<class Effect>> m_vecQliphothBlock;
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
	void BgmPlay();
	void LoadObjects(const std::filesystem::path& path, const bool _bAni = false);
	void LoadCollObjects(const std::filesystem::path& path);
	void LoadBreakablebjects(const std::filesystem::path& path);

	void TriggerSetUp();
	//  플레이어가 광장 전광판을 처음 마주친 시점 .
	void TriggerElectricBoard(const std::weak_ptr<Trigger>&_BattleTrigger);
	//  광장 전광판 도발 연출 이후에 전투 시작 
	std::weak_ptr<Trigger> TriggerElectricBoardBattle();

	// 성큰 스매쉬 .
	void Trigger2nd();
	// 호텔 앞  
	void TriggerInFrontOfHotel(const std::weak_ptr<Trigger>& _BattleTrigger);
	std::weak_ptr<Trigger> TriggerInFrontOfHotelBattle();

	void Trigger4st();

	void LateInit();
};
#endif // !__M01_HOTEL_S01_H__