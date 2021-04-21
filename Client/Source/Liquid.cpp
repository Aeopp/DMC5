#include "stdafx.h"
#include "..\Header\Liquid.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"


void Liquid::Free()
{
	GameObject::Free();
}

std::string Liquid::GetName()
{
	return "Liquid";
};

void Liquid::Reset()
{
	_SubsetIdx = 0u;

	Effect::Reset();
}

void Liquid::Imgui_Modify()
{
	if (auto Sptransform = GetComponent<ENGINE::Transform>().lock();
		Sptransform)
	{
		ImGui::Text("Eff_Liquid : %d", _VariationIdx);

		{
			Vector3 SliderPosition = Sptransform->GetPosition();
			ImGui::SliderFloat3("Pos##Liquid", SliderPosition, -10.f, 10.f);
			Sptransform->SetPosition(SliderPosition);
		}

		{
			float Scale = Sptransform->GetScale().x;
			ImGui::SliderFloat("Scale##Liquid", &Scale, 0.1f, 1.f);
			//Sptransform->SetScale({ Scale, Scale, Scale });	// x¸¸ À¯È¿
			SetScale(Scale);
		}

		{
			Vector3 SliderRotation{ 0,0,0 };
			ImGui::SliderFloat3("Rot##Liquid", SliderRotation, 0.f, 360.f);
			Sptransform->SetRotation(SliderRotation);
		}

		{
			float PlayingSpeed = _PlayingSpeed;
			ImGui::SliderFloat("PlayingSpeed##Liquid", &PlayingSpeed, 0.1f, 10.f);
			_PlayingSpeed = PlayingSpeed;
		}
	}
}

void Liquid::RenderInit()
{
	SetRenderEnable(true);

	ENGINE::RenderProperty _InitRenderProp;
	_InitRenderProp.bRender = false;
	_InitRenderProp.RenderOrders[RenderProperty::Order::GBuffer] =
	{
		{"Liquid",
		[this](const DrawInfo& _Info)
			{
				this->RenderGBuffer(_Info);
			}
		},
	};
	RenderInterface::Initialize(_InitRenderProp);
}

void Liquid::RenderGBuffer(const DrawInfo& _Info)
{
	if (MAX_VARIATION_IDX <= _VariationIdx)
		return;

	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);

	// blood
	_ExtraColor = Vector3(1.f, 0.f, 0.f);
	_Info.Fx->SetFloatArray("extraColor", _ExtraColor, 3u);
	_Info._Device->SetTexture(0, _BloodALB0Tex->GetTexture());
	_Info._Device->SetTexture(1, _BloodNRMR0Tex->GetTexture());
	//
	
	// meat
	//_ExtraColor = Vector3(1.f, 1.f, 1.f);
	//_Info.Fx->SetFloatArray("extraColor", _ExtraColor, 3u);
	//_Info._Device->SetTexture(0, _MeatALB0Tex->GetTexture());
	//_Info._Device->SetTexture(1, _MeatNRMR0Tex->GetTexture());
	//

	auto WeakSubset = _LiquidMeshVec[_VariationIdx]->GetSubset(_SubsetIdx);
	if (auto SharedSubset = WeakSubset.lock();
		SharedSubset)
	{
		SharedSubset->Render(_Info.Fx);
	}
}

Liquid* Liquid::Create()
{
	return new Liquid{};
}

HRESULT Liquid::Ready()
{
	RenderInit();

	m_nTag = GAMEOBJECTTAG::Eff_Liquid;

	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.1f, 0.1f, 0.1f });

	_LiquidMeshVec.reserve(MAX_VARIATION_IDX);
	_LiquidMeshVec.push_back(Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Liquid\\Blood4.fbx"));
	_LiquidMeshVec.push_back(Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Liquid\\Blood3.fbx"));
	_LiquidMeshVec.push_back(Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Liquid\\Blood1.fbx"));
	_LiquidMeshVec.push_back(Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Liquid\\Blood2.fbx"));
	_LiquidMeshVec.push_back(Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Liquid\\mesh_capcom_liquid_splash00_12.fbx"));
	_LiquidMeshVec.push_back(Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Liquid\\mesh_capcom_liquid_splash00_11.fbx"));
	_LiquidMeshVec.push_back(Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Liquid\\mesh_capcom_liquid_splash00_03.fbx"));
	_LiquidMeshVec.push_back(Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Liquid\\mesh_capcom_liquid_splash00_14.fbx"));
	_LiquidMeshVec.push_back(Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Liquid\\chain_capcom_liquid_splash00_00.fbx"));
	_LiquidMeshVec.push_back(Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Liquid\\chain_capcom_liquid_splash01_00.fbx"));
	_LiquidMeshVec.push_back(Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Liquid\\chain_capcom_liquid_splash02_00.fbx"));
	
	_BloodALB0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\mesh_capcom_liquid_common_blood_ALBM.tga");
	_BloodNRMR0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\mesh_capcom_liquid_common_blood_NRMR.tga");
	_MeatALB0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\mesh_capcom_liquid_common_Meat_ALBM.tga");
	_MeatNRMR0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\mesh_capcom_liquid_common_Meat_NRMR.tga");

	_PlayingSpeed = 1.f;

	SetVariationIdx(_VariationIdx);

	return S_OK;
};

HRESULT Liquid::Awake()
{
	return S_OK;
}

HRESULT Liquid::Start()
{
	return S_OK;
}

UINT Liquid::Update(const float _fDeltaTime)
{
	Effect::Update(_fDeltaTime);

	//
	_SubsetIdx = static_cast<uint32>(_AccumulateTime);
	if (_SubsetMaxIdx < _SubsetIdx)
		Reset();

	//
	Imgui_Modify();

	return 0;
}

UINT Liquid::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void Liquid::Editor()
{
	GameObject::Editor();
}

void Liquid::OnEnable()
{
	GameObject::OnEnable();
}

void Liquid::OnDisable()
{
	GameObject::OnDisable();
}

void Liquid::SetVariationIdx(Liquid::VARIATION Idx)
{
	if (Idx > MAX_VARIATION_IDX)
		return;

	Reset();

	switch (Idx)
	{
	case BLOOD_0:	// Blood4.fbx
		_SubsetMaxIdx = 21u;
		break;
	case BLOOD_1:	// Blood3.fbx
		_SubsetMaxIdx = 25u;
		break;
	case BLOOD_2:	// Blood1.fbx
		_SubsetMaxIdx = 15u;
		break;
	case BLOOD_3:	// Blood2.fbx
		_SubsetMaxIdx = 17u;
		break;
	case BLOOD_4:	// mesh_capcom_liquid_splash00_12.fbx
		_SubsetMaxIdx = 30u;
		break;
	case BLOOD_5:	// mesh_capcom_liquid_splash00_11.fbx
		_SubsetMaxIdx = 25u;
		break;
	case BLOOD_6:	// mesh_capcom_liquid_splash00_03.fbx
		_SubsetMaxIdx = 10u;
		break;
	case BLOOD_7:	// mesh_capcom_liquid_splash00_14.fbx
		_SubsetMaxIdx = 18u;
		break;
	case BLOOD_8:	// chain_capcom_liquid_splash00_00.fbx
		_SubsetMaxIdx = 29u;
		break;
	case BLOOD_9:	// chain_capcom_liquid_splash01_00.fbx
		_SubsetMaxIdx = 63u;
		break;
	case BLOOD_10:	// chain_capcom_liquid_splash02_00.fbx
		_SubsetMaxIdx = 31u;
		break;
	default:
		_SubsetMaxIdx = 0u;
		break;
	}

	_VariationIdx = Idx;
}