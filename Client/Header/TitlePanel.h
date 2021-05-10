#ifndef __UI_TITLE_PANEL__
#define __UI_TITLE_PANEL__
#include "GameObject.h"
#include "RenderInterface.h"

class TitlePanel : public ENGINE::GameObject,
				   public ENGINE::RenderInterface
{
public:

private:
	std::shared_ptr<ENGINE::StaticMesh> _PlaneMesh{};
	std::shared_ptr<ENGINE::Texture> _TitleTex{};

	Matrix _ScreenMat = Matrix();
	float _BrightScale = 0.f;

private:
	explicit TitlePanel() = default;
	virtual ~TitlePanel() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
private:
	void	RenderUI(const DrawInfo& _Info);
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