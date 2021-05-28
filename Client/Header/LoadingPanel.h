#ifndef __UI_LOADING_PANEL__
#define __UI_LOADING_PANEL__
#include "GameObject.h"
#include "RenderInterface.h"

class LoadingPanel : public ENGINE::GameObject,
					 public ENGINE::RenderInterface
{
public:
	bool IsReadyToNextScene() const { return _ReadyToNextScene; }
	void SetLoadingProgress(float Progress) { _LoadingProgress = Progress; }
	void SetShowLoadingProgress(bool Flag) { _ShowLoadingProgress = Flag; }

private:
	std::shared_ptr<ENGINE::StaticMesh> _PlaneMesh{};

	bool _ReadyToNextScene = false;
	float _LoadingProgress = 0.f;
	float _AccTime = 0.f;
	float _BrightScale = 0.f;

	std::shared_ptr<ENGINE::Texture> _Wing0Tex{};
	std::shared_ptr<ENGINE::Texture> _Wing1Tex{};
	Matrix _BaseScreenMat = Matrix();

	std::shared_ptr<ENGINE::Texture> _LoadingbarATOSTex{};
	Matrix _LoadingbarScreenMat = Matrix();
	float _NoiseAccTime = 0.f;
	float _LoadingbarCurXPosOrtho = 0.f;

	std::weak_ptr<class Font> _LoadingText{};
	bool _TextRender = false;
	float _TextBlickTick = 0.f;

	std::shared_ptr<ENGINE::Texture> _LoadingTextTex{};
	const uint32 _TextMaxCount = 6u;
	uint32 _CurTextIdx = 0u;
	Matrix _LoadingTextScreenMat = Matrix();
	float _TextAlpha = 0.f;

	bool _ShowLoadingProgress = true;

private:
	explicit LoadingPanel() = default;
	virtual ~LoadingPanel() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
private:
	void	RenderUI(const DrawInfo& _Info);
private:
	Vector2	ScreenPosToOrtho(float _ScreenPosX, float _ScreenPosY);
public:
	static LoadingPanel* Create();
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
#endif // !__UI_LOADING_PANEL__