#ifndef __UI_BTL_PANEL__
#define __UI_BTL_PANEL__
#include "GameObject.h"
#include "RenderInterface.h"
#include "Font.h"
#include "Nero.h"

class BtlPanel : public ENGINE::GameObject,
	public ENGINE::RenderInterface
{
private:
	enum UI_DESC_ID
	{
		REDORB = 0,
		TARGET_CURSOR,
		TARGET_HP,
		BOSS_GUAGE,
		HP_GLASS,
		EX_GAUGE_BACK,
		EX_GAUGE,
		STYLISH_LETTER,
		HP_GAUGE,
		TDT_GAUGE,
		KEYBOARD,
		RANK_BACK,
		RANK,
		RANK_LETTER,
		STYLISH_POINTS,
		SECRET_VISIONS,
		DESC_END
	};
	struct UI_DESC
	{
		bool	Using;
		Vector3	Pos;	// z -> plane 기준 0.02보다 작으면 짤림
		Vector3 Scale;
	};
	UI_DESC* _UIDescs = nullptr;

	std::shared_ptr<ENGINE::StaticMesh> _PlaneMesh{};
	std::shared_ptr<ENGINE::StaticMesh> _Pipe0Mesh{};

	std::shared_ptr<ENGINE::Texture> _NoiseTex{};

	std::shared_ptr<ENGINE::Texture> _RedOrbALBMTex{};
	std::shared_ptr<ENGINE::Texture> _RedOrbATOSTex{};
	std::shared_ptr<ENGINE::Texture> _RedOrbNRMRTex{};

	std::shared_ptr<ENGINE::Texture> _TargetCursorTex{};
	std::shared_ptr<ENGINE::Texture> _EnemyHPTex{};

	std::shared_ptr<ENGINE::Texture> _BossGaugeATOSTex{};
	std::shared_ptr<ENGINE::Texture> _BossGaugeNRMRTex{};
	std::shared_ptr<ENGINE::Texture> _LoadingbarAlpTex{};

	std::shared_ptr<ENGINE::StaticMesh> _HPGlassMesh{};
	std::shared_ptr<ENGINE::Texture> _HPGlassATOSTex{};
	std::shared_ptr<ENGINE::Texture> _HPGlassNRMRTex{};
	std::shared_ptr<ENGINE::Texture> _GlassTex{};
	std::shared_ptr<ENGINE::Texture> _BloodTex{};

	std::shared_ptr<ENGINE::StaticMesh> _StylishMesh{};
	std::shared_ptr<ENGINE::Texture> _StylishALBMTex{};
	std::shared_ptr<ENGINE::Texture> _StylishNRMRTex{};

	std::shared_ptr<ENGINE::StaticMesh> _Ex0Mesh{};
	std::shared_ptr<ENGINE::StaticMesh> _Ex1Mesh{};
	std::shared_ptr<ENGINE::StaticMesh> _Ex2Mesh{};
	std::shared_ptr<ENGINE::Texture> _ExBackALBMTex{};
	std::shared_ptr<ENGINE::Texture> _ExBackATOSTex{};
	std::shared_ptr<ENGINE::Texture> _ExBackNRMRTex{};
	std::shared_ptr<ENGINE::Texture> _ExALBM0Tex{};
	std::shared_ptr<ENGINE::Texture> _ExNRMR0Tex{};
	std::shared_ptr<ENGINE::Texture> _ExEmissive0Tex{};
	std::shared_ptr<ENGINE::Texture> _ExFireTex{};
	std::shared_ptr<ENGINE::Texture> _BurnRampTex{};

	std::shared_ptr<ENGINE::Texture> _HPGaugeBaseALBMTex{};
	std::shared_ptr<ENGINE::Texture> _HPGaugeBaseATOSTex{};
	std::shared_ptr<ENGINE::Texture> _HPGaugeBaseNRMRTex{};
	std::shared_ptr<ENGINE::Texture> _HPGaugeALBMTex{};
	std::shared_ptr<ENGINE::Texture> _HPGaugeATOSTex{};
	std::shared_ptr<ENGINE::Texture> _HPGaugeNRMRTex{};

	std::shared_ptr<ENGINE::Texture> _TDTGaugeATOSTex{};
	std::shared_ptr<ENGINE::Texture> _TDTGaugeNRMRTex{};

	std::shared_ptr<ENGINE::Texture> _KeyBoardTex{};

	std::shared_ptr<ENGINE::Texture> _RingTex{};

	std::shared_ptr<ENGINE::StaticMesh> _RankDMesh{};
	std::shared_ptr<ENGINE::StaticMesh> _RankCMesh{};
	std::shared_ptr<ENGINE::StaticMesh> _RankBMesh{};
	std::shared_ptr<ENGINE::StaticMesh> _RankAMesh{};
	std::shared_ptr<ENGINE::StaticMesh> _RankSMesh{};
	std::shared_ptr<ENGINE::StaticMesh> _RankSSMesh{};
	std::shared_ptr<ENGINE::StaticMesh> _RankSSSMesh{};
	std::shared_ptr<ENGINE::Texture> _NullNormalRoughnessTex{};
	std::shared_ptr<ENGINE::Texture> _RankGradationTex{};
	std::shared_ptr<ENGINE::Texture> _RankNormalTex{};
	std::shared_ptr<ENGINE::Texture> _RankLetterTex{};
	std::shared_ptr<ENGINE::Texture> _RankLetterGlintTex{};
	std::shared_ptr<ENGINE::Texture> _RankDCAlbTex{};
	std::shared_ptr<ENGINE::Texture> _RankBAAlbTex{};
	std::shared_ptr<ENGINE::Texture> _RankSAlbTex{};

	std::shared_ptr<ENGINE::Texture> _SecretVision0Tex{};
	std::shared_ptr<ENGINE::Texture> _SecretVision1Tex{};
	std::shared_ptr<ENGINE::Texture> _SecretVision2Tex{};

	float _DeltaTime = 0.f;
	float _TotalAccumulateTime = 0.f;

	Vector3 _TargetPos = Vector3(0.f, 0.f, 0.f);
	float _TargetHP_Degree = 0.f;
	Vector2 _TargetHP_StartPtOrtho = Vector2(0.f, 0.f);
	Vector2 _TargetHP_Normal0 = Vector2(0.f, 0.f);
	Vector2 _TargetHP_Normal1 = Vector2(0.f, 0.f);
	float _TargetCursorAlpha = 0.f;

	/* 0 ~ 1 */
	float _PlayerHPRatio = 1.f;
	float _PlayerHPRatioDelay = 1.f;
	int _HPGaugeCount = 5;
	float _HPGaugeWidth = 50.f;
	float _HPGauge_CurXPosOrtho = 0.f;
	float _HPGauge_CurXPosOrthoDelay = 0.f;
	float _HPGlassDirt = 0.f;
	float _HPGlassDirtAccTime = 999.f;
	float _HPGlassRotY = 0.f;

	/* 0 ~ 1 */
	float _BossGaugeHPRatio = 1.f;
	float _BossGaugeHPRatioDelay = 0.f;
	float _BossGauge_CurXPosOrtho = 0.f;
	float _BossGauge_CurXPosOrthoDelay = 0.f;

	/* 0 ~ 1 */
	float _TDTGauge = 0.f;
	float _TDTGauge_CurXPosOrtho = 0.f;
	bool _TDTGauge_ConsumeStart = false;
	float _TDTGauge_ConsumeSpeed = 1.f;
	float _TDTGauge_EmissivePower = 0.f;

	Vector2 _InputUIOffset = Vector2(0.f, 0.f);

	float _RankBackMaxScale = 1.8f;
	float _RankMaxRotY = 15.f;
	float _RankCurRotY = 0.f;

	/*
	0 X
	1 ~ 99 D
	100 ~ 199 C
	200 ~ 299 B
	300 ~ 399 A
	400 ~ 499 S
	500 ~ 599 SS
	600 ~ 699 SSS
	*/
	float _RankScore = 0.f;
	int _PreRank = -1;
	int _CurRank = -999;
	float _RankGauge_CurYPosOrtho = 0.f;
	float _RankGauge_GoalZPos = 0.f;
	float _RankLetter_GoalXPos = 0.f;
	Vector4 _RankLetter_GlintFrame = Vector4();		// (Min.xy, Max.xy)
	float _RankLetter_GlintAccumulateTime = 0.f;
	float _RankDissolveAmount = 0.f;

	/*
	0 ~ 99999999
	_CurRank에 따라서 배율 증가
	*/
	uint32 _StylishPoints = 0u;
	bool _StylishPtsAccumulateStart = false;
	// + 결과창에 보여줄 누적 StylishPoints
	bool _StylishPtsAlive = false;
	float _StylishPtsAliveTime = 0.f;
	bool _StylishPtsAlive2ndCheck = false;

	/* 0 ~ 3 */
	float _ExGauge = 0.f;
	float _ExGauge_EmissivePower[3] = { 0.f, };
	Vector4 _ExGauge_FireFrame = Vector4();
	float _ExGauge_FireAccumulateTime = 999.f;
	float _ExGauge_FullFireAccumulateTime = 0.f;
	float _ExGauge_DissolveAmount = 0.f;

	enum KEY_INPUT_ID
	{
		Q = 0, W, E, R, A, S, D, F, Z, X, C, V,
		SHIFT, CTRL, SPACE, LBUTTON, MBUTTON, RBUTTON,
		KEY_INPUT_END
	};
	bool _KeyboardInput[KEY_INPUT_END] = { false, };

	Nero::WeaponList _CurWeaponIdx = Nero::WeaponList::RQ;
	int _CbsColor = 0;	// 0: Fire, 1: Ice, 2: Thunder

	Matrix _PerspectiveProjMatrix = Matrix();

	Vector3 _LightDir = Vector3(0.f, 1.f, 1.f);
	Vector3 _LightDir_ExGauge = Vector3(-1.f, 1.f, -1.f);
	Vector3 _LightDir_Stylish = Vector3(0.f, -1.f, 1.f);

	Vector2 _MinTexUV = Vector2(0.f, 0.f);
	Vector2 _MaxTexUV = Vector2(1.f, 1.f);

	Vector3 _Rot = Vector3(0.f, 0.f, 0.f);	// 디버그용 회전벡터(Degree). 회전값이 들어간게 예외케이스라 생각해서 UI_DESC에 없음

	enum FONT_ID
	{
		REDORBCOUNT,
		STYLISH_PTS_TITLE,
		STYLISH_PTS_SCORE,

		FONT_END
	};
	std::vector<std::weak_ptr<Font>> _FontVec;

	uint32 _RedOrbCount = 0u;

	int _SecretVisionState[3] = { 0, 0, 0 };
	//int _SecretVisionPreState[3] = { 0, 0, 0 };
	float _SecretVisionBrightScale[3] = { 0.01f, 0.01f, 0.01f };
	float _SecretVisionDissolveAmount[3] = { 1.f, 1.f, 1.f };

private:
	explicit BtlPanel() = default;
	virtual ~BtlPanel() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
private:
	void	RenderUI(const DrawInfo& _ImplInfo);
private:
	void	Init_UIDescs();
	void	Create_ScreenMat(UI_DESC_ID _ID, Matrix& _Out, int _Opt = 0);
	void	Update_TargetInfo(const float _fDeltaTime);
	void	Update_PlayerHP(const float _fDeltaTime);
	void	Update_Rank(const float _fDeltaTime);
	void	Update_ExGauge(const float _fDeltaTime);
	void	Update_Font(const float _fDeltaTime);
	void	Update_Etc(const float _fDeltaTime);	// 위를 제외한 나머지 
	Vector2	WorldPosToScreenPos(const Vector3& WorldPos);
	Vector2	ScreenPosToOrtho(float _ScreenPosX, float _ScreenPosY);
	void	Check_KeyInput(const float _fDeltaTime);
	void	Imgui_ModifyUI(UI_DESC_ID _ID);
public:
	static BtlPanel* Create();
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
	void SetTargetCursorActive(bool IsActive);
	void SetTargetCursor(const Vector3& TargetPos, const float HPRatio = 1.f);	/* HPRatio = 현재 HP / 최대 HP */
	void SetPlayerHPRatio(const float HPRatio, bool IsBloodedGlass = true);		/* HPRatio = 현재 HP / 최대 HP */
	
	void SetBossGaugeActive(bool IsActive);
	void SetBossGaugeHPRatio(const float HPRatio);	/* HPRatio = 현재 HP / 최대 HP */

	float GetTDTGauge() const { return _TDTGauge; } /* 0 ~ 1 */
	void AccumulateTDTGauge(const float Amount);
	void ConsumeTDTGauge(const float Speed = 1.f);	/* 0이 될때까지 Speed * DeltaTime 만큼 TDTGauge 감소 */

	void SetKeyInputActive(bool IsActive);
	
	void AddRankScore(float Score);	// 처음 호출시 StylishPoints 누적 시작
	void ResetRankScore();			// wave 하나가 끝나면 반드시 호출
	int GetRank() const	{ return _CurRank; }	/* -1 ~ 6 */

	float GetExGauge() const { return _ExGauge; }
	uint32 GetExGaugeCount() const { return static_cast<uint32>(_ExGauge); }
	void AddExGauge(float ExGauge);
	void UseExGauge(const uint32 Count);

	void ChangeWeaponUI(Nero::WeaponList NextWeapon, int CbsColor = 0);	// 0: Ice, 1: Thunder, 2: Fire

	void AccumulateRedOrb(const uint32 Amount);

	void ActivateSecretVision(const int Number);
	void DissolveAllSecretVision();

};
#endif // !__UI_BTL_PANEL__