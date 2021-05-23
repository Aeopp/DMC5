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
		SELECT_WEAPON,
		SELECT_GUIDE,

		DESC_END
	};
	struct UI_DESC
	{
		bool	Using;
		Vector3	Pos;	// z -> plane 기준 0.02보다 작으면 짤림
		Vector3 Scale;
		//Vector3 Rot;
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
	std::shared_ptr<ENGINE::Texture> _SelectWeaponTex0{};	// Nero
	std::shared_ptr<ENGINE::Texture> _SelectWeaponTex1{};	// Dante
	std::shared_ptr<ENGINE::Texture> _SelectWeaponCursorTex{};
	std::shared_ptr<ENGINE::Texture> _SelectWeaponInfoBaseTex{};
	std::shared_ptr<ENGINE::Texture> _ItemAndInfoBaseTex{};
	std::shared_ptr<ENGINE::Texture> _ButtonBaseTex{};
	std::shared_ptr<ENGINE::Texture> _ButtonUpgradeFontTex{};
	std::shared_ptr<ENGINE::Texture> _WeaponUpgradeInfoFontTex{};
	std::shared_ptr<ENGINE::Texture> _WeaponUpgradeProgressTex{};

	std::shared_ptr<ENGINE::Texture> _WeaponBgTex0{};
	std::shared_ptr<ENGINE::Texture> _WeaponBgTex1{};
	std::shared_ptr<ENGINE::Texture> _WeaponBgTex2{};
	std::shared_ptr<ENGINE::Texture> _WeaponBgTex3{};

	float _TotalAccumulateTime = 0.f;
	Matrix _PerspectiveProjMatrix = Matrix();
	Vector3 _LightDir = Vector3(-1.f, 1.f, 1.f);
	Vector2 _MinTexUV = Vector2(0.f, 0.f);
	Vector2 _MaxTexUV = Vector2(1.f, 1.f);

	Vector3 _DebugPos = Vector3(640.f, 320.f, 0.5f);
	Vector3 _DebugScale = Vector3(1.f, 1.f, 1.f);
	Vector3 _DebugRot = Vector3(0.f, 0.f, 0.f);
	Vector2 _DebugMinUV = Vector2(0.f, 0.f);
	Vector2 _DebugMaxUV = Vector2(1.f, 1.f);

	enum FONT_ID
	{
		FT_REDORBCOUNT,
		FT_CUSTOMIZE,

		FT_WP_REDQUEEN,
		FT_WP_OVERTURE,
		FT_WP_CBS,
		FT_WP_REBELLION,

		FT_WP_RQ_EXGAUGEUP,
		FT_WP_OT_BATTERY,
		FT_WP_CBS_TRANSFORM,
		FT_WP_RB_REBELLION,

		FT_WP_RQ_EXGAUGEUP_COST,
		FT_WP_OT_BATTERY_COST,
		FT_WP_CBS_TRANSFORM_COST,
		FT_WP_RB_REBELLION_COST,

		FONT_END
	};
	std::vector<std::weak_ptr<class Font>> _FontVec;

	std::weak_ptr<class BtlPanel> _BtlPanel;
	
	enum DEPTH
	{
		NONE = -1,
		CATEGORY = 0,
		WEAPON_SELECT = 2,
	};
	DEPTH _CurDepth = CATEGORY;
	DEPTH _PreDepth = NONE;

	enum CMD
	{
		NO_CMD = 0,

		CATEGORY_WEAPON = 1,
		CATEGORY_ITEM = 2,

		WP_REDQUEEN,
		WP_OVERTURE,
		WP_CBS,
		WP_REBELLION

	};
	CMD _CurCmd = CATEGORY_WEAPON;
	CMD _PreCmd = CATEGORY_WEAPON;

	CMD _CategoryPreCmd = CATEGORY_WEAPON;
	float _CategoryCursorXPos = 194.f;
	float _CategoryCursorXScaleOffset = 1.f;
	float _CategoryWeaponSliceAmount = 0.f;
	float _CategoryWeaponBrightOffset = 1.f;
	float _CategoryWeaponInfoXPos = 410.f;
	float _CategoryWeaponInfoSliceAmount = 0.f;

	uint32 _ExgaugeUpCost = 1u;
	uint32 _BatteryCost = 2u;
	uint32 _TransformCost = 3u;
	uint32 _RebellionCost = 9999999u;

	uint32 _ExgaugeUpUpgradeCount = 1u;
	uint32 _BatteryUpgradeCount = 1u;
	uint32 _TransformUpgradeCount = 1u;
	uint32 _RebellionUpgradeCount = 0u;

	float _BrightAccTime = 0.f;
	float _ButtonBright = 1.f;
	float _UpgradeProgressBright = 1.f;

	bool _ButtonBlinkStart = false;
	float _ButtonBlinkBrightOffset = 1.f;
	float _ButtonBlinkAccTime = 0.f;
	int _ButtonBlinkCnt = 0;

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
private:
	void ResetOffset();
};
#endif // !__UI_SHOP_PANEL__