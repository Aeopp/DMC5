#include "stdafx.h"
#include "..\Header\LogoPanel.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include "Font.h"


void LogoPanel::Free()
{
	Destroy(_LoadingText);

	GameObject::Free();
}

std::string LogoPanel::GetName()
{
	return "LogoPanel";
}

LogoPanel* LogoPanel::Create()
{
	return new LogoPanel{};
}

void LogoPanel::RenderUI(const DrawInfo& _Info)
{
	if (!_Info.IsAfterPostProcessing)
	{
		auto WeakSubset = _PlaneMesh->GetSubset(0u);
		if (auto SharedSubset = WeakSubset.lock();
			SharedSubset)
		{
			if (_PressReturn && _TextChanged)
				_Info.Fx->SetTexture("ALB0Map", _Wing1Tex->GetTexture());
			else
				_Info.Fx->SetTexture("ALB0Map", _Wing0Tex->GetTexture());

			_Info.Fx->SetFloat("_SliceAmount", 0.f);
			_Info.Fx->SetFloat("_BrightScale", _BrightScale);
			_Info.Fx->SetMatrix("ScreenMat", &_ScreenMat);

			_Info.Fx->BeginPass(0);
			SharedSubset->Render(_Info.Fx);
			_Info.Fx->EndPass();
		}
	}
}

void LogoPanel::RenderReady()
{

}

HRESULT LogoPanel::Ready()
{
	SetRenderEnable(true);

	m_nTag = GAMEOBJECTTAG::UI_LogoPanel;

	ENGINE::RenderProperty _InitRenderProp;
	_InitRenderProp.bRender = true;
	_InitRenderProp.RenderOrders[RenderProperty::Order::UI] =
	{
		{"LogoPanel",
		[this](const DrawInfo& _Info)
			{
				this->RenderUI(_Info);
			}
		},
	};
	RenderInterface::Initialize(_InitRenderProp);

	_PlaneMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx");
	_Wing0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\logo_0.png");
	_Wing1Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\logo_1.png");

	D3DXMatrixIdentity(&_ScreenMat);
	_ScreenMat._11 = 12.8f;
	_ScreenMat._22 = 7.6f;
	_ScreenMat._33 = 1.f;
	_ScreenMat._41 = 0.f;
	_ScreenMat._42 = 0.f;
	_ScreenMat._43 = 0.02f;

	if (_LoadingText = AddGameObject<Font>();
		!_LoadingText.expired())
	{
		_LoadingText.lock()->SetText("Now Loading...",
			Font::TEX_ID::DMC5_GREEN_GRAD,
			Vector2(555.f, 600.f),
			Vector2(0.67f, 0.67f),
			Vector3(1.f, 1.f, 1.f),
			true);

		_LoadingText.lock()->SetRenderFlag(_TextRender);
	}

	return S_OK;
}

HRESULT LogoPanel::Awake()
{
	return S_OK;
}

HRESULT LogoPanel::Start()
{
	return S_OK;
}

UINT LogoPanel::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);

	_TextBlickTick += _fDeltaTime;
	if (!_PressReturn || !_TextChanged)
	{
		if (_TextRender)
		{
			if (1.5f < _TextBlickTick)
			{
				_TextRender = false;
				_LoadingText.lock()->SetRenderFlag(_TextRender, Font::FADE_ID::ALPHA_LINEAR);
				_TextBlickTick = 0.f;
			}
		}
		else
		{
			if (0.5f < _TextBlickTick)
			{
				_TextRender = true;
				_LoadingText.lock()->SetRenderFlag(_TextRender, Font::FADE_ID::ALPHA_LINEAR);
				_TextBlickTick = 0.f;
			}
		}
	}

	if (!_LoadingFinished)
	{
		if (1.f > _BrightScale)
		{
			_BrightScale += 2.f * _fDeltaTime;
			if (1.f < _BrightScale)
				_BrightScale = 1.f;
		}
	}
	else
	{
		if (!_PressReturn)
		{
			if (Input::GetKeyDown(DIK_RETURN))
			{
				_LoadingText.lock()->SetRenderFlag(false);
				_BrightScale = 2.5f;
				_PressReturn = true;
			}

			if (!_TextChanged)
			{
				_LoadingText.lock()->SetText("Press Enter",
					Font::TEX_ID::DMC5_GREEN_GRAD,
					Vector2(560.f, 600.f),
					Vector2(0.67f, 0.67f),
					Vector3(1.f, 1.f, 1.f),
					true);
				_TextChanged = true;
			}
		}
		else
		{
			if (!_TextChanged)	// QuickProgressToNextScene()로 건너뛰었을 경우
			{
				_LoadingText.lock()->SetRenderFlag(false);
				_BrightScale = 2.5f;
				_TextChanged = true;
			}

			if (0.f < _BrightScale)
			{
				_BrightScale -= 1.5f * _fDeltaTime;
				if (0.f >= _BrightScale)
				{
					_ReadyToNextScene = true;
					//_BrightScale = 0.f;
				}
			}
		}
	}

	return 0;
}          

UINT LogoPanel::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void LogoPanel::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
	
	}
}

void LogoPanel::OnEnable()
{
	GameObject::OnEnable();
}

void LogoPanel::OnDisable()
{
	GameObject::OnDisable();
}