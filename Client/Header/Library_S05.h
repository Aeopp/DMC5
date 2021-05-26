#ifndef __M02_LIBRARY_S05_H__
#define __M02_LIBRARY_S05_H__
#include "Scene.h"
#include "Trigger.h"

class Library_S05 : public Scene
{
private:
	std::weak_ptr<class Nero> _Player{};
	std::weak_ptr<class BtlPanel> _BtlPanel{};
	std::weak_ptr<class ShopPanel> _ShopPanel{};
	bool _LateInit = false;
	bool _IsShopAvailable = false;
private:
	explicit Library_S05();
	virtual ~Library_S05() = default;
	// Scene을(를) 통해 상속됨
	virtual void Free() override;
public:
	static Library_S05* Create();
public:
	// Scene을(를) 통해 상속됨
	virtual HRESULT LoadScene()							override;
	virtual HRESULT Awake()								override;
	virtual HRESULT Start()								override;
	virtual HRESULT Update(const float _fDeltaTime)		override;
	virtual HRESULT LateUpdate(const float _fDeltaTime) override;
private:
	void LateInit();
	void RenderDataSetUp(const bool bTest);
	void BgmPlay();
	void ApplyShopUpgradeDesc();
	void CheckShopAvailable();
private:
	void LoadObjects(const std::filesystem::path& path, const bool _bAni = false);
	void LoadCollObjects(const std::filesystem::path& path);
	void LoadBreakablebjects(const std::filesystem::path& path);
private:
	void TriggerSetUp();
	void TriggerBloodPrevious(const std::weak_ptr<Trigger> _BloodTrigger);
	std::weak_ptr<Trigger> TriggerBlood();
	//   책장 박살내며 성큰이 등장 !!
	void TriggerBookCaseSunkenSmash();
	void TriggerSewerSunken();
	void TriggerNextScene();
};
#endif // !__M02_LIBRARY_S05_H__