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
	// �̹� ���ؽ� ��ü�� ���� ��ġ�� . 
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
			"ArtemisMissile",[this](const DrawInfo& _Info)
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
			"..\\..\\Resource\\Mesh\\Static\\Primitive\\sphere00.fbx", _Info);
	}

	// �ؽ��� 
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



