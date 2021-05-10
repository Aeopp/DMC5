#include "stdafx.h"
#include "..\Header\TitlePanel.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"


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

}

HRESULT TitlePanel::Ready()
{
	SetRenderEnable(true);

	m_nTag = GAMEOBJECTTAG::UI_TitlePanel;

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