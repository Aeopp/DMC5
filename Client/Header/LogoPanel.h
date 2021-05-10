#ifndef __UI_LOGO_PANEL__
#define __UI_LOGO_PANEL__
#include "GameObject.h"
#include "RenderInterface.h"

class LogoPanel : public ENGINE::GameObject,
				  public ENGINE::RenderInterface
{
public:
	void SetLoadingFinishedFlag(bool Flag) { _LoadingFinished = Flag; }
	bool IsReadyToNextScene() const { return _ReadyToNextScene; }
	void QuickProgressToNextScene() { _PressReturn = true; }

private:
	std::shared_ptr<ENGINE::StaticMesh> _PlaneMesh{};
	std::shared_ptr<ENGINE::Texture> _Wing0Tex{};
	std::shared_ptr<ENGINE::Texture> _Wing1Tex{};

	Matrix _ScreenMat = Matrix();
	float _BrightScale = 0.f;

	bool _LoadingFinished = false;
	bool _ReadyToNextScene = false;
	bool _PressReturn = false;

	std::weak_ptr<class Font> _LoadingText{};

	float _TextBlickTick = 0.f;
	bool _TextRender = true;
	bool _TextChanged = false;

private:
	explicit LogoPanel() = default;
	virtual ~LogoPanel() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
private:
	void	RenderUI(const DrawInfo& _Info);
public:
	static LogoPanel* Create();
public:
	virtual void    RenderReady() override;
public:
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT    Update(const float _fDeltaTime) override;
	virtual UINT    LateUpdate(const float _fDeltaTime) override;
	virtual void    Editor() override;
	virtual void	OnEnable() override;
	virtual void    OnDisable() override;
};
#endif // !__UI_LOGO_PANEL__