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

	// 문자열의 시작위치, 공통 크기, 회전
	Vector3 _Pos = Vector3(640.f, 360.f, 0.02f);
	Vector3 _Scale = Vector3(1.f, 1.f, 1.f);
	Vector3 _Rot = Vector3(0.f, 0.f, 0.f);

	struct FontDesc
	{ 
		// 문자 각각의 desc
		uint32	CharID;
		Vector2 MinUV;
		Vector2 MaxUV;
		Vector2 CharScale;
	};
	vector<FontDesc*> _FontDescVec;
	enum { MAX_CHAR_ID = static_cast<uint32>('z') };

	uint32 _CurCharID = static_cast<uint32>('?');
	Matrix _ScreenMat = Matrix();
	Vector2 _PosOffset = Vector2(0.f, 0.f);
	const Vector2* _CurMinUV = nullptr;
	const Vector2* _CurMaxUV = nullptr;

	Matrix _PerspectiveProjMatrix = Matrix();
	bool _UsingPerspective = false;

	bool _UsingNoise = false;

	std::string _Text = "";
	bool _UsingShadow = false;

private:
	explicit Font() = default;
	virtual ~Font() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
	virtual std::string GetName() override;
private:
	void	RenderFont(const DrawInfo& _Info);
private:
	void	LoadFontDesc(const std::filesystem::path& path);
	void	CreateScreenMat(int Opt = 0);
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
	void SetText(const std::string& text, const Vector2& ScreenPos, const Vector2& Scale = Vector2(1.f, 1.f), bool UsingShadow = false);

};
#endif // !__UI_FONT__