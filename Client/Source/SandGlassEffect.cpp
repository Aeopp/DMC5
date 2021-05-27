#include "stdafx.h"
#include "..\Header\SandGlassEffect.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "ParticleSystem.h"

void SandGlassEffect::Free()
{
	GameObject::Free();
};

std::string SandGlassEffect::GetName()
{
	return "SandGlassEffect";
};

SandGlassEffect* SandGlassEffect::Create()
{
	return new SandGlassEffect{};
};

void SandGlassEffect::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();

	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const float CurScale = _SpTransform->GetScale().x;
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
		*reinterpret_cast<Vector3*>(&_RenderUpdateInfo.World.m[3][0]) = PlayPos;	// �Ф�

		if (_Mesh)
		{
			const uint32  Numsubset = _Mesh->GetNumSubset();
			_RenderUpdateInfo.SubsetCullingSphere.resize(Numsubset);

			for (uint32 i = 0; i < Numsubset; ++i)
			{
				const auto& _Subset = _Mesh->GetSubset(i);
				const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

				_RenderUpdateInfo.SubsetCullingSphere[i] = _CurBS.Transform(_RenderUpdateInfo.World, CurScale);
			}
		}

	}
};

void SandGlassEffect::RenderInit()
{
	m_nTag = Eff_SandGlassEffect;
	// ������ �����ؾ��ϴ� ������Ʈ��� (�������� ��� ���� ) �˸�.
	// ���� �������̽� ��ӹ��� �ʾҴٸ� Ű��������.
	SetRenderEnable(true);

	// ���� ���� �ʱ�ȭ ...
	ENGINE::RenderProperty _InitRenderProp;
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
	// ���� �Ӽ� ��ü �ʱ�ȭ 
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
	_InitRenderProp.bRender = false;

	_InitRenderProp.RenderOrders[RenderProperty::Order::Debug]
		=
	{
		{"Debug" ,
		[this](const DrawInfo& _Info)
		{
			RenderDebug(_Info);
		}
	} };

	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{

		{"SandGlassEffect",
		[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		}
	};

	RenderInterface::Initialize(_InitRenderProp);

	// 
	// ����ƽ �޽� �ε�
	Mesh::InitializeInfo _InitInfo{};
	_InitInfo.bLocalVertexLocationsStorage = false;

	_Mesh = Resources::Load<ENGINE::StaticMesh>
		(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _InitInfo);

	PushEditEntity(_Mesh.get());

	//_DynamicLight.Color =
	//{
	//	ColorLow,ColorHigh
	//};

	//_DynamicLight.Flux =
	//{
	//	ExplosionReadyFluxLow,ExplosionReadyFluxHigh
	//};

	//_DynamicLight.PointRadius =
	//{
	//	ExplosionReadyRadiusLow,ExplosionReadyRadiusHigh
	//};
};

void SandGlassEffect::PlayStart(const Vector3& Location)
{
	if (auto SpTransform = GetComponent<Transform>().lock();
		SpTransform)
	{
		SpTransform->SetPosition(Location);
		PlayPos = Location;
		//_DynamicLight.PlayStart(Location, (std::numeric_limits<float>::max)());
	}

	T = 0.0f;
	_RenderProperty.bRender = true;
};

void SandGlassEffect::UpdateParticle(const float DeltaTime)
{
	CurParticleTime -= DeltaTime;
	if (CurParticleTime < 0.0f)
	{
		CurParticleTime += ParticleTime;

		if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
			SpTransform)
		{
			if (auto _Particle =
				ParticleSystem::GetInstance()->PlayParticle(
					"SandGlassEffectParticle", 44ul, true);

				_Particle.empty() == false)
			{

				for (int32 i = 0; i < _Particle.size(); ++i)
				{
					auto& _PlayInstance = _Particle[i];
					_PlayInstance->PlayDescBind(_RenderUpdateInfo.World);
				}
			}
		};
	}
};



void SandGlassEffect::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	const uint32 Numsubset = _Mesh->GetNumSubset();

	if (Numsubset > 0)
	{
		_Info.Fx->SetMatrix("matWorld", &World);
	}

	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = _Mesh->GetSubset(i).lock();
			SpSubset)
		{
			bInnerFrustum = _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]);
			if (false == bInnerFrustum)
			{
				continue;
			}

			SpSubset->Render(_Info.Fx);
		};
	};
}


void SandGlassEffect::RenderDebug(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);

	const uint32 Numsubset = _Mesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = _Mesh->GetSubset(i).lock();
			SpSubset)
		{
			bInnerFrustum = _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]);

			if (false == bInnerFrustum)
			{
				continue;
			}

			SpSubset->Render(_Info.Fx);
		};
	};

};


HRESULT SandGlassEffect::Ready()
{
	// Ʈ������ �ʱ�ȭ .. 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	// �������� ������ �ް���� ������Ʈ�� Raw �����ͷ� Ǫ��.
	return S_OK;
};

HRESULT SandGlassEffect::Awake()
{
	GameObject::Awake();

	m_pTransform.lock()->SetScale({ 0.00007f,0.00007f,0.00007f });
	m_pTransform.lock()->SetPosition(Vector3{ 0.f, 0.f, 0.f });
	m_pTransform.lock()->SetRotation(Vector3{ 0.f, 0.f, 0.f });

	return S_OK;
}

HRESULT SandGlassEffect::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT SandGlassEffect::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	if (_RenderProperty.bRender == false) return 0;
	if (bInnerFrustum == false)return 0;

	T += _fDeltaTime;
	if (T > 13.f)
		T -= 13.f;

	UpdateParticle(_fDeltaTime);

	// ����~
	//const float LerpT = T / 13.f;
	//const float Radius = FMath::Lerp(ExplosionReadyRadiusLow, ExplosionReadyRadiusHigh, LerpT);
	//const float Flux = FMath::Lerp(ExplosionReadyFluxLow, ExplosionReadyFluxHigh, LerpT);
	//const D3DXCOLOR _Color = FMath::ToColor(FMath::Lerp(ColorLow, ColorHigh, LerpT));
	//_DynamicLight.Update(_Color, Radius, Flux, PlayPos);

	return 0;
}

UINT SandGlassEffect::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
}

void SandGlassEffect::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		const std::string ChildName = GetName() + "_Play";
		ImGui::BeginChild(ChildName.c_str());
		if (ImGui::SmallButton("Play"))
		{
			PlayStart(GetComponent<Transform>().lock()->GetPosition());
		}
		
		//_DynamicLight.Editor();

		ImGui::Text("T : %2.6f", T);

		ImGui::SliderFloat("ParticleUdateTime", &ParticleTime, 0.0f, 1.f);

		ImGui::EndChild();
	}
}


void SandGlassEffect::OnEnable()
{
	GameObject::OnEnable();
}

void SandGlassEffect::OnDisable()
{
	GameObject::OnDisable();
}



