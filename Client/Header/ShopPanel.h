#ifndef __UI_SHOP_PANEL__
#define __UI_SHOP_PANEL__
#include "GameObject.h"
#include "RenderInterface.h"

class ShopPanel : public ENGINE::GameObject,
				  public ENGINE::RenderInterface
{
private:
	enum UI_DESC_ID
	{
		STATUE_BG = 0,
		REDORB,
		CUSTOMIZE,
		SELECT_CATEGORY,
		SELECT_GUIDE,

		DESC_END
	};
	struct UI_DESC
	{
		bool	Using;
		Vector3	Pos;	// z -> plane 기준 0.02보다 작으면 짤림
		Vector3 Scale;
		Vector3 Rot;
	};
	UI_DESC* _UIDescs = nullptr;

	std::shared_ptr<ENGINE::StaticMesh> _PlaneMesh{};
	std::shared_ptr<ENGINE::StaticMesh> _Pipe0Mesh{};

	std::shared_ptr<ENGINE::Texture> _NoiseTex{};
	std::shared_ptr<ENGINE::Texture> _NullBlackTex{};

	std::shared_ptr<ENGINE::Texture> _StatueATOSTex{};
	std::shared_ptr<ENGINE::Texture> _GlintAlp0Tex{};
	std::shared_ptr<ENGINE::StaticMesh> _DivinityStatueMesh{};
	std::shared_ptr<ENGINE::Texture> _DivinityStatueALBM0Tex{};	// subset 0
	std::shared_ptr<ENGINE::Texture> _DivinityStatueNRMR0Tex{};
	std::shared_ptr<ENGINE::Texture> _DivinityStatueALBM1Tex{};	// subset 1
	std::shared_ptr<ENGINE::Texture> _DivinityStatueNRMR1Tex{};

	std::shared_ptr<ENGINE::Texture> _RedOrbALBMTex{};
	std::shared_ptr<ENGINE::Texture> _RedOrbATOSTex{};
	std::shared_ptr<ENGINE::Texture> _RedOrbNRMRTex{};

	std::shared_ptr<ENGINE::Texture> _SelectGuideTex{};
	std::shared_ptr<ENGINE::Texture> _SelectCategoryFontTex{};

	std::shared_ptr<ENGINE::Texture> _SelectCategoryCursorTex{};

	float _TotalAccumulateTime = 0.f;
	Matrix _PerspectiveProjMatrix = Matrix();
	Vector3 _LightDir = Vector3(-1.f, 1.f, 1.f);
	Vector2 _MinTexUV = Vector2(0.f, 0.f);
	Vector2 _MaxTexUV = Vector2(1.f, 1.f);

	Vector3 _DebugPos = Vector3(640.f, 320.f, 0.5f);
	Vector3 _DebugScale = Vector3(0.001f, 0.001f, 0.001f);
	Vector3 _DebugRot = Vector3(0.f, 0.f, 0.f);
	Vector2 _DebugMinUV = Vector2(0.f, 0.f);
	Vector2 _DebugMaxUV = Vector2(1.f, 1.f);

	enum FONT_ID
	{
		FT_REDORBCOUNT,
		FT_CUSTOMIZE,
	
		FONT_END
	};
	std::vector<std::weak_ptr<class Font>> _FontVec;

	std::weak_ptr<class BtlPanel> _BtlPanel;
	
	enum DEPTH
	{
		NONE = -1,
		CATEGORY = 0
	};
	DEPTH _CurDepth = CATEGORY;
	DEPTH _PreDepth = NONE;

	enum CMD
	{
		CATEGORY_WEAPON = 1,
		CATEGORY_ITEM = 2,
	};
	CMD _CurCmd = CATEGORY_WEAPON;
	CMD _PreCmd = CATEGORY_WEAPON;

	CMD _CategoryPreCmd = CATEGORY_WEAPON;

private:
	explicit ShopPanel() = default;
	virtual ~ShopPanel() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
private:
	void	RenderUI(const DrawInfo& _ImplInfo);
private:
	void	Init_UIDescs();
	void	Init_Font();
	void	Update_Font(const float _fDeltaTime);
	void	Create_ScreenMat(UI_DESC_ID _ID, Matrix& _Out, int _Opt = 0);
	Vector2	WorldPosToScreenPos(const Vector3& WorldPos);
	Vector2	ScreenPosToOrtho(float _ScreenPosX, float _ScreenPosY);
	void	Check_KeyInput(const float _fDeltaTime);
	void	Imgui_ModifyUI();
public:
	static ShopPanel* Create();
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
public:

};
#endif // !__UI_SHOP_PANEL__