#include "stdafx.h"
#include "..\Header\Font.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <istream>
#include <fstream>


void Font::SetText(const std::string& Text, Font::TEX_ID TexID, const Vector2& ScreenPos, const Vector2& Scale/*= Vector2(1.f, 1.f)*/, bool UsingShadow/*= false*/)
{
	_Text = Text;
	
	if (TexID >= FONTTEX_ID_END)
		_FontTexID = DMC5_BLACK_GRAD;
	else
		_FontTexID = TexID;

	_Pos = { ScreenPos.x, ScreenPos.y, 0.02f };
	_Scale = { Scale.x, Scale.y, 1.f };
	_UsingShadow = UsingShadow;
}

void Font::SetRenderFlag(bool IsRender, Font::FADE_ID ID/*= FADE_ID::NONE*/)
{
	if (IsRender != _IsRender)
	{
		if (ID >= FADE_ID::FADE_ID_END)
			_FadeID = FADE_ID::NONE;
		else
			_FadeID = ID;

		if (FADE_ID::DISOLVE_TORIGHT == _FadeID)
			_UsingNoise = true;
		else
			_UsingNoise = false;

		_IsRender = IsRender;
	}
}

void Font::Free()
{
	_FontTexVec.clear();
	_FontTexVec.shrink_to_fit();

	for (auto& Element : _FontDescVec)
		SafeDelete(Element);
	_FontDescVec.clear();
	_FontDescVec.shrink_to_fit();

	_CharSliceAmount.clear();
	_CharSliceAmount.shrink_to_fit();

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
			_Info.Fx->SetTexture("ALB0Map", _FontTexVec[_FontTexID]->GetTexture());
			_Info.Fx->SetFloat("_UsingNoise", _UsingNoise);
			if (_UsingNoise)
				_Info.Fx->SetTexture("NoiseMap", _NoiseTex->GetTexture());
			_Info.Fx->SetFloat("_SliceAmount", _SliceAmount);

			if (!_UsingPerspective)
			{
				size_t textLength = _Text.length();
				if (MAX_CHAR_CNT < textLength)
					textLength = MAX_CHAR_CNT;

				if (_UsingShadow)
				{
					_PosOffset = { 0.f, 0.f };
					_Info.Fx->SetFloat("_BrightScale", 0.f);
					for (size_t i = 0; i < textLength; ++i)
					{
						_CurCharID = static_cast<uint32>(_Text.at(i));

						CreateScreenMat(USINGSHADOW);
						_Info.Fx->SetMatrix("ScreenMat", &_ScreenMat);

						SetFontUV();
						_Info.Fx->SetFloatArray("_MinTexUV", *_CurMinUV, 2u);
						_Info.Fx->SetFloatArray("_MaxTexUV", *_CurMaxUV, 2u);

						if (FADE_ID::DISOLVE_TORIGHT == _FadeID)
						{
							// SliceAmount 재지정
							_Info.Fx->SetFloat("_SliceAmount", _CharSliceAmount[i]);
						}

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

					if (FADE_ID::DISOLVE_TORIGHT == _FadeID)
					{
						// SliceAmount 재지정
						_Info.Fx->SetFloat("_SliceAmount", _CharSliceAmount[i]);
					}

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

void Font::CreateScreenMat(SCREENMAT_OPT Opt/*= NOOPT*/)
{
	if (MAX_CHAR_ID < _CurCharID)
		_CurCharID = '?';

	FontDesc* pDesc = _FontDescVec[_CurCharID];
	if (!pDesc)
		pDesc = _FontDescVec['?'];

	float Interval = 2.f;
	if (pDesc->CharID == 'i' || pDesc->CharID == 'j' || pDesc->CharID == 'l')	// ㅠㅠ
		Interval = 4.f;

	Matrix RotMat;
	D3DXMatrixIdentity(&_ScreenMat);

	float Crr = 6.f;
	_ScreenMat._11 = _Scale.x * pDesc->CharScale.x * Crr;
	_ScreenMat._22 = _Scale.y * pDesc->CharScale.y * Crr;
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
	if (USINGSHADOW == Opt)
		ShadowPos = 3.f;

	_ScreenMat._41 = _Pos.x + ShadowPos - (g_nWndCX >> 1) + _PosOffset.x;
	_ScreenMat._42 = -(_Pos.y + ShadowPos - (g_nWndCY >> 1)) /*+ _PosOffset.y*/;
	_ScreenMat._43 = _Pos.z;

	Crr = 75.f;
	_PosOffset.x += (_ScreenMat._11 * Crr + Interval);
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
	_InitRenderProp.bRender = false;
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

	_FontTexVec.reserve(FONTTEX_ID_END);
	_FontTexVec.push_back(Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Font\\Texture\\dmc5font_0.png"));
	_FontTexVec.push_back(Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Font\\Texture\\dmc5font_1.png"));
	_FontTexVec.push_back(Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Font\\Texture\\dmc5font_2.png"));

	D3DXMatrixPerspectiveFovLH(&_PerspectiveProjMatrix, D3DXToRadian(2.5f), (float)g_nWndCX / g_nWndCY, 0.1f, 1.f);

	LoadFontDesc("..\\..\\Resource\\Font\\Data\\dmc5font.json");

	_CharSliceAmount.resize(MAX_CHAR_CNT);
	for (auto& Element : _CharSliceAmount)
		Element = 1.f;

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

	switch (_FadeID)
	{
	case FADE_ID::NONE:
	default:
		if (_IsRender)
		{
			_SliceAmount = 0.f;
			_RenderProperty.bRender = true;
		}
		else
		{
			_SliceAmount = 1.f;
			_RenderProperty.bRender = false;
		}

		break;

	case FADE_ID::ALPHA_LINEAR:
		if (_IsRender)
		{
			if (0.f < _SliceAmount)
			{
				_SliceAmount -= 2.f * _fDeltaTime;
				if (0.f > _SliceAmount)
					_SliceAmount = 0.f;
			}
		}
		else
		{
			if (1.f > _SliceAmount)
			{
				_SliceAmount += 2.f * _fDeltaTime;
				if (1.f < _SliceAmount)
					_SliceAmount = 1.f;
			}
		}

		if (1.f > _SliceAmount)
			_RenderProperty.bRender = true;
		else
			_RenderProperty.bRender = false;

		break;

	case FADE_ID::DISOLVE_TORIGHT:
		if (_IsRender)
		{
			if (0.f < _SliceAmount)
			{
				_SliceAmount -= 0.45f * _fDeltaTime;
				if (0.f > _SliceAmount)
					_SliceAmount = 0.f;

				uint32 idx = static_cast<uint32>((1.f - _SliceAmount) * static_cast<float>(MAX_CHAR_CNT));
				if (idx > MAX_CHAR_CNT)
					idx = MAX_CHAR_CNT;

				for (uint32 i = 0u; i < idx; ++i)
				{
					_CharSliceAmount[i] -= 0.75f * _fDeltaTime;
					if (0.f > _CharSliceAmount[i])
						_CharSliceAmount[i] = 0.f;
				}
			}
		}
		else
		{
			if (1.f > _SliceAmount)
			{
				_SliceAmount += 0.45f * _fDeltaTime;
				if (1.f < _SliceAmount)
					_SliceAmount = 1.f;

				int idx = static_cast<int>((_SliceAmount) * static_cast<float>(MAX_CHAR_CNT));
				if (idx > MAX_CHAR_CNT)
					idx = MAX_CHAR_CNT;

				for (int i = idx - 1; i >= 0; --i)
				{
					_CharSliceAmount[i] += 0.75f * _fDeltaTime;
					if (1.f < _CharSliceAmount[i])
						_CharSliceAmount[i] = 1.f;
				}
			}
		}

		if (1.f > _SliceAmount)
			_RenderProperty.bRender = true;
		else
			_RenderProperty.bRender = false;

		break;
		
	}

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

		static int isShadow = _UsingShadow;
		ImGui::SliderInt("Shadow##Font", &isShadow, 0, 1);

		static int TexID = _FontTexID;
		ImGui::SliderInt("Tex##Font", &TexID, 0, FONTTEX_ID_END - 1);

		if (ImGui::Button("Apply##Font"))
			SetText(buf, (Font::TEX_ID)TexID, Vector2((float)pos[0], (float)pos[1]), scale, isShadow);
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