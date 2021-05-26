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
	// 렌더를 수행해야하는 오브젝트라고 (렌더러에 등록 가능 ) 알림.
	// 렌더 인터페이스 상속받지 않았다면 키지마세요.
	SetRenderEnable(true);

	// 렌더 정보 초기화 ...
	ENGINE::RenderProperty _InitRenderProp;
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	// 렌더 속성 전체 초기화 
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
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
	// 스태틱 메쉬 로딩
	Mesh::InitializeInfo _InitInfo{};
	_InitInfo.bLocalVertexLocationsStorage = false;

	_Mesh = Resources::Load<ENGINE::StaticMesh>
		(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _InitInfo);

	PushEditEntity(_Mesh.get());
};

void SandGlassEffect::PlayStart(const Vector3& Location)
{
	if (auto SpTransform = GetComponent<Transform>().lock();
		SpTransform)
	{
		if (Location)
		{
			SpTransform->SetPosition(Location);
		}
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
					"SandGlassEffect", 16ul, true);

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
	// 트랜스폼 초기화 .. 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.
	return S_OK;
};

HRESULT SandGlassEffect::Awake()
{
	GameObject::Awake();

	m_pTransform.lock()->SetScale({ 0.0010f,0.0010f,0.0010f });
	m_pTransform.lock()->SetPosition(Vector3{ 0.f,0.12f,0.f });
	m_pTransform.lock()->SetRotation(Vector3{ 0.0f ,0.f ,0.0f });

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
	UpdateParticle(_fDeltaTime);

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

		ImGui::Text("T : %2.6f", T);

		ImGui::SliderFloat("ParticleUdateTime", &ParticleTime,0.0f,1.f);

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



