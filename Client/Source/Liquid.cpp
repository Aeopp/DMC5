#include "stdafx.h"
#include "..\Header\Liquid.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"


void Liquid::Free()
{
	_LiquidMeshVec.clear();
	_LiquidMeshVec.shrink_to_fit();

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
			static Vector3 SliderPosition = Sptransform->GetPosition();
			ImGui::SliderFloat3("Pos##Liquid", SliderPosition, -10.f, 10.f);
			Sptransform->SetPosition(SliderPosition);
		}

		{
			static float Scale = Sptransform->GetScale().x;
			ImGui::SliderFloat("Scale##Liquid", &Scale, 0.1f, 1.f);
			//Sptransform->SetScale({ Scale, Scale, Scale });	// x¸¸ À¯È¿
			SetScale(Scale);
		}

		{
			static Vector3 SliderRotation{ 0,0,0 };
			ImGui::SliderFloat3("Rot##Liquid", SliderRotation, 0.f, 360.f);
			Sptransform->SetRotation(SliderRotation);
		}

		{
			static float PlayingSpeed = _PlayingSpeed;
			ImGui::SliderFloat("PlayingSpeed##Liquid", &PlayingSpeed, 1.f, 50.f);
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
		{"GBufferLiquid",
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
	//_ExtraColor = Vector3(0.f, 0.f, 0.f);
	//_Info.Fx->SetFloatArray("extraColor", _ExtraColor, 3u);
	//_Info._Device->SetTexture(0, _MeatALB0Tex->GetTexture());
	//_Info._Device->SetTexture(1, _MeatNRMR0Tex->GetTexture());
	//

	auto WeakSubset = _LiquidMeshVec[_VariationIdx].first->GetSubset(_SubsetIdx);
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
	_LiquidMeshVec.push_back({ Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Liquid\\Blood4.fbx"), 21u });
	_LiquidMeshVec.push_back({ Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Liquid\\Blood3.fbx"), 25u });
	_LiquidMeshVec.push_back({ Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Liquid\\Blood1.fbx"), 15u });
	_LiquidMeshVec.push_back({ Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Liquid\\Blood2.fbx"), 17u });
	_LiquidMeshVec.push_back({ Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Liquid\\mesh_capcom_liquid_splash00_12.fbx"), 30u });
	_LiquidMeshVec.push_back({ Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Liquid\\mesh_capcom_liquid_splash00_11.fbx"), 25u });
	_LiquidMeshVec.push_back({ Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Liquid\\mesh_capcom_liquid_splash00_03.fbx"), 10u });
	_LiquidMeshVec.push_back({ Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Liquid\\mesh_capcom_liquid_splash00_14.fbx"), 18u });
	_LiquidMeshVec.push_back({ Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Liquid\\chain_capcom_liquid_splash00_00.fbx"), 29u });
	_LiquidMeshVec.push_back({ Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Liquid\\chain_capcom_liquid_splash01_00.fbx"), 63u });
	_LiquidMeshVec.push_back({ Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Liquid\\chain_capcom_liquid_splash02_00.fbx"), 31u });
	
	_BloodALB0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\mesh_capcom_liquid_common_blood_ALBM.tga");
	_BloodNRMR0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\mesh_capcom_liquid_common_blood_NRMR.tga");
	_MeatALB0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\mesh_capcom_liquid_common_Meat_ALBM.tga");
	_MeatNRMR0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\mesh_capcom_liquid_common_Meat_NRMR.tga");

	_PlayingSpeed = 40.f;

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

	if (MAX_VARIATION_IDX <= _VariationIdx)
		return 0;

	//
	_SubsetIdx = static_cast<uint32>(_AccumulateTime);
	if (_LiquidMeshVec[_VariationIdx].second < _SubsetIdx)
		Reset();

	//
	//Imgui_Modify();

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

	_VariationIdx = Idx;
}