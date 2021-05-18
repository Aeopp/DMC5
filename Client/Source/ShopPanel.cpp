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

	_ImplInfo.Fx->SetMatrix("Perspective", &_PerspectiveProjMatrix);
	_ImplInfo.Fx->SetTexture("NoiseMap", _NoiseTex->GetTexture());
	_ImplInfo.Fx->SetFloatArray("LightDirection", _LightDir, 3u);

	auto Pipe0Subset = _Pipe0Mesh->GetSubset(0u);
	if (auto SharedSubset = Pipe0Subset.lock();
		SharedSubset)
	{
		//
		CurID = STATUE_BG;
		if (!_ImplInfo.IsAfterPostProcessing && _UIDescs[STATUE_BG].Using)
		{
			_ImplInfo.Fx->SetTexture("ATOS0Map", _StatueATOSTex->GetTexture());
			_ImplInfo.Fx->SetFloat("_BrightScale", 0.001f);
			_ImplInfo.Fx->SetBool("_ApplyExposureCorr", true);

			Create_ScreenMat(CurID, ScreenMat);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

			_ImplInfo.Fx->BeginPass(1);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();
		}
	}

	auto PlaneSubset = _PlaneMesh->GetSubset(0u);
	if (auto SharedSubset = PlaneSubset.lock();
		SharedSubset)
	{
		//
		CurID = REDORB;
		if (_ImplInfo.IsAfterPostProcessing && _UIDescs[REDORB].Using)
		{
			_ImplInfo.Fx->SetTexture("ALB_NOsRGBMap", _RedOrbALBMTex->GetTexture());
			_ImplInfo.Fx->SetTexture("ATOS0Map", _RedOrbATOSTex->GetTexture());
			_ImplInfo.Fx->SetTexture("NRMR0Map", _RedOrbNRMRTex->GetTexture());
			_ImplInfo.Fx->SetFloat("_BrightScale", 1.f);
			_ImplInfo.Fx->SetBool("_ApplyExposureCorr", false);

			Create_ScreenMat(CurID, ScreenMat);
			_ImplInfo.Fx->SetMatrix("ScreenMat", &ScreenMat);

			_ImplInfo.Fx->BeginPass(0);
			SharedSubset->Render(_ImplInfo.Fx);
			_ImplInfo.Fx->EndPass();
		}
	}
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
	_StatueATOSTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\statue_atos.tga");
	_NoiseTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\noiseInput_ATOS.tga");
	_RedOrbALBMTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\red_orb_albm.tga");
	_RedOrbATOSTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\red_orb_atos.tga");
	_RedOrbNRMRTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\red_orb_nrmr.tga");

	//
	D3DXMatrixPerspectiveFovLH(&_PerspectiveProjMatrix, D3DXToRadian(2.5f), (float)g_nWndCX / g_nWndCY, 0.1f, 1.f);

	//
	Init_UIDescs();

	//
	_FontVec.reserve(FONT_END);
	for (uint32 i = 0u; i < FONT_END; ++i)
		_FontVec.push_back(static_pointer_cast<Font>(AddGameObject<Font>().lock()));
	_FontVec[REDORBCOUNT].lock()->SetRenderFlag(true);

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
		Imgui_ModifyUI(REDORB);
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
	_UIDescs[STATUE_BG] = { true, Vector3(640.f, 360.f, 0.9f), Vector3(1.f, 1.f, 1.f),  Vector3(0.f, 0.f, 0.f) };
	_UIDescs[REDORB] = { true, Vector3(1070.f, 50.f, 0.5f), Vector3(0.55f, 0.55f, 1.f),  Vector3(0.f, 0.f, 0.f) };
}

void ShopPanel::Create_ScreenMat(UI_DESC_ID _ID, Matrix& _Out, int _Opt/*= 0*/)
{
	Matrix RotMat;

	D3DXMatrixIdentity(&_Out);

	switch (_ID)
	{
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
	if (auto pFont = _FontVec[REDORBCOUNT].lock();
		pFont)
	{
		if (_UIDescs[REDORB].Using)
		{
			pFont->SetText(
				std::to_string(_BtlPanel.lock()->GetRedOrbCount()),
				Font::TEX_ID::DMC5_GREEN_GRAD,
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

void ShopPanel::Imgui_ModifyUI(UI_DESC_ID _ID)
{

}