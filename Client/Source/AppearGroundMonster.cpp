#include "stdafx.h"
#include "..\Header\AppearGroundMonster.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"


void AppearGroundMonster::Free()
{
	_BloodMeshVec.clear();
	_BloodMeshVec.shrink_to_fit();

	GameObject::Free();
}

std::string AppearGroundMonster::GetName()
{
	return "AppearGroundMonster";
};

void AppearGroundMonster::Reset()
{
	for (auto& Element : _BloodMeshVec)
		Element.SubsetIdx = 0.f;

	Effect::Reset();
}

void AppearGroundMonster::Imgui_Modify()
{
	if (auto Sptransform = GetComponent<ENGINE::Transform>().lock();
		Sptransform)
	{
		ImGui::Text("Eff_AppearGroundMonster");

		{
			Vector3 SliderPosition = Sptransform->GetPosition();
			ImGui::SliderFloat3("Pos##AppearGroundMonster", SliderPosition, -10.f, 10.f);
			Sptransform->SetPosition(SliderPosition);
		}

		{
			float Scale = Sptransform->GetScale().x;
			ImGui::SliderFloat("Scale##AppearGroundMonster", &Scale, 0.1f, 1.f);
			//Sptransform->SetScale({ Scale, Scale, Scale });	// x만 유효
			SetScale(Scale);
		}

		{
			Vector3 SliderRotation{ 0,0,0 };
			ImGui::SliderFloat3("Rot##AppearGroundMonster", SliderRotation, 0.f, 360.f);
			Sptransform->SetRotation(SliderRotation);
		}

		{
			float PlayingSpeed = _PlayingSpeed;
			ImGui::SliderFloat("PlayingSpeed##AppearGroundMonster", &PlayingSpeed, 0.1f, 10.f);
			_PlayingSpeed = PlayingSpeed;
		}
	}
}

void AppearGroundMonster::RenderInit()
{
	SetRenderEnable(true);

	ENGINE::RenderProperty _InitRenderProp;
	_InitRenderProp.bRender = false;
	_InitRenderProp.RenderOrders[RenderProperty::Order::GBuffer] =
	{
		{"GBufferLiquid",
		[this](const DrawInfo& _Info)
			{
				this->RenderGBuffer(_Info);
			}
		},
	};

	// + 바닥 blood decal

	RenderInterface::Initialize(_InitRenderProp);
}

void AppearGroundMonster::RenderGBuffer(const DrawInfo& _Info)
{
	// blood
	_ExtraColor = Vector3(1.f, 0.f, 0.f);
	_Info.Fx->SetFloatArray("extraColor", _ExtraColor, 3u);
	_Info._Device->SetTexture(0, _BloodALB0Tex->GetTexture());
	_Info._Device->SetTexture(1, _BloodNRMR0Tex->GetTexture());
	//
	
	for (auto& Element : _BloodMeshVec)
	{
		Matrix World = Element.WorldMatrix * _RenderUpdateInfo.World;
		_Info.Fx->SetMatrix("matWorld", &World);

		auto WeakSubset = Element.Mesh->GetSubset(static_cast<UINT>(Element.SubsetIdx));
		if (auto SharedSubset = WeakSubset.lock();
			SharedSubset)
		{
			SharedSubset->Render(_Info.Fx);
		}
	}
}

AppearGroundMonster* AppearGroundMonster::Create()
{
	return new AppearGroundMonster{};
}

HRESULT AppearGroundMonster::Ready()
{
	RenderInit();

	m_nTag = GAMEOBJECTTAG::Eff_AppearGroundMonster;

	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.1f, 0.1f, 0.1f });

	_BloodMeshVec.reserve(MESH_CNT);

	BloodMesh InitBloodMesh;
	InitBloodMesh.Mesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Liquid\\Blood3.fbx");
	InitBloodMesh.SubsetIdx = 0.f;
	InitBloodMesh.MaxSubsetIdx = 25.f;
	D3DXMatrixIdentity(&InitBloodMesh.WorldMatrix);
InitBloodMesh.WorldMatrix._41 -= 5.f;
	_BloodMeshVec.push_back(InitBloodMesh);

	InitBloodMesh.Mesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Liquid\\Blood1.fbx");
	InitBloodMesh.SubsetIdx = 0.f;
	InitBloodMesh.MaxSubsetIdx = 15.f;
	D3DXMatrixIdentity(&InitBloodMesh.WorldMatrix);
	_BloodMeshVec.push_back(InitBloodMesh);

	InitBloodMesh.Mesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Liquid\\Blood2.fbx");
	InitBloodMesh.SubsetIdx = 0.f;
	InitBloodMesh.MaxSubsetIdx = 17.f;
	D3DXMatrixIdentity(&InitBloodMesh.WorldMatrix);
InitBloodMesh.WorldMatrix._41 += 5.f;
	_BloodMeshVec.push_back(InitBloodMesh);

	_BloodALB0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\mesh_capcom_liquid_common_blood_ALBM.tga");
	_BloodNRMR0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\mesh_capcom_liquid_common_blood_NRMR.tga");

	_PlayingSpeed = 1.f;

	return S_OK;
};

HRESULT AppearGroundMonster::Awake()
{
	return S_OK;
}

HRESULT AppearGroundMonster::Start()
{
	return S_OK;
}

UINT AppearGroundMonster::Update(const float _fDeltaTime)
{
	Effect::Update(_fDeltaTime);

	//
	if (2.f < _AccumulateTime)
		Reset();

	//
	for (auto& Element : _BloodMeshVec)
	{
		Element.SubsetIdx += _fDeltaTime * 40.f;
		if (Element.MaxSubsetIdx < Element.SubsetIdx)
		{
			// + 위치 변경
			Element.SubsetIdx = 0.f;
		}
	}

	//
	Imgui_Modify();

	return 0;
}

UINT AppearGroundMonster::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void AppearGroundMonster::Editor()
{
	GameObject::Editor();
}

void AppearGroundMonster::OnEnable()
{
	GameObject::OnEnable();
}

void AppearGroundMonster::OnDisable()
{
	GameObject::OnDisable();
}