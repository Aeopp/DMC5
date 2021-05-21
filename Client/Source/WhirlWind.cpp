#include "stdafx.h"
#include "..\Header\WhirlWind.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "GraphicSystem.h"
#include "RedQueen.h"
#include "ParticleSystem.h"
#include "Renderer.h"

WhirlWind::WhirlWind()
{

};

void WhirlWind::Free()
{
	GameObject::Free();
};

std::string WhirlWind::GetName()
{
	return "WhirlWind";
};

WhirlWind* WhirlWind::Create()
{
	return new WhirlWind{};
};

void WhirlWind::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const Vector3 Scale = _SpTransform->GetScale();
		_RenderProperty.bRender = true;
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

void WhirlWind::RenderInit()
{
	m_nTag = Eff_FireCircle;
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
		{"WhirlWind",[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		}
	};

	RenderInterface::Initialize(_InitRenderProp);

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = true;

	_Mesh = Resources::Load<StaticMesh>("..\\..\\Resource\\Mesh\\Static\\Effect\\vortex.fbx", _Info);

	_BlurMap = Resources::Load<Texture>(
		"..\\..\\Resource\\Texture\\Effect\\ex_capcom_smoke_00_0037_alpg.tga");
};

void WhirlWind::PlayStart(
	const Vector3& Location,
	const Vector3& Direction ,
	const bool bEditPlay)
{
	auto SpTransform = GetComponent<Transform>().lock();
	CurrentDirection = FMath::Normalize(Direction);
	SpTransform->SetPosition(Location + CurrentDirection*ForwardOffset);
	SpTransform->SetRotation(Vector3{0.f,
		FMath::ToDegree( std::atan2f(-CurrentDirection.x,-CurrentDirection.z) ) ,90.f});
	_RenderProperty.bRender = true;
	T = 0.0f;

	this->bEditPlay = bEditPlay;
	if (auto RemainLight = PtLight.lock();
		RemainLight)
	{
		RemainLight->bEnable = false;
	}

	if (PtLight = Renderer::GetInstance()->RefRemainingDynamicLight();
		PtLight.expired()==false)
	{
		auto SpPtLight = PtLight.lock();
		SpPtLight->bEnable = true;

		SpPtLight->Color = PtLightColor;
		SpPtLight->Position = FMath::ConvertVector4(SpTransform->GetPosition(), 1.f);
		SpPtLight->lightFlux = LightFluxLerp.first;
		SpPtLight->PointRadius = PointLightRadius;
	}
};

void WhirlWind::PlayEnd()
{
	_RenderProperty.bRender = false;

	if (bEditPlay)
	{
		GetComponent<Transform>().lock()->SetPosition(Vector3{0.f,0.2f ,0.f});

	}

	if ( PtLight.expired() == false)
	{
		auto SpPtLight = PtLight.lock();
		SpPtLight->bEnable = false;
	}
};

void WhirlWind::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("matWorld", &_RenderUpdateInfo.World);
	_Info.Fx->SetTexture("BlurMap", _BlurMap->GetTexture());
	_Info.Fx->SetFloat("BlurIntencity", BlurIntencity);

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

void WhirlWind::ParticleUpdate(const float DeltaTime)
{
	CurParticleDelta -= DeltaTime;

	if (CurParticleDelta < 0.0f)
	{
		CurParticleDelta += ParticleDelta;

		if (auto SpTransform = m_pTransform.lock();
			SpTransform)
		{
			const std::string CurrentSpriteIdx = std::to_string(FMath::Random(1u, 15u));
			auto _PlayableParticle =
				ParticleSystem::GetInstance()->
				PlayParticle("FireVortexSprite" + 
					CurrentSpriteIdx, 300ul, true);
			// 파이어 파티클 뿌리기 ....

			for (int32 i = 0; i < _PlayableParticle.size(); ++i)
			{
				auto& _PlayInstance = _PlayableParticle[i];

				_PlayInstance->PlayDescBind(_RenderUpdateInfo.World	);
			}
		}
	}
};

void WhirlWind::RenderDebug(const DrawInfo& _Info)
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

HRESULT WhirlWind::Ready()
{
	auto InitTransform = GetComponent<ENGINE::Transform>();
	m_nTag = Eff_WhirlWind;
	PushEditEntity(InitTransform.lock().get());
	RenderInit();

	return S_OK;
};

HRESULT WhirlWind::Awake()
{
	GameObject::Awake();

	m_pTransform.lock()->SetPosition(Vector3{ 0.f,0.2f,0.f });
	m_pTransform.lock()->SetScale(Vector3{ 0.0035f ,0.0035f ,0.0035f });
	m_pTransform.lock()->SetRotation(Vector3{ 90.f,0.f,0.f });

	return S_OK;
}

HRESULT WhirlWind::Start()
{
	GameObject::Start();

	return S_OK;
};

UINT WhirlWind::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	if (_RenderProperty.bRender == false) return 0;
	T += _fDeltaTime;

	if (T > PlayTime)
	{
		PlayEnd();
		return 0;
	};

	auto SpTransform = GetComponent<Transform>().lock();
	SpTransform->SetPosition(SpTransform->GetPosition()  + CurrentDirection * _fDeltaTime * Velocity);

	ParticleUpdate(_fDeltaTime);

	if (PtLight.expired() == false)
	{
		auto SpPtLight = PtLight.lock();

		SpPtLight->Color = PtLightColor;
		SpPtLight->Position = FMath::ConvertVector4(SpTransform->GetPosition(), 1.f);

		SpPtLight->lightFlux = FMath::Lerp(LightFluxLerp.first, LightFluxLerp.second, T / PlayTime);
		SpPtLight->PointRadius = PointLightRadius;
	}

	return 0;
}

UINT WhirlWind::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
};

void WhirlWind::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		const std::string ChildName = GetName() + "_Play";
		ImGui::BeginChild(ChildName.c_str());
		{
			if (ImGui::SmallButton("Play"))
			{
				PlayStart( GetComponent<Transform>().lock()->GetPosition(), EditPlayDirection,true);
			}

			if (ImGui::SmallButton("PlayEnd"))
			{
				PlayEnd();
			}

			ImGui::SliderFloat3("EditPlayDirection", EditPlayDirection, -1.f, 1.f);
			ImGui::SliderFloat("Velocity", &Velocity, 0.0f, 10.f);
			ImGui::SliderFloat("BlurIntencity", &BlurIntencity, 0.0f, 1000.f);
			ImGui::SliderFloat("LightFluxBegin", &LightFluxLerp.first, 0.0f, 1000.f);
			ImGui::SliderFloat("LightFluxEnd", &LightFluxLerp.second, 0.0f, 1000.f);

			ImGui::SliderFloat("PointLightRadius ", &PointLightRadius,0.0f,100.f);
		}
		ImGui::EndChild();
	}
};

void WhirlWind::OnEnable()
{
	GameObject::OnEnable();
};

void WhirlWind::OnDisable()
{
	GameObject::OnDisable();
};


