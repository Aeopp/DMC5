#ifndef __UI_FONT__
#define __UI_FONT__
#include "GameObject.h"
#include "RenderInterface.h"

class Font : public ENGINE::GameObject,
			 public ENGINE::RenderInterface				
{
private:
	std::shared_ptr<ENGINE::StaticMesh> _PlaneMesh{};
	std::shared_ptr<ENGINE::Texture> _NoiseTex{};

	std::shared_ptr<ENGINE::Texture> _Dmc5FontTex{};

	Vector3 _Pos = Vector3(640.f, 360.f, 0.02f);
	Vector3 _Scale = Vector3(0.55f, 0.55f, 1.f);
	Vector3 _Rot = Vector3(0.f, 0.f, 0.f);

	Matrix _ScreenMat = Matrix();
	Vector2 _MinTexUV = Vector2(0.f, 0.f);
	Vector2 _MaxTexUV = Vector2(1.f, 1.f);

	Matrix _PerspectiveProjMatrix = Matrix();
	bool _UsingPerspective = false;

	bool _UsingNoise = false;

private:
	explicit Font() = default;
	virtual ~Font() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
private:
	void	RenderFont(const DrawInfo& _Info);
private:
	void	CreateScreenMat();
	void	SetFontUV(/*char index*/);
public:
	static Font* Create();
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
#endif // !__UI_FONT__