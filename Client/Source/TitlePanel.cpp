#include "stdafx.h"
#include "..\Header\TitlePanel.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include "Font.h"

void TitlePanel::Free()
{
	GameObject::Free();
}

std::string TitlePanel::GetName()
{
	return "TitlePanel";
}

TitlePanel* TitlePanel::Create()
{
	return new TitlePanel{};
}

void TitlePanel::RenderUI(const DrawInfo& _Info)
{
	if (_Info.IsAfterPostProcessing)
	{
		auto WeakSubset = _PlaneMesh->GetSubset(0u);
		if (auto SharedSubset = WeakSubset.lock();
			SharedSubset)
		{
			_Info.Fx->SetTexture("ALB0Map", _TitleTex->GetTexture());
			_Info.Fx->SetFloat("_SliceAmount", 0.f);
			_Info.Fx->SetFloat("_BrightScale", _BrightScale);
			_Info.Fx->SetMatrix("ScreenMat", &_ScreenMat);

			_Info.Fx->BeginPass(0);
			SharedSubset->Render(_Info.Fx);
			_Info.Fx->EndPass();
		}
	}
}

void TitlePanel::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto SpTransform = _WeakTransform.lock();
		SpTransform)
	{
		_RenderUpdateInfo.World = SpTransform->GetRenderMatrix();
	}
}

HRESULT TitlePanel::Ready()
{
	SetRenderEnable(true);

	m_nTag = GAMEOBJECTTAG::UI_TitlePanel;

	//auto InitTransform = GetComponent<ENGINE::Transform>();
	//InitTransform.lock()->SetPosition({ 0.f, 0.f, 0.f });

	ENGINE::RenderProperty _InitRenderProp;
	_InitRenderProp.bRender = true;
	_InitRenderProp.RenderOrders[RenderProperty::Order::UI] =
	{
		{"TitlePanel",
		[this](const DrawInfo& _Info)
			{
				this->RenderUI(_Info);
			}
		},
	};
	RenderInterface::Initialize(_InitRenderProp);

	_PlaneMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx");
	_TitleTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\logo_2.png");

	D3DXMatrixIdentity(&_ScreenMat);
	_ScreenMat._11 = 12.8f;
	_ScreenMat._22 = 7.6f;
	_ScreenMat._33 = 1.f;
	_ScreenMat._41 = 0.f;
	_ScreenMat._42 = 0.f;
	_ScreenMat._43 = 0.02f;

	if (_NewGameText = AddGameObject<Font>();
		!_NewGameText.expired())
	{
		_NewGameText.lock()->SetText("NEW GAME",
			Font::TEX_ID::DMC5_WHITE,
			Vector2(555.f, 600.f),
			Vector2(0.67f, 0.67f),
			Vector3(1.f, 1.f, 1.f),
			false);

		_NewGameText.lock()->SetRenderFlag(true);
	}
	//if (_ExitText = AddGameObject<Font>();
	//	!_ExitText.expired())
	//{
	//	_ExitText.lock()->SetText("EXIT",
	//		Font::TEX_ID::DMC5_WHITE,
	//		Vector2(555.f, 630.f),
	//		Vector2(0.67f, 0.67f),
	//		Vector3(1.f, 1.f, 1.f),
	//		true);

	//	_ExitText.lock()->SetRenderFlag(true);
	//}

	return S_OK;
}

HRESULT TitlePanel::Awake()
{
	return S_OK;
}

HRESULT TitlePanel::Start()
{
	return S_OK;
}

UINT TitlePanel::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);

	if (1.f > _BrightScale)
	{
		_BrightScale += 2.f * _fDeltaTime;
		if (1.f < _BrightScale)
			_BrightScale = 1.f;
	}

	return 0;
}          

UINT TitlePanel::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void TitlePanel::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
	
	}
}

void TitlePanel::OnEnable()
{
	GameObject::OnEnable();
}

void TitlePanel::OnDisable()
{
	GameObject::OnDisable();
}