#include "stdafx.h"
#include "..\Header\LoadingPanel.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include "Font.h"

void LoadingPanel::Free()
{
	Destroy(_LoadingText);

	GameObject::Free();
}

std::string LoadingPanel::GetName()
{
	return "LoadingPanel";
}

LoadingPanel* LoadingPanel::Create()
{
	return new LoadingPanel{};
}

void LoadingPanel::RenderUI(const DrawInfo& _Info)
{
	if (!_Info.IsAfterPostProcessing)
	{
		auto WeakSubset = _PlaneMesh->GetSubset(0u);
		if (auto SharedSubset = WeakSubset.lock();
			SharedSubset)
		{
			if (1.f <= _LoadingProgress)
				_Info.Fx->SetTexture("ALB0Map", _Wing1Tex->GetTexture());
			else
				_Info.Fx->SetTexture("ALB0Map", _Wing0Tex->GetTexture());

			_Info.Fx->SetFloat("_SliceAmount", 0.f);
			_Info.Fx->SetFloat("_BrightScale", _BrightScale);
			_Info.Fx->SetMatrix("ScreenMat", &_BaseScreenMat);
			_Info.Fx->SetFloatArray("_MinTexUV", Vector2(0.f, 0.f), 2u);
			_Info.Fx->SetFloatArray("_MaxTexUV", Vector2(1.f, 1.f), 2u);

			_Info.Fx->BeginPass(0);
			SharedSubset->Render(_Info.Fx);
			_Info.Fx->EndPass();

			_Info.Fx->SetTexture("ATOS0Map", _LoadingbarATOSTex->GetTexture());
			//_Info.Fx->SetFloat("_SliceAmount", 0.f);
			_Info.Fx->SetFloat("_BrightScale", 0.001f);
			_Info.Fx->SetMatrix("ScreenMat", &_LoadingbarScreenMat);
			_Info.Fx->SetFloatArray("_MinTexUV", Vector2(0.f, 0.f), 2u);
			_Info.Fx->SetFloatArray("_MaxTexUV", Vector2(1.f, 1.f), 2u);

			_Info.Fx->BeginPass(1);
			SharedSubset->Render(_Info.Fx);
			_Info.Fx->EndPass();

			Matrix ScreenMat = _LoadingbarScreenMat;
			ScreenMat._11 *= 0.8f;
			_Info.Fx->SetMatrix("ScreenMat", &ScreenMat);
			_Info.Fx->SetFloat("_BrightScale", 0.4f);
			_Info.Fx->SetFloat("_AccumulationTexU", _NoiseAccTime * 0.1f);
			_Info.Fx->SetFloat("_LoadingbarCurXPosOrtho", _LoadingbarCurXPosOrtho);

			_Info.Fx->BeginPass(2);
			SharedSubset->Render(_Info.Fx);
			_Info.Fx->EndPass();

			if (1.f > _LoadingProgress)
			{
				_Info.Fx->SetTexture("ALB0Map", _LoadingTextTex->GetTexture());
				_Info.Fx->SetFloat("_SliceAmount", 1.f - _TextAlpha);
				_Info.Fx->SetFloat("_BrightScale", _BrightScale);
				_Info.Fx->SetMatrix("ScreenMat", &_LoadingTextScreenMat);
				_Info.Fx->SetFloatArray("_MinTexUV", Vector2(0.f, 0.0625f * static_cast<float>(_CurTextIdx)), 2u);
				_Info.Fx->SetFloatArray("_MaxTexUV", Vector2(1.f, 0.0625f * static_cast<float>(_CurTextIdx + 1u)), 2u);

				_Info.Fx->BeginPass(0);
				SharedSubset->Render(_Info.Fx);
				_Info.Fx->EndPass();
			}
		}
	}
}

Vector2 LoadingPanel::ScreenPosToOrtho(float _ScreenPosX, float _ScreenPosY)
{
	Vector2 Ret = Vector2(_ScreenPosX - (g_nWndCX >> 1), -(_ScreenPosY - (g_nWndCY >> 1)));
	D3DXVec2TransformCoord(&Ret, &Ret, &Renderer::GetInstance()->_RenderInfo.Ortho);
	return Ret;
}

void LoadingPanel::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto SpTransform = _WeakTransform.lock();
		SpTransform)
	{
		_RenderUpdateInfo.World = SpTransform->GetRenderMatrix();
	}
}

HRESULT LoadingPanel::Ready()
{
	SetRenderEnable(true);

	m_nTag = GAMEOBJECTTAG::UI_LoadingPanel;

	//auto InitTransform = GetComponent<ENGINE::Transform>();
	//InitTransform.lock()->SetPosition({ 0.f, 0.f, 0.f });

	ENGINE::RenderProperty _InitRenderProp;
	_InitRenderProp.bRender = true;
	_InitRenderProp.RenderOrders[RenderProperty::Order::UI] =
	{
		{"LoadingPanel",
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
	_LoadingbarATOSTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\loadingbar_atos.tga");

	D3DXMatrixIdentity(&_BaseScreenMat);
	_BaseScreenMat._11 = 12.8f;
	_BaseScreenMat._22 = 7.6f;
	_BaseScreenMat._33 = 1.f;
	_BaseScreenMat._41 = 0.f;
	_BaseScreenMat._42 = 0.f;
	_BaseScreenMat._43 = 0.02f;

	D3DXMatrixIdentity(&_LoadingbarScreenMat);
	_LoadingbarScreenMat._11 = 12.8f;
	_LoadingbarScreenMat._22 = 7.6f;
	_LoadingbarScreenMat._33 = 1.f;
	Matrix RotMat;
	D3DXMatrixRotationZ(&RotMat, D3DXToRadian(180.f));
	_LoadingbarScreenMat *= RotMat;
	_LoadingbarScreenMat._41 = 0.f;
	_LoadingbarScreenMat._42 = -280.f;
	_LoadingbarScreenMat._43 = 0.02f;
		
	if (_LoadingText = AddGameObject<Font>();
		!_LoadingText.expired())
	{
		_LoadingText.lock()->SetText("Now Loading...",
			Font::TEX_ID::DMC5_GREEN_GRAD,
			Vector2(555.f, 600.f),
			Vector2(0.67f, 0.67f),
			Vector3(1.f, 1.f, 1.f),
			true);

		_LoadingText.lock()->SetRenderFlag(false);
	}

	_LoadingTextTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\Loading\\LoadingSceneText.png");
	_CurTextIdx = FMath::Random<uint32>(0u, _TextMaxCount - 1u);
	D3DXMatrixIdentity(&_LoadingTextScreenMat);
	_LoadingTextScreenMat._11 = 10.24f;
	_LoadingTextScreenMat._22 = 0.64f;
	_LoadingTextScreenMat._33 = 1.f;
	_LoadingTextScreenMat._41 = 0.f;
	_LoadingTextScreenMat._42 = 280.f;
	_LoadingTextScreenMat._43 = 0.02f;

	return S_OK;
}

HRESULT LoadingPanel::Awake()
{
	return S_OK;
}

HRESULT LoadingPanel::Start()
{
	return S_OK;
}

UINT LoadingPanel::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	
	_NoiseAccTime += _fDeltaTime;
	float LoadingbarOrthoOffsetToCenter = 0.6875f;
	_LoadingbarCurXPosOrtho = -LoadingbarOrthoOffsetToCenter + (_LoadingProgress * 2.f * LoadingbarOrthoOffsetToCenter);

	if (1.f <= _LoadingProgress)
	{
		// 로딩 완료
		_AccTime += _fDeltaTime;
		if (2.f < _AccTime)
			_ReadyToNextScene = true;

		_BrightScale -= 1.5f * _fDeltaTime;
		if (0.f >= _BrightScale)
			_BrightScale = 0.f;

		_LoadingText.lock()->SetRenderFlag(false);
	}
	else
	{
		if (1.f > _BrightScale)
		{
			_BrightScale += 2.f * _fDeltaTime;
			if (1.f < _BrightScale)
				_BrightScale = 1.f;
		}

		_TextBlickTick += _fDeltaTime;
		if (_TextRender)
		{
			if (1.5f < _TextBlickTick)
			{
				_TextRender = false;
				_LoadingText.lock()->SetRenderFlag(_TextRender, Font::FADE_ID::ALPHA_LINEAR);
				_TextBlickTick = 0.f;
			}

			if (1.f > _TextAlpha)
			{
				_TextAlpha += 2.05f * _fDeltaTime;
				if (1.f < _TextAlpha)
					_TextAlpha = 1.f;
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

			if (0.f < _TextAlpha)
			{
				_TextAlpha -= 2.05f * _fDeltaTime;
				if (0.f >= _TextAlpha)
				{
					++_CurTextIdx;
					if (_TextMaxCount <= _CurTextIdx)
						_CurTextIdx = 0u;

					_TextAlpha = 0.f;
				}
			}
		}
	}

	////
	//POINT pt{};
	//GetCursorPos(&pt);
	//ScreenToClient(g_hWnd, &pt);
	//Vector2 TargetPos = Vector2(static_cast<float>(pt.x), static_cast<float>(pt.y));
	//std::cout << ScreenPosToOrtho(TargetPos.x, TargetPos.y).x << std::endl;

	return 0;
}          

UINT LoadingPanel::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void LoadingPanel::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{

	}
}

void LoadingPanel::OnEnable()
{
	GameObject::OnEnable();
}

void LoadingPanel::OnDisable()
{
	GameObject::OnDisable();
}