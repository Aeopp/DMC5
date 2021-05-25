#include "stdafx.h"
#include "..\Header\Energism.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "ParticleSystem.h"
#include "NuclearLensFlare.h"

void Energism::Free()
{
	GameObject::Free();
};

std::string Energism::GetName()
{
	return "Energism";
};

Energism* Energism::Create()
{
	return new Energism{};
};

void Energism::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();

	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
	}
};

void Energism::RenderInit()
{
	m_nTag = Eff_Energism;
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

		{"Energism",
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
		(L"..\\..\\Usable\\Artemis\\Energism.fbx", _InitInfo);


	_AlbmMap =  Resources::Load<ENGINE::Texture>(
		"..\\..\\Resource\\Texture\\Effect\\lightning_alb.png"
		);

	_NoiseMap = Resources::Load<ENGINE::Texture>(
		"..\\..\\Usable\\Artemis\\1.tga"
		);

	;
	PushEditEntity(_Mesh.get());

};

void Energism::PlayStart(const Vector3& Location)
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

void Energism::PlayEnd()
{
	T = 0.0f;
	_RenderProperty.bRender = false;
};

void Energism::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	const uint32 Numsubset = _Mesh->GetNumSubset();

	if (Numsubset > 0)
	{
		_Info.Fx->SetMatrix("matWorld", &World);
		_Info.Fx->SetTexture("AlbmMap", _AlbmMap->GetTexture());
		_Info.Fx->SetTexture("NoiseMap", _NoiseMap->GetTexture());
		
		_Info.Fx->SetFloat("Time",  T*TimeCorr);
		_Info.Fx->SetFloat("ColorIntencity", ColorIntencity);
		_Info.Fx->SetFloat("CurveScale", CurveScale);
		;
		;
	}

	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset =
			_Mesh->GetSubset(i).lock();
			SpSubset)
		{

			SpSubset->Render(_Info.Fx);
		};
	};
}


void Energism::RenderDebug(const DrawInfo& _Info)
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


HRESULT Energism::Ready()
{
	// Ʈ������ �ʱ�ȭ .. 
	auto InitTransform = GetComponent<ENGINE::Transform>();
	PushEditEntity(InitTransform.lock().get());
	RenderInit();
	// �������� ������ �ް���� ������Ʈ�� Raw �����ͷ� Ǫ��.
	return S_OK;
};

HRESULT Energism::Awake()
{
	GameObject::Awake();

	m_pTransform.lock()->SetScale({ 0.0020f,0.0020f,0.0020f });
	m_pTransform.lock()->SetPosition(Vector3{ -37.411f,0.821f,30.663f });
	m_pTransform.lock()->SetRotation(Vector3{ 0.0f,0.f ,0.0f });

	return S_OK;
}

HRESULT Energism::Start()
{
	GameObject::Start();

	return S_OK;
}

UINT Energism::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	if (_RenderProperty.bRender == false) return 0;

	T += _fDeltaTime;

	if (T > PlayTime)
	{
		PlayEnd();
	};

	return 0;
}


UINT Energism::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	return 0;
}

void Energism::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		const std::string ChildName = GetName() + "_Play";
		ImGui::BeginChild(ChildName.c_str());
		if (ImGui::SmallButton("Play"))
		{
			PlayStart(
				GetComponent<Transform>().lock()->GetPosition());
		}

		ImGui::Text("T : %2.6f", T);

		ImGui::SliderFloat("TimeCorr", &TimeCorr, 0.0f, 10.f);
		ImGui::SliderFloat("ColorIntencity", &ColorIntencity, 
			0.0f, 10.f);
		ImGui::SliderFloat("CurveScale", &CurveScale,
			0.0f, 100.f);

		;
		;

		ImGui::EndChild();
	}
}


void Energism::OnEnable()
{
	GameObject::OnEnable();
}

void Energism::OnDisable()
{
	GameObject::OnDisable();
}



