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
	// Scene��(��) ���� ��ӵ�
	virtual void Free() override;
public:
	static Hotel_S01* Create();
public:
	// Scene��(��) ���� ��ӵ�
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
	//  �÷��̾ ���� �������� ó�� ����ģ ���� .
	void TriggerElectricBoard(const std::weak_ptr<Trigger>&_BattleTrigger);
	//  ���� ������ ���� ���� ���Ŀ� ���� ���� 
	std::weak_ptr<Trigger> TriggerElectricBoardBattle();

	// ��ū ���Ž� .
	void Trigger2nd();
	// ȣ�� ��  
	void TriggerInFrontOfHotel(const std::weak_ptr<Trigger>& _BattleTrigger);
	std::weak_ptr<Trigger> TriggerInFrontOfHotelBattle();

	void Trigger3rd();
	void Trigger4st();

	void LateInit();
};
#endif // !__M01_HOTEL_S01_H__