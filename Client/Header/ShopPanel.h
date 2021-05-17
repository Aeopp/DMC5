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
		REDORB = 0,

		DESC_END
	};
	struct UI_DESC
	{
		bool	Using;
		Vector3	Pos;	// z -> plane ���� 0.02���� ������ ©��
		Vector3 Scale;
		Vector3 Rot;
	};
	UI_DESC* _UIDescs = nullptr;

	std::shared_ptr<ENGINE::StaticMesh> _PlaneMesh{};
	std::shared_ptr<ENGINE::Texture> _NoiseTex{};
	std::shared_ptr<ENGINE::Texture> _RedOrbALBMTex{};
	std::shared_ptr<ENGINE::Texture> _RedOrbATOSTex{};
	std::shared_ptr<ENGINE::Texture> _RedOrbNRMRTex{};

	float _TotalAccumulateTime = 0.f;
	Matrix _PerspectiveProjMatrix = Matrix();
	Vector3 _LightDir = Vector3(0.f, 1.f, 1.f);
	Vector2 _MinTexUV = Vector2(0.f, 0.f);
	Vector2 _MaxTexUV = Vector2(1.f, 1.f);

	enum FONT_ID
	{
		REDORBCOUNT,

		FONT_END
	};
	std::vector<std::weak_ptr<class Font>> _FontVec;

	std::weak_ptr<class BtlPanel> _BtlPanel;
	
private:
	explicit ShopPanel() = default;
	virtual ~ShopPanel() = default;
	// GameObject��(��) ���� ��ӵ�
	virtual void Free() override;
	virtual std::string GetName() override;
private:
	void	RenderUI(const DrawInfo& _ImplInfo);
private:
	void	Init_UIDescs();
	void	Update_Font(const float _fDeltaTime);
	void	Create_ScreenMat(UI_DESC_ID _ID, Matrix& _Out, int _Opt = 0);
	Vector2	WorldPosToScreenPos(const Vector3& WorldPos);
	Vector2	ScreenPosToOrtho(float _ScreenPosX, float _ScreenPosY);
	void	Check_KeyInput(const float _fDeltaTime);
	void	Imgui_ModifyUI(UI_DESC_ID _ID);
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