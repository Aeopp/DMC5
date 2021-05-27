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
	std::weak_ptr<class FadeOut> _ShopFadeOut{};
	std::vector<weak_ptr<class Effect>> m_vecQliphothBlock{};
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
	void TriggerBloodPrevious(const std::weak_ptr<Trigger> _WaveStart);

	std::weak_ptr<Trigger> TriggerBloodFirstWave(const std::weak_ptr<Trigger>_WaveTrigger);
	std::weak_ptr<Trigger> TriggerBloodSecondWave(const std::weak_ptr<Trigger>_WaveTrigger);
	std::weak_ptr<Trigger> TriggerBloodThirdWave();

	// 책장 박살내며 성큰이 등장 !!
	// 6개의 책장이 전부 박살내며 나오는 버전 !!
	void TriggerBookCaseSunkenSmash();

	// 책장 & 성큰 순차로 나오는 이벤트 !! 트리거는 전부 Disable 상태이니 시간차로 재생해주세요 ...
	void                   TriggerBookCaseSunkenFirstSmash();
	std::weak_ptr<Trigger> TriggerBookCaseSunkenSecondSmash();
	std::weak_ptr<Trigger> TriggerBookCaseSunkenThirdSmash();	// 성큰 소환 순서대로 죽여주세요... UI RANK 때문에.... - hscho
	std::optional<float>  OBookCaseSunkenSmash = std::nullopt;
	float BookCaseDelta = 0.5f;
	std::vector<std::weak_ptr<Trigger>> BookCaseTrigger{};

	void TriggerSewerSunken();
	void TriggerShop();
	void TriggerNextScene();
};
#endif // !__M02_LIBRARY_S05_H__
