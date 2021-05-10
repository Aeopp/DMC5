#ifndef __UI_FONT__
#define __UI_FONT__
#include "GameObject.h"
#include "RenderInterface.h"

class Font : public ENGINE::GameObject,
	public ENGINE::RenderInterface
{
public:
	enum TEX_ID : uint32
	{
		DMC5_BLACK_GRAD,
		DMC5_GREEN_GRAD,
		DMC5_WHITE,
		
		FONTTEX_ID_END
	};

	enum FADE_ID : uint32
	{
		NONE,
		ALPHA_LINEAR,
		DISOLVE_TORIGHT,

		FADE_ID_END
	};

	void SetText(const std::string& Text, 
		Font::TEX_ID TexID,
		const Vector2& ScreenPos, 
		const Vector2& Scale = Vector2(1.f, 1.f), 
		bool UsingShadow = false);
	
	void SetRenderFlag(bool IsRender, 
		Font::FADE_ID ID = FADE_ID::NONE);

private:
	std::shared_ptr<ENGINE::StaticMesh> _PlaneMesh{};
	std::shared_ptr<ENGINE::Texture> _NoiseTex{};

	std::vector<std::shared_ptr<ENGINE::Texture>> _FontTexVec{};
	TEX_ID _FontTexID = DMC5_BLACK_GRAD;

	// ���ڿ��� ������ġ, ���� ũ��, ȸ��
	Vector3 _Pos = Vector3(640.f, 360.f, 0.02f);
	Vector3 _Scale = Vector3(1.f, 1.f, 1.f);
	Vector3 _Rot = Vector3(0.f, 0.f, 0.f);

	struct FontDesc
	{ 
		// ���� ������ desc
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
	enum SCREENMAT_OPT { NOOPT = 0u, USINGSHADOW };
	bool _UsingShadow = false;

	bool _IsRender = false;
	FADE_ID _FadeID = FADE_ID::NONE;
	float _SliceAmount = 1.f;

	enum { MAX_CHAR_CNT = 32 };
	vector<float> _CharSliceAmount{};
	
private:
	explicit Font() = default;
	virtual ~Font() = default;
	// GameObject��(��) ���� ��ӵ�
	virtual void Free() override;
	virtual std::string GetName() override;
private:
	void	RenderFont(const DrawInfo& _Info);
private:
	void	LoadFontDesc(const std::filesystem::path& path);
	void	CreateScreenMat(SCREENMAT_OPT Opt = NOOPT);
	void	SetFontUV();
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
};
#endif // !__UI_FONT__