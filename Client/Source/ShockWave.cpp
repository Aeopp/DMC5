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
		const float LerpT = std::sinf(FMath::Clamp((T / EndT), 0.f, 1.f) * FMath::HalfPI);
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
};


void ShockWave::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("matWorld", &_RenderUpdateInfo.World);
	
	const float LerpT = std::sinf(FMath::Clamp((T / EndT),0.f,1.f) * FMath::HalfPI);
	_Info.Fx->SetFloat("BlurIntencity", FMath::Lerp(BlurIntencity.first, BlurIntencity.second, LerpT));
	_Info.Fx->SetFloat("DistortionIntencity", DistortionIntencity);
	_Info.Fx->SetFloat("BlurMaxLength", BlurMaxLength);
	_Info.Fx->SetFloat("BlurAlpha", FMath::Lerp(BlurAlpha.first, BlurAlpha.second, LerpT));
	_Info.Fx->SetFloat("DistortionAlpha", DistortionAlpha);

	_Info.Fx->SetVector("_Color", &Color);
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
				/*if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
				{
					continue;
				}*/

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
	InitTransform.lock()->SetScale(Vector3{ 0.001f,0.001f,0.001f });

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
			if (ImGui::Button("Play"))
			{
				PlayStart(GetComponent<Transform>().lock()->GetPosition());
			}

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

void ShockWave::PlayStart(const Vector3& PlayLocation)
{
	_RenderProperty.bRender = true;
	T = 0.0f;

	if (auto SpTransform = GetComponent<Transform>().lock();
		SpTransform)
	{
		SpTransform->SetPosition(PlayLocation);
	}
};

void ShockWave::PlayEnd()
{
	_RenderProperty.bRender = false;
	T = 0.0f;
};


