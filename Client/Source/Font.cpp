#include "stdafx.h"
#include "..\Header\Font.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
//#include "EngineStdafx.h"
//#include <filesystem>
#include <istream>
#include <fstream>

void Font::Free()
{
	for (auto& Element : _FontDescVec)
		SafeDelete(Element);
	_FontDescVec.clear();
	_FontDescVec.shrink_to_fit();

	GameObject::Free();
}

std::string Font::GetName()
{
	return "Font";
}

Font* Font::Create()
{
	return new Font{};
}

void Font::RenderFont(const DrawInfo& _Info)
{
	if (_Info.IsAfterPostProcessing)
	{
		auto WeakSubset = _PlaneMesh->GetSubset(0u);
		if (auto SharedSubset = WeakSubset.lock();
			SharedSubset)
		{
			_Info.Fx->SetTexture("ALB0Map", _Dmc5FontTex->GetTexture());
			_Info.Fx->SetFloat("_UsingNoise", _UsingNoise);
			if (_UsingNoise)
				_Info.Fx->SetTexture("NoiseMap", _NoiseTex->GetTexture());
			_Info.Fx->SetFloat("_SliceAmount", 0.f);

			if (!_UsingPerspective)
			{
				size_t textLength = _Text.length();

				if (_UsingShadow)
				{
					_PosOffset = { 0.f, 0.f };
					_Info.Fx->SetFloat("_BrightScale", 0.f);
					for (size_t i = 0; i < textLength; ++i)
					{
						_CurCharID = static_cast<uint32>(_Text.at(i));

						CreateScreenMat(1);
						_Info.Fx->SetMatrix("ScreenMat", &_ScreenMat);

						SetFontUV();
						_Info.Fx->SetFloatArray("_MinTexUV", *_CurMinUV, 2u);
						_Info.Fx->SetFloatArray("_MaxTexUV", *_CurMaxUV, 2u);

						_Info.Fx->BeginPass(0);
						SharedSubset->Render(_Info.Fx);
						_Info.Fx->EndPass();
					}
				}

				_PosOffset = { 0.f, 0.f };
				_Info.Fx->SetFloat("_BrightScale", 1.f);
				for (uint32 i = 0; i < textLength; ++i)
				{
					_CurCharID = static_cast<uint32>(_Text.at(i));

					CreateScreenMat();
					_Info.Fx->SetMatrix("ScreenMat", &_ScreenMat);

					SetFontUV();
					_Info.Fx->SetFloatArray("_MinTexUV", *_CurMinUV, 2u);
					_Info.Fx->SetFloatArray("_MaxTexUV", *_CurMaxUV, 2u);

					_Info.Fx->BeginPass(0);
					SharedSubset->Render(_Info.Fx);
					_Info.Fx->EndPass();
				}

				return;
			}
			else if (_UsingPerspective)
			{
				_Info.Fx->SetMatrix("Perspective", &_PerspectiveProjMatrix);
			
				// ...

				return;
			}
		}
	}
}

void Font::LoadFontDesc(const std::filesystem::path& path)
{
	// Clear
	for (auto& Element : _FontDescVec)
		SafeDelete(Element);
	_FontDescVec.clear();
	_FontDescVec.shrink_to_fit();

	// Load json
	using namespace rapidjson;

	std::ifstream Is{ path };

	if (!Is.is_open())
		return;

	IStreamWrapper Isw(Is);
	Document _Document{};
	_Document.ParseStream(Isw);

	if (_Document.HasParseError())
	{
		PRINT_LOG(L"Warning!", L"Font Parse Error!");
		return;
	}

	const float TexWidth = _Document["ScaleW"].GetFloat();
	const float TexHeight = _Document["ScaleH"].GetFloat();
	const uint32 CharsCount = _Document["CharsCount"].GetUint();
	_FontDescVec.resize(MAX_CHAR_ID + 1);	// 직접 접근 위해

	const Value& FontDescData = _Document["FontDataArray"];
	for (auto iter = FontDescData.Begin(); iter != FontDescData.End(); ++iter)
	{
		FontDesc desc;

		desc.CharID = iter->FindMember("CharID")->value.GetUint();

		const float x = iter->FindMember("X")->value.GetFloat();
		const float y = iter->FindMember("Y")->value.GetFloat();
		const float w = iter->FindMember("Width")->value.GetFloat();
		const float h = iter->FindMember("Height")->value.GetFloat();

		desc.MinUV = { x / TexWidth, y / TexHeight };
		desc.CharScale = { w / TexWidth, h / TexHeight };
		desc.MaxUV = desc.MinUV + desc.CharScale;

		_FontDescVec[desc.CharID] = new FontDesc(desc);
	}
}

void Font::CreateScreenMat(int Opt/*= 0*/)
{
	FontDesc* pDesc = _FontDescVec[_CurCharID];
	if (!pDesc)
		pDesc = _FontDescVec['?'];

	Matrix RotMat;
	D3DXMatrixIdentity(&_ScreenMat);

	float crr = 5.5f;
	_ScreenMat._11 = _Scale.x * pDesc->CharScale.x * crr;
	_ScreenMat._22 = _Scale.y * pDesc->CharScale.y * crr;
	_ScreenMat._33 = _Scale.z;
	if (_UsingPerspective)
	{
		D3DXMatrixRotationX(&RotMat, D3DXToRadian(_Rot.x));
		_ScreenMat *= RotMat;
		D3DXMatrixRotationY(&RotMat, D3DXToRadian(_Rot.y));
		_ScreenMat *= RotMat;
	}
	D3DXMatrixRotationZ(&RotMat, D3DXToRadian(_Rot.z));
	_ScreenMat *= RotMat;
	
	float ShadowPos = 0.f;
	if (1 == Opt)
		ShadowPos = 3.f;

	_ScreenMat._41 = _Pos.x + ShadowPos - (g_nWndCX >> 1) + _PosOffset.x;
	_ScreenMat._42 = -(_Pos.y + ShadowPos - (g_nWndCY >> 1)) /*+ _PosOffset.y*/;
	_ScreenMat._43 = _Pos.z;

	crr = 75.f;
	_PosOffset.x += (_ScreenMat._11 * crr + 2.f);
}

void Font::SetFontUV()
{
	FontDesc* pDesc = _FontDescVec[_CurCharID];
	if (!pDesc)
		pDesc = _FontDescVec['?'];

	_CurMinUV = &(pDesc->MinUV);
	_CurMaxUV = &(pDesc->MaxUV);
}

void Font::RenderReady()
{

}

HRESULT Font::Ready()
{
	SetRenderEnable(true);

	m_nTag = GAMEOBJECTTAG::UI_Font;

	ENGINE::RenderProperty _InitRenderProp;
	_InitRenderProp.bRender = true;
	_InitRenderProp.RenderOrders[RenderProperty::Order::UI] =
	{
		{"Font",
		[this](const DrawInfo& _Info)
			{
				this->RenderFont(_Info);
			}
		},
	};
	RenderInterface::Initialize(_InitRenderProp);

	_PlaneMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx");
	_NoiseTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\noiseInput_ATOS.tga");
	_Dmc5FontTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Font\\Texture\\dmc5font.tga");

	D3DXMatrixPerspectiveFovLH(&_PerspectiveProjMatrix, D3DXToRadian(2.5f), (float)g_nWndCX / g_nWndCY, 0.1f, 1.f);

	LoadFontDesc("..\\..\\Resource\\Font\\Data\\dmc5font.json");

	return S_OK;
}

HRESULT Font::Awake()
{
	return S_OK;
}

HRESULT Font::Start()
{
	return S_OK;
}

UINT Font::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);

	return 0;
}          

UINT Font::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void Font::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		static char buf[32] = "";
		ImGui::InputText("Text##Font", buf, 32);

		static int pos[2] = { 640, 360 };
		ImGui::SliderInt2("Pos##Font", pos, 0, 1280);

		static Vector2 scale = { 1.f, 1.f };
		ImGui::SliderFloat2("scale##Font", scale, 0.1f, 10.f);

		if (ImGui::Button("Apply##Font"))
			SetText(buf, Vector2((float)pos[0], (float)pos[1]), scale);
	}
}

void Font::OnEnable()
{
	GameObject::OnEnable();
}

void Font::OnDisable()
{
	GameObject::OnDisable();
}

void Font::SetText(const std::string& text, const Vector2& ScreenPos, const Vector2& Scale/*= Vector2(1.f, 1.f)*/, bool UsingShadow/*= false*/)
{
	_Text = text;
	_Pos = { ScreenPos.x, ScreenPos.y, 0.02f };
	_Scale = { Scale.x, Scale.y, 1.f };
	_UsingShadow = UsingShadow;
}