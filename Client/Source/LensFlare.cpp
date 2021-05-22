#include "stdafx.h"
#include "..\Header\LensFlare.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "ParticleSystem.h"

void LensFlare::Free()
{
	GameObject::Free();
};

std::string LensFlare::GetName()
{
	return "LensFlare";
};

LensFlare* LensFlare::Create()
{
	return new LensFlare{};
};

void LensFlare::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();

	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderUpdateInfo.World =
			FMath::Scale(_SpTransform->GetScale()) * 
			Renderer::GetInstance()->_RenderInfo.Billboard * 
			FMath::Translation(_SpTransform->GetPosition());
	}
};

void LensFlare::RenderInit()
{
	m_nTag = Eff_LensFlare;
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
		} 
	};

	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{

		{"LensFlare",
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

	_Alpg = Resources::Load<ENGINE::Texture>
		(L"..\\..\\Usable\\LensFlare\\1.tga");

	PushEditEntity(_Mesh.get());
	PushEditEntity(_Alpg.get());

};

void LensFlare::PlayStart(
	const Vector3& Location)
{
	if (auto SpTransform = GetComponent<Transform>().lock();
		SpTransform)
	{
		SpTransform->SetPosition(Location);
	}

	T = 0.0f;
	_RenderProperty.bRender = true;
};

void LensFlare::PlayEnd()
{
	_RenderProperty.bRender = false;
	T = 0.0f;
};

void LensFlare::
		RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	const uint32 Numsubset = _Mesh->GetNumSubset();

	if (Numsubset > 0)
	{
		_Info.Fx->SetMatrix("matWorld", &World);

		const float SinFactor = std::sinf((T / PlayTime) * FMath::PI); 
		const float CurColorIntencity = SinFactor * ColorIntencity;

		_Info.Fx->SetFloat(
			"ColorIntencity", CurColorIntencity);

		const float AlphaFactor= SinFactor;
		_Info.Fx->SetFloat(
			"AlphaFactor", AlphaFactor);
		_Info.Fx->SetTexture("AlpgMap", _Alpg->GetTexture());
	}

	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = _Mesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
}


void LensFlare::RenderDebug(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);

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


HRESULT LensFlare::Ready()
{
	// 트랜스폼 초기화 .. 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.
	return S_OK;
};

HRESULT LensFlare::Awake()
{
	GameObject::Awake();

	m_pTransform.lock()->SetPosition(Vector3{0.f,0.12f,0.f });
	m_pTransform.lock()->SetScale({ 0.001f,0.001f ,0.001f });
	m_pTransform.lock()->SetRotation(Vector3{ 0.0f ,0.f ,0.0f });

	return S_OK;
}

HRESULT LensFlare::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT LensFlare::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	if (_RenderProperty.bRender == false) return 0;

	T += _fDeltaTime;


	const float CurScale = FMath::Lerp(StartScale, EndScale, T /PlayTime);
	GetComponent<Transform>().lock()->SetScale(Vector3{ CurScale ,CurScale ,CurScale });

	// 끝날 쯔음 .
	if (T > PlayTime)
	{
		PlayEnd();
	};

	return 0;
}

UINT LensFlare::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
}

void LensFlare::Editor()
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

		ImGui::SliderFloat("StartScale", &StartScale, 0.0f, 1.f);
		ImGui::SliderFloat("EndScale", &EndScale, 0.0f, 1.f);

		ImGui::SliderFloat("ColorIntencity", &ColorIntencity, 0.0f, 0.001f);
		;
		ImGui::EndChild();
	}
}


void LensFlare::OnEnable()
{
	GameObject::OnEnable();
}

void LensFlare::OnDisable()
{
	GameObject::OnDisable();
}



