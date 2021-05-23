#include "stdafx.h"
#include "..\Header\Reverberation.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "GraphicSystem.h"
#include "RedQueen.h"
#include "ParticleSystem.h"
#include "ParticleInstanceDesc.hpp"

Reverberation::Reverberation()
{

};

void Reverberation::Free()
{
	GameObject::Free();
};

std::string Reverberation::GetName()
{
	return "Reverberation";
};

Reverberation* Reverberation::Create()
{
	return new Reverberation{};
};

void Reverberation::RenderReady()
{
	// �̹� ���ؽ� ��ü�� ���� ��ġ�� . 
	_RenderUpdateInfo.World;

	if (auto _SpTransform = GetComponent<ENGINE::Transform>().lock();
		_SpTransform)
	{
		_RenderUpdateInfo.World =
			FMath::Scale(_SpTransform->GetScale()) *
			DirRotMatrix *
			FMath::Translation(_SpTransform->GetPosition());
	}
};

void Reverberation::RenderInit()
{
	m_nTag = Eff_ArteMisMissile;
	// ������ �����ؾ��ϴ� ������Ʈ��� (�������� ��� ����) �˸�.
	// ���� �������̽� ��ӹ��� �ʾҴٸ� Ű��������.
	SetRenderEnable(true);

	// ���� ���� �ʱ�ȭ ...
	ENGINE::RenderProperty _InitRenderProp;
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
	// ���� �Ӽ� ��ü �ʱ�ȭ 
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
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
			"Reverberation",[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		}

	};

	RenderInterface::Initialize(_InitRenderProp);

	// �޽�
	{
		Mesh::InitializeInfo _Info{};
		_Info.bLocalVertexLocationsStorage = true;
		_Mesh = Resources::Load<StaticMesh>(
			"..\\..\\Resource\\Mesh\\Static\\Primitive\\pipe01.fbx", _Info);
	}

	// �ؽ��� 
	GradMap = Resources::Load<Texture>(
		"..\\..\\Resource\\Texture\\Effect\\lightning_alb.png");

	PushEditEntity(_Mesh.get());
	PushEditEntity(GradMap.get());
};

void Reverberation::PlayStart(
	const Vector3& Location,
	const Vector3& Direction,
	const float StartScale,
	const float EndScale ,
	const bool bEditPlay )
{
	_RenderProperty.bRender = true;
	T = 0.0f;

	this->StartScale = StartScale;
	this->EndScale = EndScale;
	const Vector3 DirectionNorm = FMath::Normalize(Direction);
	if (auto SpTransform = GetComponent<Transform>().lock();
		SpTransform)
	{
		SpTransform->SetPosition(Location);

		const Vector3 Look = DirectionNorm.z > 0.f ? Vector3{ 0.f,0.f,-1.f } : Vector3{ 0.f,0.f,1.f};
		const Vector3 Axis = FMath::Cross(DirectionNorm, Look);
		const Vector3 NormalAxis = FMath::Normalize(Axis);
		const float Rad = std::asinf(FMath::Length(Axis));

		if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
			SpTransform)
		{
			DirRotMatrix = FMath::RotationAxisMatrix(NormalAxis, Rad);
		};

		this->Direction =  FMath::Normalize(DirectionNorm);
	}

	this->bEditPlay = bEditPlay; 
};

void Reverberation::PlayEnd()
{
	_RenderProperty.bRender = false;
	T = 0.0f;

	if (bEditPlay)
	{
		GetComponent<Transform>().lock()->SetPosition(Vector3{ 0.f,0.12f,0.f });
	}
};

void Reverberation::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	_Info.Fx->SetMatrix("matWorld", &_RenderUpdateInfo.World);
	_Info.Fx->SetVector("_Color", &Color);
	_Info.Fx->SetTexture("GradMap", GradMap->GetTexture());
	_Info.Fx->SetFloat("GradSampleU", T * TimeAcc);
	const float SinFactor = std::sinf(T * TimeAcc / PlayTime * FMath::PI);
	const float CurColorIntencity = FMath::Lerp(ColorIntencityHigh,ColorIntencityLow, SinFactor);

	_Info.Fx->SetFloat("ColorIntencity", CurColorIntencity);
	_Info.Fx->SetFloat("AlphaFactor", std::sinf(T / PlayTime * FMath::PI));

	{
		const uint32 Numsubset = _Mesh->GetNumSubset();
		for (uint32 i = 0; i < Numsubset; ++i)
		{
			if (auto SpSubset = _Mesh->GetSubset(i).lock();
				SpSubset)
			{
				SpSubset->Render(_Info.Fx);
			};
		};
	}
};

void Reverberation::ParticleUpdate(const float DeltaTime)
{
	CurParticleDelta -= DeltaTime;

	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		if (CurParticleDelta < 0.0f)
		{
			CurParticleDelta += ParticleDelta;

			{
				const uint32 ParticleCnt = 66u;

				auto _PlayableParticle = ParticleSystem::GetInstance()->
					PlayParticle("ReverberationParticle", ParticleCnt, true);

				for (int32 i = 0; i < _PlayableParticle.size();
					++i)
				{
					auto& _PlayInstance = _PlayableParticle[i];
					_PlayInstance->PlayDescBind(_RenderUpdateInfo.World);
				}
			}

		}
	}
};

void Reverberation::RenderDebug(const DrawInfo& _Info)
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


HRESULT Reverberation::Ready()
{
	auto InitTransform = GetComponent<ENGINE::Transform>();
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	return S_OK;
};

HRESULT Reverberation::Awake()
{
	GameObject::Awake();
	m_pTransform.lock()->SetPosition(Vector3{ 0.f,0.12f,0.f  });
	m_pTransform.lock()->SetScale(Vector3{ 0.001f,0.001f,0.001f });
	m_pTransform.lock()->SetRotation(Vector3{ 0.f,0.f,0.f });

	return S_OK;
}

HRESULT Reverberation::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT Reverberation::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);

	if (_RenderProperty.bRender == false) return 0;

	T += _fDeltaTime;

	if (T > PlayTime)
	{
		PlayEnd();
		return 0;
	}
	const float CurScale = FMath::Lerp(StartScale, EndScale, T / PlayTime);
	auto SpTransform = GetComponent<Transform>().lock();
	SpTransform->SetScale(Vector3{ CurScale ,CurScale ,CurScale });
	SpTransform->SetPosition(SpTransform->GetPosition() + Direction  * Velocity* _fDeltaTime);
	ParticleUpdate(_fDeltaTime);
	
	return 0;
}

UINT Reverberation::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
};

void Reverberation::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		const std::string ChildName = GetName() + "_Play";
		ImGui::BeginChild(ChildName.c_str());
		{
			static Vector3 EditPlayDirection{ 1.f,0.f,0.f };
			ImGui::SliderFloat3("EditPlayDirection", EditPlayDirection, -1.f, 1.f);

			if (ImGui::SmallButton("Play"))
			{
				PlayStart(m_pTransform.lock()->GetPosition(), EditPlayDirection,StartScale,EndScale ,true);
			}
			if (ImGui::SmallButton("PlayEnd"))
			{
				PlayEnd();
			}
			ImGui::ColorEdit4("Color", Color);
			ImGui::SliderFloat("ColorIntencityHigh", &ColorIntencityHigh, 0.f, 1.f);
			ImGui::SliderFloat("ColorIntencityLow", &ColorIntencityLow, 0.f, 1.f);
			ImGui::SliderFloat("TimeAcc", &TimeAcc, 0.f, 10.f);
			ImGui::SliderFloat("StartScale", &StartScale, 0.f, 0.1f);
			ImGui::SliderFloat("EndScale", &EndScale, 0.f, 0.1f);
			ImGui::SliderFloat("Velocity", &Velocity, 0.f, 10.f);
		}
		ImGui::EndChild();
	}
};

void Reverberation::OnEnable()
{
	GameObject::OnEnable();
};

void Reverberation::OnDisable()
{
	GameObject::OnDisable();
};



