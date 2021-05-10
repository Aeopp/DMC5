#ifndef __UI_TITLE_PANEL__
#define __UI_TITLE_PANEL__
#include "GameObject.h"
#include "RenderInterface.h"

class TitlePanel : public ENGINE::GameObject,
				   public ENGINE::RenderInterface
{
public:
	bool IsReadyToNextScene() const { return _ReadyToNextScene; }
	void SetLoadingProgress(float Progress) { _LoadingProgress = Progress; }

private:
	std::shared_ptr<ENGINE::StaticMesh> _PlaneMesh{};
	std::shared_ptr<ENGINE::Texture> _TitleTex{};
	std::shared_ptr<ENGINE::Texture> _SelectTex{};

	Matrix _TitleScreenMat = Matrix();
	float _BrightScale = 0.f;

	uint32 _CurState = 0u;
	Matrix _SelectScreenMat = Matrix();
	Vector2 _SelectUVMin = Vector2(0.f, 0.f);
	Vector2 _SelectUVMax = Vector2(1.f, 1.f);
	
	std::weak_ptr<class Font> _NewGameText{};
	std::weak_ptr<class Font> _ExitText{};

	bool _ReadyToNextScene = false;
	bool _PressReturn = false;

	float _AccTime = 0.f;
	uint32 _TextBlinkCnt = 0u;
	bool _TextRender = true;

	bool _ShowLoadingProgress = false;

	// Ÿ��Ʋ ȭ����� �̸� �ε��ؼ� �갡 ���� ����
	// �ٸ� ���� ���� �г��� ���� ����
	float _LoadingProgress = 0.f;
	std::shared_ptr<ENGINE::Texture> _LoadingBaseTex{};

private:
	explicit TitlePanel() = default;
	virtual ~TitlePanel() = default;
	// GameObject��(��) ���� ��ӵ�
	virtual void Free() override;
	virtual std::string GetName() override;
private:
	void	RenderUI(const DrawInfo& _Info);
private:
	void	Update_TitleLogic(const float _fDeltaTime);
	void	Update_LoadingLogic(const float _fDeltaTime);
public:
	static TitlePanel* Create();
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
#endif // !__UI_TITLE_PANEL__