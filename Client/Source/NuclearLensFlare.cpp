#include "stdafx.h"
#include "..\Header\NuclearLensFlare.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "ParticleSystem.h"

void NuclearLensFlare::Free()
{
	GameObject::Free();
};

std::string NuclearLensFlare::GetName()
{
	return "NuclearLensFlare";
};

NuclearLensFlare* NuclearLensFlare::Create()
{
	return new NuclearLensFlare{};
};

void NuclearLensFlare::RenderReady()
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

void NuclearLensFlare::RenderInit()
{
	m_nTag = Eff_NuclearLensFlare;
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
	// ����ƽ �޽� �ε�
	Mesh::InitializeInfo _InitInfo{};
	_InitInfo.bLocalVertexLocationsStorage = false;

	_Mesh = Resources::Load<ENGINE::StaticMesh>
		(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx", _InitInfo);

	_Alpg = Resources::Load<ENGINE::Texture>
		(L"..\\..\\Usable\\Artemis\\LightSphereLensFlare.tga");

	PushEditEntity(_Mesh.get());
	PushEditEntity(_Alpg.get());
};

void NuclearLensFlare::PlayStart(
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

void NuclearLensFlare::PlayEnd()
{
	_RenderProperty.bRender = false;
	T = 0.0f;
};

void NuclearLensFlare::UpdatePlayVariable(const float Lerp )
{
	const float ClampLerp = FMath::Clamp(Lerp, 0.0f, 1.f);
	const float CurScale = FMath::Lerp(0.0f, ScaleEnd, ClampLerp);
	GetComponent<Transform>().lock()->SetScale(Vector3{ CurScale ,CurScale ,CurScale });
	CurColorIntencity = FMath::Lerp(0.0f, ColorIntencity, ClampLerp);
}; 

void NuclearLensFlare::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	const uint32 Numsubset = _Mesh->GetNumSubset();

	if (Numsubset > 0)
	{
		_Info.Fx->SetMatrix("matWorld", &World);

		_Info.Fx->SetFloat(
			"ColorIntencity", CurColorIntencity);
		const float AlphaFactor = 1.f;
		_Info.Fx->SetFloat(
			"AlphaFactor", CurColorIntencity *(1.0f/ColorIntencity)  * _AlphaFactor);

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


void NuclearLensFlare::RenderDebug(const DrawInfo& _Info)
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


HRESULT NuclearLensFlare::Ready()
{
	// Ʈ������ �ʱ�ȭ .. 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	// �������� ������ �ް���� ������Ʈ�� Raw �����ͷ� Ǫ��.
	return S_OK;
};

HRESULT NuclearLensFlare::Awake()
{
	GameObject::Awake();

	m_pTransform.lock()->SetPosition(Vector3{ 0.f,0.12f,0.f });
	m_pTransform.lock()->SetScale({ 0.001f,0.001f ,0.001f });
	m_pTransform.lock()->SetRotation(Vector3{ 0.0f ,0.f ,0.0f });

	return S_OK;
}

HRESULT NuclearLensFlare::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT NuclearLensFlare::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	if (_RenderProperty.bRender == false) return 0;

	T += _fDeltaTime;

	// ���� ���� .
	if (T > PlayTime)
	{
		PlayEnd();
	};



	return 0;
}

UINT NuclearLensFlare::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
}

void NuclearLensFlare::Editor()
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

		ImGui::SliderFloat("ColorIntencity", &ColorIntencity, 0.0f, 1.f);
		ImGui::SliderFloat("_AlphaFactor", &_AlphaFactor, 0.0f, 1.f);
		; 
		ImGui::SliderFloat("ScaleEnd", &ScaleEnd, 0.0f, 1.f);
	
		ImGui::EndChild();
	}
};

void NuclearLensFlare::OnEnable()
{
	GameObject::OnEnable();
};

void NuclearLensFlare::OnDisable()
{
	GameObject::OnDisable();
};
