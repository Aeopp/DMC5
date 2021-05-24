#include "stdafx.h"
#include "..\Header\ShockWave.h"
#include "Transform.h"
#include "Subset.h"
#include "Color.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "GraphicSystem.h"
#include "RedQueen.h"
#include "ParticleSystem.h"
#include "FLight.h"

ShockWave::ShockWave()
{

};

void ShockWave::Free()
{
	GameObject::Free();
};

std::string ShockWave::GetName()
{
	return "ShockWave ";
};

ShockWave* ShockWave::Create()
{
	return new ShockWave{};
};

void ShockWave::RenderReady()
{
	// 이미 버텍스 자체가 월드 위치임 . 

	if (auto SpTransform = GetComponent<Transform>().lock();
		SpTransform)
	{
		const float LerpT = std::sinf(FMath::Clamp((T / EndT), 0.f, 1.f) * FMath::PI);
		const float _Scale = FMath::Lerp(ScaleLerp.first, ScaleLerp.second, LerpT);
		SpTransform->SetScale(Vector3{ _Scale ,_Scale ,_Scale } );

		_RenderUpdateInfo.World = 	
			FMath::Scale(SpTransform->GetScale())
			* Renderer::GetInstance()->_RenderInfo.Billboard 
			* FMath::Translation(SpTransform->GetPosition());
	}
};

void ShockWave::RenderInit()
{
	m_nTag = Eff_ShockWave;
	// 렌더를 수행해야하는 오브젝트라고 (렌더러에 등록 가능) 알림.
	// 렌더 인터페이스 상속받지 않았다면 키지마세요.
	SetRenderEnable(true);

	// 렌더 정보 초기화 ...
	ENGINE::RenderProperty _InitRenderProp;
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	// 렌더 속성 전체 초기화 
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	_InitRenderProp.bRender = false;

	_InitRenderProp.RenderOrders[RenderProperty::Order::Debug]=
	{
		{"Debug" ,[this](const DrawInfo& _Info)
			{
				RenderDebug(_Info);
			}
	} };

	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{
		{"ShockWave",[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		}
	};

	RenderInterface::Initialize(_InitRenderProp);
	//magic_eletric00.fbx
	Mesh::InitializeInfo _InitInfo{};
	_InitInfo.bLocalVertexLocationsStorage = true;
	// 메쉬 바꾸셈.

	Mesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\pipe02.fbx", _InitInfo);

	BlurMskMap = Resources::Load<Texture>("..\\..\\Usable\\BlurMsk.tga");

	_PlaneMesh = Resources::Load<StaticMesh>(
		"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _InitInfo);

	
}; 


void ShockWave::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("matWorld", &_RenderUpdateInfo.World);
	
	const float LerpT = std::sinf(FMath::Clamp((T / EndT),0.f,1.f) * FMath::PI);
	_Info.Fx->SetFloat("BlurIntencity", FMath::Lerp(BlurIntencity.first, BlurIntencity.second, LerpT));
	_Info.Fx->SetFloat("DistortionIntencity", DistortionIntencity);
	_Info.Fx->SetFloat("BlurMaxLength", BlurMaxLength);
	_Info.Fx->SetFloat("BlurAlpha", FMath::Lerp(BlurAlpha.first, BlurAlpha.second, LerpT));
	
	_Info.Fx->SetFloat("DistortionAlpha", DistortionAlpha);
	_Info.Fx->SetVector("_Color", &Color); 
	_Info.Fx->SetBool("bBlurMsk", bBlurMsk);
	_Info.Fx->SetTexture("BlurMskMap", BlurMskMap->GetTexture());

	if (bDirectionReverse==false)
		_Info.Fx->SetFloat("DirectionFactor", 1.f);
	else
		_Info.Fx->SetFloat("DirectionFactor", -1.f);

	if (bBlurMsk)
	{
		const uint32 SubsetCnt = _PlaneMesh->GetNumSubset();

		for (uint32 i = 0; i < SubsetCnt; ++i)
		{
			if (auto SpSubset = _PlaneMesh->GetSubset(i).lock();
				SpSubset)
			{
				SpSubset->Render(_Info.Fx);
			};
		};
	}
	else
	{
		const uint32 SubsetCnt = Mesh->GetNumSubset();

		for (uint32 i = 0; i < SubsetCnt; ++i)
		{
			if (auto SpSubset = Mesh->GetSubset(i).lock();
				SpSubset)
			{
				SpSubset->Render(_Info.Fx);
			};
		};
	}
};


void ShockWave::RenderDebug(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("World", &_RenderUpdateInfo.World);


	{
		const uint32 Numsubset = Mesh->GetNumSubset();
		for (uint32 i = 0; i < Numsubset; ++i)
		{
			if (auto SpSubset = Mesh->GetSubset(i).lock();
				SpSubset)
			{
				SpSubset->Render(_Info.Fx);
			};
		};
	}

};


HRESULT ShockWave::Ready()
{
	auto InitTransform = GetComponent<ENGINE::Transform>();

	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	return S_OK;
};

HRESULT ShockWave::Awake()
{
	GameObject::Awake();
	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetPosition(Vector3{ 0.f,0.25f,0.f } );
	InitTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	InitTransform.lock()->SetScale(Vector3{ 0.002f ,0.002f ,0.002f });

	return S_OK;
}

HRESULT ShockWave::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT ShockWave::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	if (_RenderProperty.bRender == false) return 0;
	T += _fDeltaTime;

	if (T > PlayTime)
	{
		PlayEnd();
		return 0;
	}

	return 0;
}

UINT ShockWave::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	if (_RenderProperty.bRender == false) return 0;

	return 0;
};

void ShockWave::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		const std::string ChildName = GetName() + "_Play";
		ImGui::BeginChild(ChildName.c_str());
		{
			static bool bEditBlurMsk = false;

			if (ImGui::Button("Play"))
			{
				PlayStart(GetComponent<Transform>().lock()->GetPosition(),EditOption ,bEditBlurMsk);
			}
			
			ImGui::Checkbox("BlurMsk", &bEditBlurMsk);
			ImGui::Checkbox
			("bDirectionReverse", &bDirectionReverse);
			
			
			ImGui::SliderInt("Option", &EditOption, 0, Option::None);

			ImGui::SliderFloat("PlayTime" ,&PlayTime,0.f, 100.f);
			ImGui::SliderFloat("EndT", &EndT, 0.f, 100.f);

			ImGui::SliderFloat("DistortionIntencity", &DistortionIntencity, 0.f, 1.f);

			ImGui::SliderFloat("ScaleLerp Begin", (float*)&ScaleLerp.first, 0.f, 1.f);
			ImGui::SliderFloat("ScaleLerp End", (float*)&ScaleLerp.second, 0.f, 1.f);

			ImGui::SliderFloat("BlurIntencity Begin", (float*)&BlurIntencity.first, 0.f, 10.f);
			ImGui::SliderFloat("BlurIntencity End", (float*)&BlurIntencity.second, 0.f, 10.f);

			ImGui::SliderFloat("BlurAlphaLerp Begin", (float*)&BlurAlpha.first, 0.f, 10.f);
			ImGui::SliderFloat("BlurAlphaLerp End", (float*)&BlurAlpha.second, 0.f, 10.f);

			ImGui::SliderFloat("BlurMaxLength", &BlurMaxLength, 0.f, 1000.f);
			
			ImGui::SliderFloat("DistortionAlpha ", &DistortionAlpha, 0.f, 1.f);
			ImGui::ColorEdit4("Color", Color);
		}
		ImGui::EndChild();
	}
};

void ShockWave::OnEnable()
{
	GameObject::OnEnable();
};

void ShockWave::OnDisable()
{
	GameObject::OnDisable();
};

void ShockWave::PlayStart(const Vector3& PlayLocation ,
						  const int32& _Option ,
					       const bool bBlurMsk)
{
	_RenderProperty.bRender = true;
	T = 0.0f;
	this->bBlurMsk = bBlurMsk;
	if (auto SpTransform = GetComponent<Transform>().lock();
		SpTransform)
	{
		SpTransform->SetPosition(PlayLocation);
	}

	switch (_Option)
	{
	case Option::None:
		break;
	case Option::Weak:
		PlayTime = 0.204f;
		EndT = 0.204f;
		DistortionIntencity = 0.000f;
		ScaleLerp.first = 0.001f;
		ScaleLerp.second = 0.004f;
		BlurIntencity.first = 0.467f;
		BlurIntencity.second = 1.1f;
		BlurAlpha.first = 0.000f;
		BlurAlpha.second = 0.711f;
		BlurMaxLength = 65.f;
		DistortionAlpha = 0.0f;
		Color = { 0.0f /255.f , 0.0f / 255.f ,0.0f / 255.f ,0.0f / 255.f };
		break;
	case Option::Middle:
		PlayTime = 0.407f;
		EndT = 0.407f;
		DistortionIntencity = 0.000f;
		ScaleLerp.first =  0.000f;
		ScaleLerp.second = 0.022f;
		BlurIntencity.first = 0.854f;
		BlurIntencity.second = 1.179f;
		BlurAlpha.first = 0.285f;
		BlurAlpha.second = 0.589f;
		BlurMaxLength = 35.f;
		DistortionAlpha = 0.0f;
		Color = { 0.0f / 255.f , 0.0f / 255.f ,0.0f / 255.f ,0.0f / 255.f };
		break;
	case Option::Strong:

		break;
	case Option::GoliathPunch:
		PlayTime = 0.409f;
		EndT = 0.409f;
		DistortionIntencity = 0.000f;
		ScaleLerp.first = 0.178f;
		ScaleLerp.second = 0.219f;
		BlurIntencity.first = 1.104f;
		BlurIntencity.second = 1.186f;
		BlurAlpha.first = 0.675f;
		BlurAlpha.second = 1.f;
		BlurMaxLength = 40.9f;
		DistortionAlpha = 0.0f;
		Color = { 0.f / 255.f , 0.f / 255.f ,0.f / 255.f ,9.f / 255.f };
		break;
	case Option::GoliathJumpAttack:
		PlayTime = 0.613f;
		EndT = 0.613f;
		DistortionIntencity = 0.000f;
		ScaleLerp.first = 0.002f;
		ScaleLerp.second = 0.076f;
		BlurIntencity.first = 0.700f;
		BlurIntencity.second = 1.002f;
		BlurAlpha.first = 0.757f;
		BlurAlpha.second = 0.900f;
		BlurMaxLength = 40.900f;
		DistortionAlpha = 0.0f;
		Color = { 5.f / 255.f , 0.f / 255.f ,0.f / 255.f ,9.f / 255.f };
		break;
	case Option::Change:
		PlayTime = 1.f;
		EndT = 1.f;
		DistortionIntencity = 0.000f;
		ScaleLerp.first = 0.0f;
		ScaleLerp.second = 0.307f;
		BlurIntencity.first = 0.0f;
		BlurIntencity.second = 2.049f;
		BlurAlpha.first = 0.675f;
		BlurAlpha.second = 1.0f;
		BlurMaxLength = 40.9f;
		DistortionAlpha = 0.0f;
		Color = { 0.0f / 255.f , 0.0f / 255.f ,0.0f / 255.f ,0.0f / 255.f };
		this->bBlurMsk = true;
		break;
	case Option::Hit:
		PlayTime = 0.55f;
		EndT = 0.55f;
		DistortionIntencity = 0.000f;
		ScaleLerp.first = 0.0f;
		ScaleLerp.second = 0.0075f;
		BlurIntencity.first = 0.0f;
		BlurIntencity.second = 4.0f;
		BlurAlpha.first = 0.0f;
		BlurAlpha.second = 0.40f;
		BlurMaxLength = 70.f;
		DistortionAlpha = 0.0f;
		Color = { 0.0f/ 255.f , 0.0f / 255.f ,0.0f / 255.f ,0.0f / 255.f };
		this->bBlurMsk = true;
		this->bDirectionReverse = true;
		break;
	case Option::SnatchRush:
		PlayTime = 0.8f;
		EndT = 0.8f;
		DistortionIntencity = 0.000f;
		ScaleLerp.first = 0.0f;
		ScaleLerp.second = 0.307f;
		BlurIntencity.first = 0.0f;
		BlurIntencity.second = 2.049f;
		BlurAlpha.first = 0.675f;
		BlurAlpha.second = 1.0f;
		BlurMaxLength = 40.9f;
		DistortionAlpha = 0.0f;
		Color = { 0.0f / 255.f , 0.0f / 255.f ,0.0f / 255.f ,0.0f / 255.f };
		this->bBlurMsk = true;
		break;
	case Option::ArtemisRush:
		PlayTime = 0.8f;
		EndT = 0.8f;
		DistortionIntencity = 0.f;
		ScaleLerp.first = 0.002f;
		ScaleLerp.second = 0.07f;
		BlurIntencity.first = 0.7f;
		BlurIntencity.second = 1.f;
		BlurAlpha.first = 0.5f;
		BlurAlpha.second = 0.7f;
		BlurMaxLength = 40.9f;
		DistortionAlpha = 0.0f;
		Color = { 0.f / 255.f , 0.f / 255.f ,0.f / 255.f ,9.f / 255.f };
		break;
	case Option::ArtemisLaunch:
		PlayTime = 0.409f;
		EndT = 0.409f;
		DistortionIntencity = 0.000f;
		ScaleLerp.first = 0.000f;
		ScaleLerp.second = 0.022f;
		BlurIntencity.first = 0.700f;
		BlurIntencity.second = 1.002f;
		BlurAlpha.first = 0.300f;
		BlurAlpha.second = 0.650f;
		BlurMaxLength = 40.900f;
		DistortionAlpha = 0.0f;
		Color = { 0.f / 255.f , 0.f / 255.f ,0.f / 255.f ,9.f / 255.f };
		break;
	case Option::ArtemisEnd:
		PlayTime = 1.5f;
		EndT = 1.5f;
		DistortionIntencity = 0.000f;
		ScaleLerp.first = 0.000f;
		ScaleLerp.second = 0.3f;
		BlurIntencity.first = 0.0f;
		BlurIntencity.second = 1.f;
		BlurAlpha.first = 0.0f;
		BlurAlpha.second = 0.4f;
		BlurMaxLength = 27.833f;
		DistortionAlpha = 1.f;
		Color = { 13.f / 255.f , 3.5f/ 255.f ,10.f / 255.f ,5.f / 255.f };
		this->bBlurMsk = true;
		this->bDirectionReverse = true;
		break;
	case StreakEnd:
		PlayTime = 0.6f;
		EndT = 0.6f;
		DistortionIntencity = 0.000f;
		ScaleLerp.first = 0.0f;
		ScaleLerp.second = 0.0125f;
		BlurIntencity.first = 0.0f;
		BlurIntencity.second = 4.0f;
		BlurAlpha.first = 0.35f;
		BlurAlpha.second = 0.35f;
		BlurMaxLength = 70.f;
		DistortionAlpha = 0.0f;
		Color = { 0.0f / 255.f , 0.0f / 255.f ,0.0f / 255.f ,0.0f / 255.f };
		this->bBlurMsk = true;
		this->bDirectionReverse = true;
		break;
	case Streak:
		PlayTime = 0.6f;
		EndT = 0.6f;
		DistortionIntencity = 0.000f;
		ScaleLerp.first = 0.0f;
		ScaleLerp.second = 0.0125f;
		BlurIntencity.first = 0.0f;
		BlurIntencity.second = 4.0f;
		BlurAlpha.first = 0.35f;
		BlurAlpha.second = 0.35f;
		BlurMaxLength = 70.f;
		DistortionAlpha = 0.0f;
		Color = { 0.0f / 255.f , 0.0f / 255.f ,0.0f / 255.f ,0.0f / 255.f };
		this->bBlurMsk = true;
		this->bDirectionReverse = true;
		break;
	case SplitEnd:
		PlayTime = 0.6f;
		EndT = 0.6f;
		DistortionIntencity = 0.000f;
		ScaleLerp.first = 0.0f;
		ScaleLerp.second = 0.0125f;
		BlurIntencity.first = 0.0f;
		BlurIntencity.second = 4.0f;
		BlurAlpha.first = 0.35f;
		BlurAlpha.second = 0.35f;
		BlurMaxLength = 70.f;
		DistortionAlpha = 0.0f;
		Color = { 0.0f / 255.f , 0.0f / 255.f ,0.0f / 255.f ,0.0f / 255.f };
		this->bBlurMsk = true;
		this->bDirectionReverse = true;
		break;
	case Split:
		PlayTime = 0.6f;
		EndT = 0.6f;
		DistortionIntencity = 0.000f;
		ScaleLerp.first = 0.0f;
		ScaleLerp.second = 0.0125f;
		BlurIntencity.first = 0.0f;
		BlurIntencity.second = 4.0f;
		BlurAlpha.first = 0.35f;
		BlurAlpha.second = 0.35f;
		BlurMaxLength = 70.f;
		DistortionAlpha = 0.0f;
		Color = { 0.0f / 255.f , 0.0f / 255.f ,0.0f / 255.f ,0.0f / 255.f };
		this->bBlurMsk = true;
		this->bDirectionReverse = true;
		break;
	case Buster:
		PlayTime = 1.f;
		EndT = 1.f;
		DistortionIntencity = 0.000f;
		ScaleLerp.first = 0.0f;
		ScaleLerp.second = 0.0125f;
		BlurIntencity.first = 0.0f;
		BlurIntencity.second = 4.0f;
		BlurAlpha.first = 0.35f;
		BlurAlpha.second = 0.35f;
		BlurMaxLength = 70.f;
		DistortionAlpha = 0.0f;
		Color = { 0.0f / 255.f , 0.0f / 255.f ,0.0f / 255.f ,0.0f / 255.f };
		this->bBlurMsk = true;
		this->bDirectionReverse = true;
		break;
	case Kaboom:
		PlayTime = 1.5f;
		EndT = 1.5f;
		DistortionIntencity = 0.000f;
		ScaleLerp.first = 0.0f;
		ScaleLerp.second = 0.1f;
		BlurIntencity.first = 0.0f;
		BlurIntencity.second = 8.f;
		BlurAlpha.first = 0.38f;
		BlurAlpha.second = 0.38f;
		BlurMaxLength = 70.f;
		DistortionAlpha = 0.0f;
		Color = { 1.83f / 255.f , 0.91f / 255.f ,1.6f / 255.f ,0.0f / 255.f };
		this->bBlurMsk = true;
		this->bDirectionReverse = true;
		break;
	default:
		break;
	}
};

void ShockWave::PlayEnd()
{
	_RenderProperty.bRender = false;
	T = 0.0f;
};


