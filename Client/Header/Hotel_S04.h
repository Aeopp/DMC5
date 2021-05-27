#ifndef __M01_HOTEL_S04_H__
#define __M01_HOTEL_S04_H__
#include "Scene.h"

class Hotel_S04 : public Scene
{
private:
	std::weak_ptr<class Nero> _Player{};
	std::weak_ptr<class BtlPanel> _BtlPanel{};
	std::weak_ptr<class MainCamera> _Camera{};
	std::weak_ptr<class FadeOut> _FadeOut{};
	weak_ptr<class Em5000> m_pBoss;
	bool _LateInit = false;
private:
	explicit Hotel_S04();
	virtual ~Hotel_S04() = default;
	// Scene을(를) 통해 상속됨
	virtual void Free() override;
public:
	static Hotel_S04* Create();
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
private:
	void LoadObjects(const std::filesystem::path& path);
private:
	void TriggerSetUp(); 
	void TriggerMeetingWithGoliath(const std::weak_ptr<class Trigger>& _CamTrigger);
	std::weak_ptr<class Trigger> TriggerCutScene();

};
#endif // !__M01_HOTEL_S04_H__