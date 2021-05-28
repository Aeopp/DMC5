#ifndef __M02_LIBRARY_S06_H__
#define __M02_LIBRARY_S06_H__
#include "Scene.h"

class Em5300;
class Trigger;

class Library_S06 : public Scene
{
private:
	std::weak_ptr<class Nero> _Player{};
	std::weak_ptr<class BtlPanel> _BtlPanel{};
	std::weak_ptr<Em5300> m_pBoss{};
	std::weak_ptr<class FadeOut> _FadeOut{};
	std::weak_ptr<class MainCamera> _MainCamera;
	bool _LateInit = false;
private:
	explicit Library_S06();
	virtual ~Library_S06() = default;
	// Scene을(를) 통해 상속됨
	virtual void Free() override;
public:
	static Library_S06* Create();
public:
	// Scene을(를) 통해 상속됨
	virtual HRESULT LoadScene()							override;
	virtual HRESULT Awake()								override;
	virtual HRESULT Start()								override;
	virtual HRESULT Update(const float _fDeltaTime)		override;
	virtual HRESULT LateUpdate(const float _fDeltaTime) override;
private:	
	void LateInit();
	void BgmPlay();
	void RenderDataSetUp(const bool bTest);
	void ApplyShopUpgradeDesc();
private:
	void LoadObjects(const std::filesystem::path& path, const bool _bAni = false);
private:
	void TriggerSetUp();
	std::weak_ptr<Trigger> TriggerUlte();
};
#endif // !__M02_LIBRARY_S06_H__