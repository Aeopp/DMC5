#include "stdafx.h"
#include "..\Header\Change.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "GraphicSystem.h"
#include "RedQueen.h"
#include "ParticleSystem.h"
#include "ParticleInstanceDesc.hpp"
#include "FLight.h"

Change::Change() {};

void Change::Free()
{
	GameObject::Free();
};

std::string Change::GetName()
{
	return "Change";
};

Change* Change::Create()
{
	return new Change{};
};

void Change::RenderReady()
{
	// 이미 버텍스 자체가 월드 위치임 . 
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
	}
};

void Change::RenderInit()
{
	m_nTag = Eff_Change;
	// 렌더를 수행해야하는 오브젝트라고 (렌더러에 등록 가능) 알림.
	// 렌더 인터페이스 상속받지 않았다면 키지마세요.
	SetRenderEnable(true);

	// 렌더 정보 초기화 ...
	ENGINE::RenderProperty _InitRenderProp;
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	// 렌더 속성 전체 초기화 
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	_InitRenderProp.bRender = false;

	_InitRenderProp.RenderOrders[RenderProperty::Order::Debug] =
	{
		{"Debug" ,[this](const DrawInfo& _Info)
			{
				RenderDebug(_Info);
			}
	} };

	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{
		{
			"Change",[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		}

	};

	RenderInterface::Initialize(_InitRenderProp);

	// 메시
	{
		Mesh::InitializeInfo _Info{};
		_Info.bLocalVertexLocationsStorage = true;

		_Mesh = Resources::Load<StaticMesh>(
			"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx");
	}

	// 텍스쳐
	_AlbmMap = Resources::Load<Texture>(
		"..\\..\\Usable\\Judgement\\alb.tga");
	_AlpgMap = Resources::Load<Texture>(
		"..\\..\\Usable\\Judgement\\Magic.tga");
	_MskMap = Resources::Load<Texture>(
		"..\\..\\Usable\\Judgement\\msk.tga");

	_ShockWave = AddGameObject<ShockWave>();

	PushEditEntity(_Mesh.get());
	// 알베도 (알파 무조건 1)
	PushEditEntity(_AlbmMap.get());
	// 마스크 (알파 무조건 1)
	PushEditEntity(_MskMap.get());
	// 알파맵 
	PushEditEntity(_AlpgMap.get());
};

void Change::PlayStart(
	const Vector3& Location)
{
	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
			SpTransform)
		{
			StoneLocation = Location;
			SpTransform->SetPosition(Location);
			SpTransform->SetScale(Vector3{ 0.f,0.f,0.f });
			// 재생 ..
		}
	}

	if (auto SpTransform = GetComponent<Transform>().lock();
		SpTransform)
	{
		if (auto SpShockWave = _ShockWave.lock();
			SpShockWave)
		{
			SpShockWave->PlayStart(SpTransform->GetPosition(), ShockWave::Option::Change);
		}

		if (auto _PtLight = PtLight.lock();
			_PtLight)
		{
			_PtLight->bEnable = false;
		}

		if (PtLight = Renderer::GetInstance()->RefRemainingDynamicLight();
			PtLight.expired() == false)
		{
			auto SpPtLight = PtLight.lock();
			SpPtLight->SetPosition
			(FMath::ConvertVector4(
				SpTransform->GetPosition(),
				1.f
			));
			SpPtLight->bEnable = true;
			SpPtLight->Color = D3DXCOLOR(
				1.f, 0.f, 0.f, 1.f);
			SpPtLight->PointRadius = PtLightRadius;
			SpPtLight->lightFlux = 0.0f;
		}
	}


	CurParticleTime = ParticleCycle;
	_RenderProperty.bRender = true;
	T = 0.0f;
};



void Change::PlayEnd()
{
	_RenderProperty.bRender = false;
	T = 0.0f;

	if (auto _PtLight = PtLight.lock();
		_PtLight)
	{
		_PtLight->bEnable = false;
	}
};

void Change::UpdateParticle(const float DeltaTime)
{
	CurParticleDelta -= DeltaTime;
	if (CurParticleDelta < 0.0f)
	{
		CurParticleDelta += ParticleTime;
		PlayParticle();
	}
}
void Change::UpdateStoneParticle(const float DeltaTime)
{
	CurStoneParticleDelta -= DeltaTime;
	if (CurStoneParticleDelta < 0.0f)
	{
		CurStoneParticleDelta += StoneParticleTime;
		if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
			SpTransform)
		{
			auto StoneWorld = FMath::Scale(EndScale)
				* FMath::Identity()
				* FMath::Translation(StoneLocation);

			if (auto _Particle =
				ParticleSystem::GetInstance()->PlayParticle(
					"ChangeStone", 11ul, true);
				_Particle.empty() == false)
			{
				for (int32 i = 0; i < _Particle.size(); ++i)
				{
					auto& _PlayInstance = _Particle[i];
					_PlayInstance->PlayDescBind(StoneWorld);
				}
			}
		};
	}
};

void Change::PlayParticle()
{
	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		if (auto _Particle =
			ParticleSystem::GetInstance()->PlayParticle(
				"ChangeParticle", 555ul, true);
			_Particle.empty() == false)
		{
			for (int32 i = 0; i < _Particle.size(); ++i)
			{
				auto& _PlayInstance = _Particle[i];
				_PlayInstance->PlayDescBind(_RenderUpdateInfo.World);
			}
		}
	};
};

void Change::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("matWorld", &_RenderUpdateInfo.World);

	_Info.Fx->SetFloat("ColorIntencity", ColorIntencity);

	_Info.Fx->SetVector("CurColor", &_Color);

	_Info.Fx->SetTexture("AlbmMap", _AlbmMap->GetTexture());
	_Info.Fx->SetTexture("AlpgMap", _AlpgMap->GetTexture());
	_Info.Fx->SetTexture("MskMap", _MskMap->GetTexture());

	const uint32 Numsubset = _Mesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = _Mesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
};

void Change::RenderDebug(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("World", &_RenderUpdateInfo.World);

	const uint32 Numsubset = _Mesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = _Mesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
};


HRESULT Change::Ready()
{
	auto InitTransform = GetComponent<ENGINE::Transform>();
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	return S_OK;
};

HRESULT Change::Awake()
{
	GameObject::Awake();
	m_pTransform.lock()->SetPosition(Vector3{ 0.f,0.5f,0.f });
	m_pTransform.lock()->SetScale(Vector3{ 0.005f,0.005f,0.005f });
	m_pTransform.lock()->SetRotation(Vector3{ 90.f,0.f,0.f });

	return S_OK;
}

HRESULT Change::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT Change::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);

	if (_RenderProperty.bRender == false) return 0;

	T += _fDeltaTime;

	if (T > PlayTime)
	{
		PlayEnd();
		return 0;
	}

	UpdateParticle(_fDeltaTime);
	UpdateStoneParticle(_fDeltaTime);

	if (auto SpTransform = GetComponent<Transform>().lock();
		SpTransform)
	{
		Vector3 Location = SpTransform->GetPosition();
		Location.y += VelocityY;
		SpTransform->SetPosition(Location);

		const float Lerp = std::sinf(T / PlayTime * FMath::PI);
		const float LerpScale = FMath::Lerp(0.f, EndScale, Lerp);

		SpTransform->SetScale(Vector3{ LerpScale ,LerpScale ,LerpScale });

		if (PtLight.expired() == false)
		{
			auto SpPtLight = PtLight.lock();
			SpPtLight->SetPosition
			(FMath::ConvertVector4(
				SpTransform->GetPosition(),
				1.f
			));
			SpPtLight->Color = D3DXCOLOR(
				1.f, 0.f, 0.f, 1.f);
			SpPtLight->PointRadius = PtLightRadius;
			SpPtLight->lightFlux =
				(std::sinf((T / PlayTime) * FMath::PI)) * PtLightFlux;
		}
	}

	return 0;
}

UINT Change::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
};

void Change::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		const std::string ChildName = GetName() + "_Play";
		ImGui::BeginChild(ChildName.c_str());
		{
			if (ImGui::SmallButton("Play"))
			{
				Vector3 Point{ 0.f,0.2f ,0.f };
				/*if (auto SpTransform = m_pTransform.lock();
					SpTransform)
				{
					Point = SpTransform->GetPosition();
				};*/

				PlayStart(Point);
			}
			if (ImGui::SmallButton("PlayEnd"))
			{
				PlayEnd();
			}

			ImGui::ColorEdit4("Color", _Color);

			ImGui::SliderFloat("StoneParticleTime", &StoneParticleTime, 0.0f, 0.5f);
			ImGui::SliderFloat("ParticleTime", &ParticleTime, 0.0f, 0.5f);

			ImGui::SliderFloat("EndScale", &EndScale, 0.0f, 0.1f);
			ImGui::SliderFloat("PtLightFlux", &PtLightFlux, 0.0f, 100.f);

			ImGui::SliderFloat("EditPlayTime", &PlayTime, FLT_MIN, 10.f, "%9.6f");
			ImGui::InputFloat("In EditPlayTime", &PlayTime, 0.f, 0.f, "%9.6f");

			ImGui::SliderFloat("ColorIntencity", &ColorIntencity, FLT_MIN, 1.f, "%9.6f");
			ImGui::InputFloat("In ColorIntencity", &ColorIntencity, 0.f, 0.f, "%9.6f");

			ImGui::SliderFloat("VelocityY", &VelocityY, FLT_MIN, 1.f, "%9.6f");
			ImGui::InputFloat("In VelocityY", &VelocityY, 0.f, 0.f, "%9.6f");
		}
		ImGui::EndChild();
	}
};

void Change::OnEnable()
{
	GameObject::OnEnable();
};

void Change::OnDisable()
{
	GameObject::OnDisable();
};



