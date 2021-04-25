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
	{
		Element.SubsetIdx = static_cast<float>(FMath::Random<int>(-20, -1));
		D3DXMatrixScaling(&Element.ChildWorldMatrix, 0.05f, 0.05f, 0.05f);
		Element.ChildWorldMatrix._41 += FMath::Random<float>(-3.f, 3.f);
		Element.ChildWorldMatrix._42 = 0.001f;
		Element.ChildWorldMatrix._43 += FMath::Random<float>(-3.f, 3.f);
	}

	_SliceAmount = 1.f;

	Effect::Reset();
}

void AppearGroundMonster::Imgui_Modify()
{
	if (auto Sptransform = GetComponent<ENGINE::Transform>().lock();
		Sptransform)
	{
		ImGui::Text("Eff_AppearGroundMonster");

		{
			static Vector3 SliderPosition = Sptransform->GetPosition();
			ImGui::SliderFloat3("Pos##AppearGroundMonster", SliderPosition, -10.f, 10.f);
			Sptransform->SetPosition(SliderPosition);
		}

		{
			static float Scale = Sptransform->GetScale().x;
			ImGui::SliderFloat("Scale##AppearGroundMonster", &Scale, 0.1f, 1.f);
			//Sptransform->SetScale({ Scale, Scale, Scale });	// x¸¸ À¯È¿
			SetScale(Scale);
		}

		{
			static Vector3 SliderRotation{ 0,0,0 };
			ImGui::SliderFloat3("Rot##AppearGroundMonster", SliderRotation, 0.f, 360.f);
			Sptransform->SetRotation(SliderRotation);
		}

		{
			static float PlayingSpeed = _PlayingSpeed;
			ImGui::SliderFloat("PlayingSpeed##AppearGroundMonster", &PlayingSpeed, 0.1f, 10.f);
			_PlayingSpeed = PlayingSpeed;
		}
	}
}

AppearGroundMonster* AppearGroundMonster::Create()
{
	return new AppearGroundMonster{};
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
	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{
		{"AppearGroundMonster",
		[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		},
	};

	RenderInterface::Initialize(_InitRenderProp);
}

void AppearGroundMonster::RenderGBuffer(const DrawInfo& _Info)
{
	// blood
	_ExtraColor = Vector3(0.55f, 0.f, 0.f);
	_Info.Fx->SetFloatArray("extraColor", _ExtraColor, 3u);
	_Info._Device->SetTexture(0, _BloodALB0Tex->GetTexture());
	_Info._Device->SetTexture(1, _BloodNRMR0Tex->GetTexture());
	_Info.Fx->SetFloat("magicNumber", 0.5f);
	//
	
	for (auto& Element : _BloodMeshVec)
	{
		if (0.f > Element.SubsetIdx)
			continue;

		Matrix World = Element.ChildWorldMatrix * _RenderUpdateInfo.World;
		_Info.Fx->SetMatrix("matWorld", &World);

		auto WeakSubset = Element.Mesh->GetSubset(static_cast<UINT>(Element.SubsetIdx));
		if (auto SharedSubset = WeakSubset.lock();
			SharedSubset)
		{
			SharedSubset->Render(_Info.Fx);
		}
	}
}

void AppearGroundMonster::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	auto WeakSubset = _PlaneMesh->GetSubset(0u);
	if (auto SharedSubset = WeakSubset.lock();
		SharedSubset)
	{
		Matrix World = _DecalBloodChildWorldMatrix * _RenderUpdateInfo.World;
		_Info.Fx->SetMatrix("World", &World);

		_Info.Fx->SetTexture("NRMR0Map", _DecalBloodNRMR0Tex->GetTexture());
		_Info.Fx->SetTexture("Msk0Map", _DecalBloodMsk0Tex->GetTexture());
		_Info.Fx->SetTexture("NoiseMap", _NoiseTex->GetTexture());
		_Info.Fx->SetFloat("_SliceAmount", _SliceAmount);

		SharedSubset->Render(_Info.Fx);
	}
}


HRESULT AppearGroundMonster::Ready()
{
	RenderInit();

	m_nTag = GAMEOBJECTTAG::Eff_AppearGroundMonster;

	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.075f, 0.075f, 0.075f });
	
	_PlaneMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx");

	_BloodMeshVec.reserve(MESH_CNT);

	BloodMesh InitBloodMesh;
	InitBloodMesh.Mesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Liquid\\Blood3.fbx");
	InitBloodMesh.MaxSubsetIdx = 25.f;
	_BloodMeshVec.push_back(InitBloodMesh);
	_BloodMeshVec.push_back(InitBloodMesh);

	InitBloodMesh.Mesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Liquid\\Blood1.fbx");
	InitBloodMesh.MaxSubsetIdx = 15.f;
	_BloodMeshVec.push_back(InitBloodMesh);
	_BloodMeshVec.push_back(InitBloodMesh);

	InitBloodMesh.Mesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Liquid\\Blood2.fbx");
	InitBloodMesh.MaxSubsetIdx = 17.f;
	_BloodMeshVec.push_back(InitBloodMesh);
	_BloodMeshVec.push_back(InitBloodMesh);

	_BloodALB0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\mesh_capcom_liquid_common_blood_ALBM.tga");
	_BloodNRMR0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\mesh_capcom_liquid_common_blood_NRMR.tga");

	_DecalBloodNRMR0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\tex_03_decal_blood_env_0000_nrmr.tga");
	_DecalBloodMsk0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\tex_03_decal_blood_env_0000_msk2.tga");
	
	Matrix TempMat;
	D3DXMatrixScaling(&_DecalBloodChildWorldMatrix, 0.1f, 0.1f, 0.1f);
	D3DXMatrixRotationX(&TempMat, D3DXToRadian(-90.f));
	_DecalBloodChildWorldMatrix *= TempMat;
	_DecalBloodChildWorldMatrix._42 = 0.001f;

	_NoiseTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\noiseInput_ATOS.tga");

	_PlayingSpeed = 1.f;

	Reset();

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
	if (5.f < _AccumulateTime)
		Reset();
	else if (4.f < _AccumulateTime)
		_SliceAmount =  (_AccumulateTime - 4.f) * 0.6f;
	else
		_SliceAmount = 1.f - _AccumulateTime * 0.6f;

	//
	for (auto& Element : _BloodMeshVec)
	{
		if (1.5f < _AccumulateTime)
			Element.SubsetIdx += _fDeltaTime * 40.f;

		if (Element.MaxSubsetIdx < Element.SubsetIdx)
		{
			if (4.f < _AccumulateTime)
				Element.SubsetIdx = -999.f;
			else
			{
				Element.SubsetIdx = static_cast<float>(FMath::Random<int>(-20, 0));
				D3DXMatrixScaling(&Element.ChildWorldMatrix, 0.05f, 0.05f, 0.05f);
				Element.ChildWorldMatrix._41 += FMath::Random<float>(-3.f, 3.f);
				Element.ChildWorldMatrix._42 = 0.001f;
				Element.ChildWorldMatrix._43 += FMath::Random<float>(-3.f, 3.f);
			}
		}
	}

	//
	//Imgui_Modify();

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