#include "stdafx.h"
#include "..\Header\ArtemisMissile.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "GraphicSystem.h"
#include "RedQueen.h"
#include "ParticleSystem.h"
#include "ParticleInstanceDesc.hpp"

ArtemisMissile::ArtemisMissile()
{

};

void ArtemisMissile::Free()
{
	GameObject::Free();
};

std::string ArtemisMissile::GetName()
{
	return "ArtemisMissile";
};

ArtemisMissile* ArtemisMissile::Create()
{
	return new ArtemisMissile{};
};

void ArtemisMissile::RenderReady()
{
	// 이미 버텍스 자체가 월드 위치임 . 
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const Vector3 Scale = _SpTransform->GetScale();
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
		if (_Mesh)
		{
			const uint32  Numsubset = _Mesh->GetNumSubset();
			_RenderUpdateInfo.SubsetCullingSphere.resize(Numsubset);

			for (uint32 i = 0; i < Numsubset; ++i)
			{
				const auto& _Subset = _Mesh->GetSubset(i);
				const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

				_RenderUpdateInfo.SubsetCullingSphere[i] = _CurBS.Transform(_RenderUpdateInfo.World, Scale.x);
			}
		}
	}
};

void ArtemisMissile::RenderInit()
{
	m_nTag = Eff_ArteMisMissile;
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
			"ArtemisMissile",[this](const DrawInfo& _Info)
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
			"..\\..\\Resource\\Mesh\\Static\\Primitive\\sphere00.fbx", _Info);
	}

	// 텍스쳐 
	GradMap = Resources::Load<Texture>(
		"..\\..\\Resource\\Texture\\Effect\\lightning_alb.png");

	PushEditEntity(_Mesh.get());
	PushEditEntity(GradMap.get());
};

void ArtemisMissile::PlayStart()
{
	_RenderProperty.bRender = true;
	T = 0.0f;
};

void ArtemisMissile::PlayEnd()
{
	_RenderProperty.bRender = false;
	T = 0.0f;
};


void ArtemisMissile::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("matWorld", &_RenderUpdateInfo.World);
	_Info.Fx->SetVector("_Color", &Color);
	_Info.Fx->SetTexture("GradMap", GradMap->GetTexture());
	_Info.Fx->SetFloat("GradSampleU", T);

	const float LuminT = FMath::Clamp(T / LuminDecayTime, 0.f, 1.f);
	const float CurColorIntencity = FMath::Lerp(StartColorIntencity,ColorIntencity, LuminT);
	_Info.Fx->SetFloat("ColorIntencity", CurColorIntencity);

	{
		const uint32 Numsubset = _Mesh->GetNumSubset();
		for (uint32 i = 0; i < Numsubset; ++i)
		{
			if (auto SpSubset = _Mesh->GetSubset(i).lock();
				SpSubset)
			{
				if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
				{
					continue;
				}

				SpSubset->Render(_Info.Fx);
			};
		};
	}
};

void ArtemisMissile::ParticleUpdate(const float DeltaTime)
{
	CurParticleDelta -= DeltaTime;

	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		if (CurParticleDelta < 0.0f)
		{
			CurParticleDelta += ParticleDelta;

			const Matrix Mat = SpTransform->GetRenderMatrix();
			{
				const uint32 ParticleCnt = 22u;

				auto _PlayableParticle = ParticleSystem::GetInstance()->
					PlayParticle("ArtemisMissileParticle", 22u, true);

				
				for (int32 i = 0; i < _PlayableParticle.size();
					++i)
				{
					auto& _PlayInstance = _PlayableParticle[i];
					_PlayInstance->PlayDescBind(Mat);
				}
			}

		}
	}
}
void ArtemisMissile::RushParticle()
{
	const uint32 ParticleCnt = 1000u;
	auto _PlayableParticle = ParticleSystem::GetInstance()->
		PlayParticle("ArtemisRushParticle", ParticleCnt, true);

	for (int32 i = 0; i < _PlayableParticle.size();
		++i)
	{
		auto& _PlayInstance = _PlayableParticle[i];
		_PlayInstance->PlayDescBind(_RenderUpdateInfo.World);
	};
};

void ArtemisMissile::RenderDebug(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("World", &_RenderUpdateInfo.World);


	const uint32 Numsubset = _Mesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = _Mesh->GetSubset(i).lock();
			SpSubset)
		{
			if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
			{
				continue;
			}

			SpSubset->Render(_Info.Fx);
		};
	};

};


HRESULT ArtemisMissile::Ready()
{
	auto InitTransform = GetComponent<ENGINE::Transform>();
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	return S_OK;
};

HRESULT ArtemisMissile::Awake()
{
	GameObject::Awake();
	m_pTransform.lock()->SetPosition(Vector3{ 0.f,0.12f,0.f });
	m_pTransform.lock()->SetScale(Vector3{ 0.0001f,0.0001f,0.0001f });
	m_pTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });
	return S_OK;
}

HRESULT ArtemisMissile::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT ArtemisMissile::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);

	if (_RenderProperty.bRender == false) return 0;

	T += _fDeltaTime;

	ParticleUpdate(_fDeltaTime);

	return 0;
}

UINT ArtemisMissile::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
};

void ArtemisMissile::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		const std::string ChildName = GetName() + "_Play";
		ImGui::BeginChild(ChildName.c_str());
		{
			if (ImGui::SmallButton("Play"))
			{
				PlayStart();
			}
			if (ImGui::SmallButton("PlayEnd"))
			{
				PlayEnd();
			}

			if (ImGui::SmallButton("RushParticle"))
			{
				RushParticle();
			}

			ImGui::ColorEdit4("Color", Color);
			ImGui::SliderFloat("ColorIntencity", &ColorIntencity, 0.f, 10.f);
			ImGui::SliderFloat("StartColorIntencity", &StartColorIntencity, 0.f, 10.f);
		}
		ImGui::EndChild();
	}
};

void ArtemisMissile::OnEnable()
{
	GameObject::OnEnable();
};

void ArtemisMissile::OnDisable()
{
	GameObject::OnDisable();
};



