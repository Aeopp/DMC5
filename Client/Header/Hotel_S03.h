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
	void LoadObjects(const std::filesystem::path& path, const bool _bAni = false);
	void LoadCollObjects(const std::filesystem::path& path);
	void LoadBreakablebjects(const std::filesystem::path& path);
private:
	void RenderDataSetUp(const bool bTest);
	void BgmPlay();
	void TriggerSetUp();
	void TriggerUpGround();
	void TriggerNextScene();
	void TriggerShop(const std::weak_ptr<Trigger>& _BattleTrigger);
	std::weak_ptr<Trigger> TriggerBattleStart();


	//// �ϴ� ���� Ʈ���� ..... 
	////   UpGround ������ ��ġ Battle
	//void TriggerFirstBattle();
	////   ���� �տ����� ���� . 
	//void TriggerBattleInFrontShop();
	//// ���� Ʈ���� ... 

	void LateInit();
};
#endif // !__M01_HOTEL_S03_H__