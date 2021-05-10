#include "stdafx.h"
#include "..\Header\TitlePanel.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include "Font.h"

void TitlePanel::Free()
{
	Destroy(_NewGameText);
	Destroy(_ExitText);

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
			if (_ShowLoadingProgress)
			{
				// 로딩바
				_Info.Fx->SetTexture("ALB0Map", _LoadingBaseTex->GetTexture());
				_Info.Fx->SetFloat("_SliceAmount", 0.f);
				_Info.Fx->SetFloat("_BrightScale", _BrightScale);
				_Info.Fx->SetMatrix("ScreenMat", &_TitleScreenMat);
				_Info.Fx->SetFloatArray("_MinTexUV", Vector2(0.f, 0.f), 2u);
				_Info.Fx->SetFloatArray("_MaxTexUV", Vector2(1.f, 1.f), 2u);

				_Info.Fx->BeginPass(0);
				SharedSubset->Render(_Info.Fx);
				_Info.Fx->EndPass();
			}
			else
			{
				// 타이틀 화면
				_Info.Fx->SetTexture("ALB0Map", _TitleTex->GetTexture());
				_Info.Fx->SetFloat("_SliceAmount", 0.f);
				_Info.Fx->SetFloat("_BrightScale", _BrightScale);
				_Info.Fx->SetMatrix("ScreenMat", &_TitleScreenMat);
				_Info.Fx->SetFloatArray("_MinTexUV", Vector2(0.f, 0.f), 2u);
				_Info.Fx->SetFloatArray("_MaxTexUV", Vector2(1.f, 1.f), 2u);

				_Info.Fx->BeginPass(0);
				SharedSubset->Render(_Info.Fx);
				_Info.Fx->EndPass();

				_Info.Fx->SetTexture("ALB0Map", _SelectTex->GetTexture());
				_Info.Fx->SetFloat("_SliceAmount", 0.f);
				_Info.Fx->SetFloat("_BrightScale", _BrightScale);
				_Info.Fx->SetMatrix("ScreenMat", &_SelectScreenMat);
				_Info.Fx->SetFloatArray("_MinTexUV", _SelectUVMin, 2u);
				_Info.Fx->SetFloatArray("_MaxTexUV", _SelectUVMax, 2u);

				_Info.Fx->BeginPass(0);
				SharedSubset->Render(_Info.Fx);
				_Info.Fx->EndPass();
			}
		}
	}
}

void TitlePanel::Update_TitleLogic(const float _fDeltaTime)
{
	if (!_PressReturn)
	{
		if (1.f > _BrightScale)
		{
			_BrightScale += 2.f * _fDeltaTime;
			if (1.f < _BrightScale)
				_BrightScale = 1.f;
		}

		if (0u != _CurState && Input::GetKeyDown(DIK_RETURN))
		{
			_PressReturn = true;
		}
		else if (Input::GetKeyDown(DIK_UPARROW) || Input::GetKeyDown(DIK_RIGHTARROW))
		{
			if (2u == _CurState || 0u == _CurState)
			{
				if (!_NewGameText.expired())
				{
					_NewGameText.lock()->SetText("NEW GAME",
						Font::TEX_ID::DMC5_WHITE,
						Vector2(590.f, 510.f),
						Vector2(0.55f, 0.55f),
						Vector3(0.25f, 0.654f, 0.654f),
						false);
				}
				if (!_ExitText.expired())
				{
					_ExitText.lock()->SetText("EXIT",
						Font::TEX_ID::DMC5_WHITE,
						Vector2(620.f, 555.f),
						Vector2(0.55f, 0.55f),
						Vector3(0.654f, 0.654f, 0.654f),
						false);
				}

				_SelectUVMin = Vector2(0.f, 0.007812f);
				_SelectUVMax = Vector2(0.970703f, 0.210937f);
				_SelectScreenMat._42 = -150.f;
				_CurState = 1u;
			}
		}
		else if (Input::GetKeyDown(DIK_DOWNARROW) || Input::GetKeyDown(DIK_LEFTARROW))
		{
			if (1u == _CurState || 0u == _CurState)
			{
				if (!_NewGameText.expired())
				{
					_NewGameText.lock()->SetText("NEW GAME",
						Font::TEX_ID::DMC5_WHITE,
						Vector2(590.f, 510.f),
						Vector2(0.55f, 0.55f),
						Vector3(0.654f, 0.654f, 0.654f),
						false);
				}
				if (!_ExitText.expired())
				{
					_ExitText.lock()->SetText("EXIT",
						Font::TEX_ID::DMC5_WHITE,
						Vector2(620.f, 555.f),
						Vector2(0.55f, 0.55f),
						Vector3(0.25f, 0.654f, 0.654f),
						false);
				}

				_SelectUVMin = Vector2(0.f, 0.472656f);
				_SelectUVMax = Vector2(0.970703f, 0.667968f);
				_SelectScreenMat._42 = -195.f;
				_CurState = 2u;
			}
		}
	}
	else
	{
		_AccTime += _fDeltaTime;

		if (4u <= _TextBlinkCnt)
		{
			if (0.f < _BrightScale)
			{
				_BrightScale -= 2.f * _fDeltaTime;

				if (0.5f > _BrightScale && 4u == _TextBlinkCnt)
				{
					_NewGameText.lock()->SetRenderFlag(false);
					_ExitText.lock()->SetRenderFlag(false);
					++_TextBlinkCnt;
				}

				if (0.f > _BrightScale)
				{
					//_BrightScale = 0.f;

					if (2u == _CurState)
					{
						PostQuitMessage(0);
						// + 로딩스레드 강제 종료 ?
					}
					else
					{
						//_ReadyToNextScene = true;	 << 로딩바 꽉차면 트루
						_ShowLoadingProgress = true;
						_AccTime = 0.f;
					}
				}
			}
		}
		else if (0.1f < _AccTime && 4u > _TextBlinkCnt)
		{
			_TextRender = !_TextRender;

			if (1u == _CurState)
			{
				if (!_NewGameText.expired())
				{
					_NewGameText.lock()->SetRenderFlag(_TextRender);
				}
			}
			else
			{
				if (!_ExitText.expired())
				{
					_ExitText.lock()->SetRenderFlag(_TextRender);
				}
			}

			++_TextBlinkCnt;
			_AccTime = 0.f;
		}
	}
}

void TitlePanel::Update_LoadingLogic(const float _fDeltaTime)
{
	if (1.f > _BrightScale)
	{
		_BrightScale += 2.f * _fDeltaTime;
		if (1.f < _BrightScale)
			_BrightScale = 1.f;
	}

	// 로딩 완료
	if (1.f <= _LoadingProgress)
	{
		//if (0.f < _BrightScale)
		//{
		//	_BrightScale -= 2.f * _fDeltaTime;
		//	if (0.f >= _BrightScale)
		//	{
				_ReadyToNextScene = true;
		//	}
		//}
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
	_SelectTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\ui4000_iam.tga");
	
	// 임시
	_LoadingBaseTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\UI\\logo_0.png");

	_SelectUVMin = Vector2(0.f, 0.f);
	_SelectUVMax = Vector2(0.f, 0.f);

	D3DXMatrixIdentity(&_TitleScreenMat);
	_TitleScreenMat._11 = 12.8f;
	_TitleScreenMat._22 = 7.6f;
	_TitleScreenMat._33 = 1.f;
	_TitleScreenMat._41 = 0.f;
	_TitleScreenMat._42 = 0.f;
	_TitleScreenMat._43 = 0.02f;

	D3DXMatrixIdentity(&_SelectScreenMat);
	_SelectScreenMat._11 = 12.8f;
	_SelectScreenMat._22 = 0.4f;
	_SelectScreenMat._33 = 1.f;
	_SelectScreenMat._41 = 0.f;
	_SelectScreenMat._42 = -150.f;
	_SelectScreenMat._43 = 0.02f;
	
	if (_NewGameText = AddGameObject<Font>();
		!_NewGameText.expired())
	{
		_NewGameText.lock()->SetText("NEW GAME",
			Font::TEX_ID::DMC5_WHITE,
			Vector2(590.f, 510.f),
			Vector2(0.55f, 0.55f),
			Vector3(0.654f, 0.654f, 0.654f),
			false);

		_NewGameText.lock()->SetRenderFlag(true);
	}
	if (_ExitText = AddGameObject<Font>();
		!_ExitText.expired())
	{
		_ExitText.lock()->SetText("EXIT",
			Font::TEX_ID::DMC5_WHITE,
			Vector2(620.f, 555.f),
			Vector2(0.55f, 0.55f),
			Vector3(0.654f, 0.654f, 0.654f),
			false);

		_ExitText.lock()->SetRenderFlag(true);
	}

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
	
	if (_ShowLoadingProgress)
		Update_LoadingLogic(_fDeltaTime);
	else
		Update_TitleLogic(_fDeltaTime);

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