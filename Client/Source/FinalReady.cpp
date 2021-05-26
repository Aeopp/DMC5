#include "stdafx.h"
#include "..\Header\FinalReady.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "ParticleSystem.h"

void FinalReady::Free()
{
	GameObject::Free();
};

std::string FinalReady::GetName()
{
	return "FinalReady";
};

FinalReady* FinalReady::Create()
{
	return new FinalReady{};
};

void FinalReady::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();

	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderUpdateInfo.World =
			FMath::Scale(_SpTransform->GetScale()) *
			Renderer::GetInstance()->_RenderInfo.Billboard *
			FMath::Translation(_SpTransform->GetPosition());
	};
};

void FinalReady::PlayParticle()
{
	if (auto SpTransform = GetComponent<ENGINE::Transform>().lock();
		SpTransform)
	{
		const uint32 ParticleCnt = 5555u;
		const Matrix Mat = FMath::WorldMatrix
		(SpTransform->GetScale(),
			Vector3{ 0.f,0.f,0.f },
			SpTransform->GetPosition());

		auto _PlayableParticle = ParticleSystem::GetInstance()->
			PlayParticle("ArtemisCylinderParticle", ParticleCnt, true);

		for (int32 i = 0; i < _PlayableParticle.size();
			++i)
		{
			auto& _PlayInstance = _PlayableParticle[i];
			_PlayInstance->PlayDescBind(Mat);
		}

	}
}

void FinalReady::RenderInit()
{
	m_nTag = Eff_FinalReady;
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
		{"FinalReady",
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

	_StaticMesh = Resources::Load<ENGINE::StaticMesh>
		(L"..\\..\\Usable\\LocationMesh\\Cylinder.fbx", 
			_InitInfo);

	_Alpg = Resources::Load<ENGINE::Texture>(
		L"..\\..\\Usable\\LightShaft\\1.tga");

	PushEditEntity(_StaticMesh.get());
	PushEditEntity(_Alpg.get());
};

void FinalReady::PlayStart(const Vector3& Location)
{
	if (auto SpTransform = GetComponent<Transform>().lock();
		SpTransform)
	{
		SpTransform->SetPosition(Location);
	}

	this->PlayTime = PlayTime;
	T = 0.0f;
	_RenderProperty.bRender = true;

	PlayParticle();
}; 

void FinalReady::PlayEnd()
{
	_RenderProperty.bRender = false;
	T = 0.0f;
};

void FinalReady::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	const uint32 Numsubset = _StaticMesh->GetNumSubset();

	if (Numsubset > 0)
	{
		_Info.Fx->SetMatrix("matWorld", &World);
		_Info.Fx->SetTexture("AlpgMap", 
						_Alpg->GetTexture());

		const float LerpT = 
			FMath::Clamp ( T / LerpEndT ,0.f,1.f );

		const float CurColorIntencity = 
			FMath::Lerp(0.f, ColorIntencity, LerpT);

		const float AlphaFactor  = 
			FMath::Lerp(0.f, AlphaIntencity,LerpT);

		_Info.Fx->SetFloat("ColorIntencity", CurColorIntencity);
		_Info.Fx->SetFloat("AlphaFactor", AlphaFactor);
	};

	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = _StaticMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
}


void FinalReady::RenderDebug(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);

	const uint32 Numsubset = _StaticMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = _StaticMesh->GetSubset(i).lock();
			SpSubset)
		{

			SpSubset->Render(_Info.Fx);
		};
	};

};


HRESULT FinalReady::Ready()
{
	// 트랜스폼 초기화 .. 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.
	return S_OK;
};

HRESULT FinalReady::Awake()
{
	GameObject::Awake();

	m_pTransform.lock()->SetPosition(Vector3{ -37.411f,0.821f,30.663f  });
	m_pTransform.lock()->SetScale({ 0.001f ,0.001f ,0.001f });
	m_pTransform.lock()->SetRotation(
		{ 0.f ,0.f ,0.f});

	return S_OK;
};


HRESULT FinalReady::Start()
{
	GameObject::Start();

	return S_OK;
};


UINT FinalReady::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);

	if (_RenderProperty.bRender == false) 
		return 0;

	T += _fDeltaTime;

	if (T > PlayTime)
	{
		PlayEnd();
	};

	return 0;
}


UINT FinalReady::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
}

void FinalReady::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		const std::string ChildName = GetName() + "_Play";
		ImGui::BeginChild(ChildName.c_str());
		if (ImGui::SmallButton("PlayStart"))
		{
			PlayStart(GetComponent<Transform>().lock()->GetPosition());
		}
		ImGui::SliderFloat("AlphaIntencity", &AlphaIntencity, 0.0f, 1.f);
		ImGui::SliderFloat("ColorIntencity", &ColorIntencity, 0.0f, 1.f);

		float LerpEndT = 2.f;
		float PlayTime = 5.f;



		ImGui::EndChild();
	}
};

void FinalReady::OnEnable()
{
	GameObject::OnEnable();
};

void FinalReady::OnDisable()
{
	GameObject::OnDisable();
};



