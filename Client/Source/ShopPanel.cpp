#include "stdafx.h"
#include "..\Header\ShopPanel.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include "Font.h"
#include "BtlPanel.h"


void ShopPanel::Free()
{
	SafeDeleteArray(_UIDescs);

	for (auto& Element : _FontVec)
		Destroy(Element);
	_FontVec.clear();
	_FontVec.shrink_to_fit();

	GameObject::Free();
}

std::string ShopPanel::GetName()
{
	return "ShopPanel";
}

ShopPanel* ShopPanel::Create()
{
	return new ShopPanel{};
}


void ShopPanel::RenderUI(const DrawInfo& _ImplInfo)
{
	UI_DESC_ID CurID = DESC_END;
	Matrix ScreenMat;
	Vector3 ExtraColor = Vector3(0.f, 0.f, 0.f);

	// 노출도 고정
	Renderer::GetInstance()->FixedExposure = 1.f;

	auto PlaneSubset = _PlaneMesh->GetSubset(0u);

	_ImplInfo.Fx->SetMatrix("Perspective", &_PerspectiveProjMatrix);
	_ImplInfo.Fx->SetTexture("NoiseMap", _NoiseTex->GetTexture());
	_ImplInfo.Fx->SetFloatArray("LightDirection", _LightDir, 3u);

	//
	CurID = STATUE_BG;
	if (!_ImplInfo.IsAfterPostProcessing && _UIDescs[CurID].Using)
	{
		if (auto SharedSubset = PlaneSubset.lock();
			SharedSubset)
		{
			_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _NullBlackTex->GetTexture());
			_ImplInfo.Fx->SetFloat("_BrightScale", 0.f);

			Create_ScreenMat(CurID, ScreenMat, 1);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

			_ImplInfo.Fx->BeginPass(1);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();
		}

		auto Pipe0Subset = _Pipe0Mesh->GetSubset(0u);
		if (auto SharedSubset = Pipe0Subset.lock();
			SharedSubset)
		{
			_ImplInfo.Fx->SetTexture("ATOS0Map", _StatueATOSTex->GetTexture());
			_ImplInfo.Fx->SetFloat("_AccumulationTexU", _TotalAccumulateTime * 0.01f);
			_ImplInfo.Fx->SetFloat("_BrightScale", 0.001f);

			Create_ScreenMat(CurID, ScreenMat);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

			_ImplInfo.Fx->BeginPass(2);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();
		}

		if (auto SharedSubset = PlaneSubset.lock();
			SharedSubset)
		{
			_ImplInfo.Fx->SetTexture("ATOS0Map", _GlintAlp0Tex->GetTexture());
			_ImplInfo.Fx->SetFloat("_BrightScale", 0.1f);
			_ImplInfo.Fx->SetFloat("_SliceAmount", (sinf(_TotalAccumulateTime * 2.f)) * 0.002f + 0.997f);

			Create_ScreenMat(CurID, ScreenMat, 2);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

			_ImplInfo.Fx->BeginPass(3);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();
		}

		// divinity statue
		_ImplInfo.Fx->SetFloatArray("_ExtraColor", Vector3(0.f, 0.f, 0.f), 3u);
		_ImplInfo.Fx->SetFloat("_BrightScale", 0.01f);

		Create_ScreenMat(CurID, ScreenMat, 3);	// Perspective
		_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

		_ImplInfo.Fx->BeginPass(4);

		auto StatueSubset = _DivinityStatueMesh->GetSubset(0u);
		if (auto SharedSubset = StatueSubset.lock();
			SharedSubset)
		{
			_ImplInfo.Fx->SetTexture("ALB0Map", _DivinityStatueALBM0Tex->GetTexture());
			_ImplInfo.Fx->SetTexture("NRMR0Map", _DivinityStatueNRMR0Tex->GetTexture());

			SharedSubset->Render(_ImplInfo.Fx);
		}
		StatueSubset = _DivinityStatueMesh->GetSubset(1u);
		if (auto SharedSubset = StatueSubset.lock();
			SharedSubset)
		{
			_ImplInfo.Fx->SetTexture("ALB0Map", _DivinityStatueALBM1Tex->GetTexture());
			_ImplInfo.Fx->SetTexture("NRMR0Map", _DivinityStatueNRMR1Tex->GetTexture());

			SharedSubset->Render(_ImplInfo.Fx);
		}

		_ImplInfo.Fx->EndPass();
	}

	if (auto SharedSubset = PlaneSubset.lock();
		SharedSubset)
	{
		//
		CurID = REDORB;
		if (_ImplInfo.IsAfterPostProcessing && _UIDescs[CurID].Using)
		{
			_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _RedOrbALBMTex->GetTexture());
			_ImplInfo.Fx->SetTexture("ATOS0Map", _RedOrbATOSTex->GetTexture());
			_ImplInfo.Fx->SetTexture("NRMR0Map", _RedOrbNRMRTex->GetTexture());
			_ImplInfo.Fx->SetFloat("_BrightScale", 1.f);

			Create_ScreenMat(CurID, ScreenMat);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

			_ImplInfo.Fx->BeginPass(0);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();
		}

		//
		CurID = SELECTGUIDE;
		if (_ImplInfo.IsAfterPostProcessing && _UIDescs[CurID].Using)
		{
			// ~~~
			//_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _RedOrbALBMTex->GetTexture());
			//_ImplInfo.Fx->SetTexture("ATOS0Map", _RedOrbATOSTex->GetTexture());
			//_ImplInfo.Fx->SetTexture("NRMR0Map", _RedOrbNRMRTex->GetTexture());
			//_ImplInfo.Fx->SetFloat("_BrightScale", 1.f);

			//Create_ScreenMat(CurID, ScreenMat);
			//_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

			//_ImplInfo.Fx->BeginPass(1);
			//SharedSubset->Render(_ImplInfo.Fx);
			//_ImplInfo.Fx->EndPass();
		}
	}

	// 노출도 고정 해제
	Renderer::GetInstance()->FixedExposure = std::nullopt;
}

void ShopPanel::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto SpTransform = _WeakTransform.lock();
		SpTransform)
	{
		_RenderUpdateInfo.World = SpTransform->GetRenderMatrix();
	}
}

HRESULT ShopPanel::Ready()
{
	//
	SetRenderEnable(true);

	//
	m_nTag = GAMEOBJECTTAG::UI_ShopPanel;

	//
	ENGINE::RenderProperty _InitRenderProp;
	_InitRenderProp.bRender = true;
	_InitRenderProp.RenderOrders[RenderProperty::Order::UI] =
	{
		{"ShopPanel",
		[this](const DrawInfo& _Info)
			{
				this->RenderUI(_Info);
			}
		},
	};
	RenderInterface::Initialize(_InitRenderProp);

	//
	_PlaneMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx");
	_Pipe0Mesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\pipe00.fbx");
	
	_NoiseTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\noiseInput_ATOS.tga");
	_NullBlackTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\NullBlack.tga");
	
	_StatueATOSTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\statue_atos.tga");
	_GlintAlp0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Light\\tex_capcom_light_glint_0007_alpg.tga");
	_DivinityStatueMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Map\\Object\\Jikuushinzou\\Jikuushinzou.fbx");
	_DivinityStatueALBM0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Map\\Object\\Jikuushinzou\\sm0994_jikuushinzou_A_ALBM.dds");
	_DivinityStatueNRMR0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Map\\Object\\Jikuushinzou\\sm0994_jikuushinzou_A_NRMR.dds");
	_DivinityStatueALBM1Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Map\\Object\\Jikuushinzou\\sm0994_jikuushinzou_B_ALBM.dds");
	_DivinityStatueNRMR1Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Map\\Object\\Jikuushinzou\\sm0994_jikuushinzou_B_NRMR.dds");
	
	_RedOrbALBMTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\red_orb_albm.tga");
	_RedOrbATOSTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\red_orb_atos.tga");
	_RedOrbNRMRTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\red_orb_nrmr.tga");

	_SelectGuideTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\shop\\SELECTGUIDE.png");

	//
	D3DXMatrixPerspectiveFovLH(&_PerspectiveProjMatrix, D3DXToRadian(0.5f), (float)g_nWndCX / g_nWndCY, 0.001f, 1.f);

	//
	Init_UIDescs();

	//
	Init_Font();

	//
	_BtlPanel = std::static_pointer_cast<BtlPanel>(FindGameObjectWithTag(UI_BtlPanel).lock());

	return S_OK;
}

HRESULT ShopPanel::Awake()
{
	return S_OK;
}

HRESULT ShopPanel::Start()
{
	return S_OK;
}

void ShopPanel::Init_Font()
{
	_FontVec.reserve(FONT_END);
	for (uint32 i = 0u; i < FONT_END; ++i)
		_FontVec.push_back(static_pointer_cast<Font>(AddGameObject<Font>().lock()));
	
	_FontVec[FT_REDORBCOUNT].lock()->SetRenderFlag(true);
	
	_FontVec[FT_CUSTOMIZE].lock()->SetText(
		"C U S T O  M I  Z E",
		Font::TEX_ID::DMC5_WHITE,
		{ _UIDescs[CUSTOMIZE].Pos.x, _UIDescs[CUSTOMIZE].Pos.y },
		{ 0.5f, 0.5f },
		Vector3(0.537f, 0.494f, 0.408f),
		false);
	_FontVec[FT_CUSTOMIZE].lock()->SetRenderFlag(true);
}

UINT ShopPanel::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	_TotalAccumulateTime += _fDeltaTime;

	//
	Update_Font(_fDeltaTime);
	Check_KeyInput(_fDeltaTime);

	//
	//POINT pt{};
	//GetCursorPos(&pt);
	//ScreenToClient(g_hWnd, &pt);
	//Vector2 TargetPos = Vector2(static_cast<float>(pt.x), static_cast<float>(pt.y));
	//std::cout << ScreenPosToOrtho(TargetPos.x, TargetPos.y).x << std::endl;

	return 0;
}

UINT ShopPanel::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void ShopPanel::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		Imgui_ModifyUI();
	}
}

void ShopPanel::OnEnable()
{
	GameObject::OnEnable();
}

void ShopPanel::OnDisable()
{
	GameObject::OnDisable();
}

void ShopPanel::Init_UIDescs()
{
	if (!_UIDescs)
		_UIDescs = new UI_DESC[DESC_END];

	// Using, Pos, Scale, Rot
	_UIDescs[STATUE_BG] = { true, Vector3(640.f, 360.f, 0.9f), Vector3(8.f, 8.f, 1.f),  Vector3(0.f, 0.f, 0.f) };
	_UIDescs[REDORB] = { true, Vector3(1070.f, 50.f, 0.5f), Vector3(0.55f, 0.55f, 1.f),  Vector3(0.f, 0.f, 0.f) };
	_UIDescs[CUSTOMIZE] = { true, Vector3(50.f, 40.f, 0.5f), Vector3(0.6f, 0.6f, 1.f),  Vector3(0.f, 0.f, 0.f) };
	_UIDescs[SELECTGUIDE] = { true, Vector3(50.f, 600.f, 0.5f), Vector3(1.f, 1.f, 1.f),  Vector3(0.f, 0.f, 0.f) };
}

void ShopPanel::Create_ScreenMat(UI_DESC_ID _ID, Matrix& _Out, int _Opt/*= 0*/)
{
	Matrix RotMat;

	D3DXMatrixIdentity(&_Out);

	switch (_ID)
	{
	case STATUE_BG:
		switch (_Opt)
		{
		case 1:
			_Out._11 = 12.8f;
			_Out._22 = 7.2f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 0.f;
			_Out._42 = 0.f;
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.f);
			_MaxTexUV = Vector2(1.f, 1.f);
			break;
		case 2:
			_Out._11 = 16.f;
			_Out._22 = 16.f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = 0.f;
			_Out._42 = 0.f;
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.f);
			_MaxTexUV = Vector2(1.f, 1.f);
			break;
		case 3:	// Perspective
			_Out._11 = 0.001f;
			_Out._22 = 0.001f;
			_Out._33 = FLT_MIN;
			_Out._41 = 0.f;
			_Out._42 = -0.08f;
			_Out._43 = 7.f;

			//_Out._11 = _DebugScale.x;
			//_Out._22 = _DebugScale.y;
			//_Out._33 = _DebugScale.z;
			//D3DXMatrixRotationX(&RotMat, D3DXToRadian(_DebugRot.x));
			//_Out *= RotMat;
			//D3DXMatrixRotationY(&RotMat, D3DXToRadian(_DebugRot.y));
			//_Out *= RotMat;
			//D3DXMatrixRotationZ(&RotMat, D3DXToRadian(_DebugRot.z));
			//_Out *= RotMat;
			//_Out._41 = _DebugPos.x;
			//_Out._42 = _DebugPos.y;
			//_Out._43 = _DebugPos.z;

			break;

		case 0: default:
			goto DEFAULT;
			break;
		}
		break;

	case REDORB:
		if (1 == _Opt)
		{
			_Out._11 = _UIDescs[_ID].Scale.x * 1.f;
			_Out._22 = _UIDescs[_ID].Scale.y * 0.5f;
			_Out._33 = _UIDescs[_ID].Scale.z;
			_Out._41 = (_UIDescs[_ID].Pos.x + 55.f) - (g_nWndCX >> 1);
			_Out._42 = -((_UIDescs[_ID].Pos.y) - (g_nWndCY >> 1));
			_Out._43 = _UIDescs[_ID].Pos.z;
			_MinTexUV = Vector2(0.f, 0.f);
			_MaxTexUV = Vector2(1.f, 1.f);
		}
		else
		{
			goto DEFAULT;
		}
		break;

	case SELECTGUIDE:
		_Out._11 = _UIDescs[_ID].Scale.x;
		_Out._22 = _UIDescs[_ID].Scale.y;
		_Out._33 = _UIDescs[_ID].Scale.z;
		_Out._41 = (_UIDescs[_ID].Pos.x) - (g_nWndCX >> 1);
		_Out._42 = -((_UIDescs[_ID].Pos.y) - (g_nWndCY >> 1));
		_Out._43 = _UIDescs[_ID].Pos.z;
		if (0 == _Opt)
		{
			_MinTexUV = Vector2(0.f, 0.f);
			_MaxTexUV = Vector2(1.f, 0.5f);
		}
		else if (1 == _Opt)
		{
			_MinTexUV = Vector2(0.f, 0.5f);
			_MaxTexUV = Vector2(1.f, 1.f);
		}
		break;

	default: DEFAULT:
		_Out._11 = _UIDescs[_ID].Scale.x;
		_Out._22 = _UIDescs[_ID].Scale.y;
		_Out._33 = _UIDescs[_ID].Scale.z;
		D3DXMatrixRotationX(&RotMat, D3DXToRadian(_UIDescs[_ID].Rot.x));
		_Out *= RotMat;
		D3DXMatrixRotationY(&RotMat, D3DXToRadian(_UIDescs[_ID].Rot.y));
		_Out *= RotMat;
		D3DXMatrixRotationZ(&RotMat, D3DXToRadian(_UIDescs[_ID].Rot.z));
		_Out *= RotMat;
		_Out._41 = _UIDescs[_ID].Pos.x - (g_nWndCX >> 1);
		_Out._42 = -(_UIDescs[_ID].Pos.y - (g_nWndCY >> 1));
		_Out._43 = _UIDescs[_ID].Pos.z;
		break;
	}
}

void ShopPanel::Update_Font(const float _fDeltaTime)
{
	// RedOrb
	if (auto pFont = _FontVec[FT_REDORBCOUNT].lock();
		pFont)
	{
		if (_UIDescs[REDORB].Using)
		{
			pFont->SetText(
				std::to_string(_BtlPanel.lock()->GetRedOrbCount()),
				Font::TEX_ID::DMC5_BLACK_GRAD,
				{ _UIDescs[REDORB].Pos.x + 35.f, _UIDescs[REDORB].Pos.y },
				{ 0.8f, 0.8f },
				Vector3(1.f, 1.f, 1.f),
				true);

			pFont->SetRenderFlag(true);
		}
		else
		{
			pFont->SetRenderFlag(false);
		}
	}

	// Customize
	if (auto pFont = _FontVec[FT_CUSTOMIZE].lock();
		pFont)
	{
		if (_UIDescs[CUSTOMIZE].Using)
			pFont->SetRenderFlag(true);
		else
			pFont->SetRenderFlag(false);
	}
}

Vector2 ShopPanel::WorldPosToScreenPos(const Vector3& WorldPos)
{
	const ENGINE::RenderInformation& info = Renderer::GetInstance()->_RenderInfo;

	Vector4 Pos = Vector4(WorldPos.x, WorldPos.y, WorldPos.z, 1.f);
	D3DXVec4Transform(&Pos, &Pos, &info.View);
	D3DXVec4Transform(&Pos, &Pos, &info.Projection);

	float x = (Pos.x / Pos.w + 1.f) * (g_nWndCX >> 1);
	float y = (-Pos.y / Pos.w + 1.f) * (g_nWndCY >> 1);

	return Vector2(x, y);
}

Vector2 ShopPanel::ScreenPosToOrtho(float _ScreenPosX, float _ScreenPosY)
{
	Vector2 Ret = Vector2(_ScreenPosX - (g_nWndCX >> 1), -(_ScreenPosY - (g_nWndCY >> 1)));
	D3DXVec2TransformCoord(&Ret, &Ret, &Renderer::GetInstance()->_RenderInfo.Ortho);
	return Ret;
}

void ShopPanel::Check_KeyInput(const float _fDeltaTime)
{

}

void ShopPanel::Imgui_ModifyUI()
{
	Vector3 Position = _DebugPos;
	ImGui::InputFloat3("Position##ShopPanel", Position);
	_DebugPos = Position;

	Vector3 Scale = _DebugScale;
	ImGui::InputFloat3("Scale##ShopPanel", Scale);
	_DebugScale = Scale;

	Vector3 Rot = _DebugRot;
	ImGui::InputFloat3("Rot##ShopPanel", Rot);
	_DebugRot = Rot;
}