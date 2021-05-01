#include "stdafx.h"
#include "..\Header\StoneDebris.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"


void StoneDebris::SetVariationIdx(StoneDebris::VARIATION Idx)
{
	if (Idx > MAX_VARIATION_IDX)
		return;

	Reset();

	_VariationIdx = Idx;
}

void StoneDebris::Free()
{
	GameObject::Free();
}

std::string StoneDebris::GetName()
{
	return "StoneDebris";
}

void StoneDebris::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const Vector3 Scale = _SpTransform->GetScale();
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();

		const auto& _Subset = _DebrisMesh->GetSubset(_SubsetIdx);
		const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

		_RenderUpdateInfo.SubsetCullingSphere.resize(1);
		_RenderUpdateInfo.SubsetCullingSphere[0] = _CurBS.Transform(_RenderUpdateInfo.World, Scale.x);
	}
}

void StoneDebris::Reset()
{
	_SubsetIdx = 0u;

	Effect::Reset();
}

void StoneDebris::Imgui_Modify()
{
	if (auto Sptransform = GetComponent<ENGINE::Transform>().lock();
		Sptransform)
	{
		ImGui::Text("Eff_StoneDebris");

		{
			static Vector3 SliderPosition = Sptransform->GetPosition();
			ImGui::SliderFloat3("Pos##Eff_StoneDebris", SliderPosition, -10.f, 10.f);
			Sptransform->SetPosition(SliderPosition);
		}

		{
			static float Scale = Sptransform->GetScale().x;
			ImGui::SliderFloat("Scale##Eff_StoneDebris", &Scale, 0.001f, 0.01f);
			//Sptransform->SetScale({ Scale, Scale, Scale });	// x만 유효
			SetScale(Scale);
		}

		{
			static Vector3 SliderRotation{ 0,0,0 };
			ImGui::SliderFloat3("Rot##Eff_StoneDebris", SliderRotation, 0.f, 360.f);
			Sptransform->SetRotation(SliderRotation);
		}

		{
			static float PlayingSpeed = _PlayingSpeed;
			ImGui::SliderFloat("PlayingSpeed##Eff_StoneDebris", &PlayingSpeed, 1.f, 100.f);
			_PlayingSpeed = PlayingSpeed;
		}

		{
			static int VariationIdx = _VariationIdx;
			ImGui::SliderInt("VariationIdx##Eff_StoneDebris", &VariationIdx, 0, 2);
			_VariationIdx = (StoneDebris::VARIATION)VariationIdx;
		}

		{
			static int SubsetIdx = _SubsetIdx;
			ImGui::SliderInt("SubsetIdx##Eff_StoneDebris", &SubsetIdx, 0, 3);
			_SubsetIdx = (uint32)SubsetIdx;
		}
	}
}

void StoneDebris::RenderInit()
{
	SetRenderEnable(true);

	ENGINE::RenderProperty _InitRenderProp;
	_InitRenderProp.bRender = false;
	_InitRenderProp.RenderOrders[RenderProperty::Order::GBuffer] =
	{
		{"GBufferEffect",
		[this](const DrawInfo& _Info)
			{
				this->RenderGBuffer(_Info);
			}
		},
	};
	_InitRenderProp.RenderOrders[RenderProperty::Order::AlphaBlendEffect] =
	{
		{"Sprite",
		[this](const DrawInfo& _Info)
			{
				this->RenderAlphaBlendEffect(_Info);
			}
		},
	};
	RenderInterface::Initialize(_InitRenderProp);
}

void StoneDebris::RenderGBuffer(const DrawInfo& _Info)
{
	if (!_Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[0]))
		return;

	_Info.Fx->SetMatrix("matWorld", &_RenderUpdateInfo.World);

	switch (_VariationIdx)
	{
	case STONE_0:
		_Info._Device->SetTexture(0, _DebrisALBMTex->GetTexture());
		_Info._Device->SetTexture(1, _DebrisNRMRTex->GetTexture());
		_BrightScale = 0.1f;
		break;
	case REDORB_0:
		_Info._Device->SetTexture(0, _RedOrbALBMTex->GetTexture());
		_Info._Device->SetTexture(1, _RedOrbNRMRTex->GetTexture());
		_BrightScale = 1.f;
		break;
	case GREENORB_0:
		_Info._Device->SetTexture(0, _GreenOrbALBMTex->GetTexture());
		_Info._Device->SetTexture(1, _GreenOrbNRMRTex->GetTexture());
		_BrightScale = 1.f;
		break;
	}

	_Info.Fx->SetFloat("brightScale", _BrightScale);

	auto WeakSubset = _DebrisMesh->GetSubset(_SubsetIdx);
	if (auto SharedSubset = WeakSubset.lock();
		SharedSubset)
	{
		SharedSubset->Render(_Info.Fx);
	}
}

void StoneDebris::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	auto WeakSubset = _DebrisMesh->GetSubset(_SubsetIdx);
	if (auto SharedSubset = WeakSubset.lock();
		SharedSubset)
	{
		auto WeakSubset_Plane = _PlaneMesh->GetSubset(0u);
		if (auto SharedSubset_Plane = WeakSubset_Plane.lock();
			SharedSubset_Plane)
		{
			_Info.Fx->SetTexture("ALB0Map", _DustSingleTex->GetTexture());
			//_Info.Fx->SetTexture("NoiseMap", _SmokeALB0Tex->GetTexture());
			_Info.Fx->SetBool("_UsingNoise", false);
			_Info.Fx->SetFloat("_SliceAmount", 0.f);
			_Info.Fx->SetFloat("_BrightScale", 1.f);
			//_Info.Fx->SetFloat("SoftParticleDepthScale", _SoftParticleDepthScale);
			_Info.Fx->SetFloatArray("_MinTexUV", Vector2(0.f, 0.f), 2u);
			_Info.Fx->SetFloatArray("_MaxTexUV", Vector2(0.25f, 0.25f), 2u);
						
			for (auto& p : *SharedSubset->GetVertexBufferDesc().LocalVertexLocation)
			{
				Matrix temp;
				D3DXMatrixScaling(&temp, 0.001f, 0.001f, 0.001f);
				Vector3 pos;
				D3DXVec3TransformCoord(&pos, &p, &_RenderUpdateInfo.World);
				temp._41 += pos.x;
				temp._42 += pos.y;
				temp._43 += pos.z;

				_Info.Fx->SetMatrix("World", &temp);
				SharedSubset->Render(_Info.Fx);		// 너무 공간이 빔 ㅠㅠ
			}
		}
	}
}

StoneDebris* StoneDebris::Create()
{
	return new StoneDebris{};
}

HRESULT StoneDebris::Ready()
{
	RenderInit();

	m_nTag = GAMEOBJECTTAG::Eff_StoneDebris;

	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.001f, 0.001f, 0.001f });

	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = true;
	_DebrisMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Stone\\mesh_capcom_debris_stone00_small.fbx", _Info);

	_DebrisALBMTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\mesh_capcom_debris_stone00_ALBM.tga");
	_DebrisNRMRTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\mesh_capcom_debris_stone00_NRMR.tga");
	_RedOrbALBMTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\orb000_0_ALBM.tga");
	_RedOrbNRMRTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\orb000_0_NRMR.tga");
	_GreenOrbALBMTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\orb001_0_Low_ALBM.tga");
	_GreenOrbNRMRTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\orb001_0_Low_NRMR.tga");

	_PlaneMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx");
	_DustSingleTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\tex_03_dust_single_0003_alpg.tga");

	_PlayingSpeed = 1.f;
	_BrightScale = 0.1f;

	return S_OK;
};

HRESULT StoneDebris::Awake()
{
	return S_OK;
}

HRESULT StoneDebris::Start()
{
	return S_OK;
}

UINT StoneDebris::Update(const float _fDeltaTime)
{
	Effect::Update(_fDeltaTime);

	if (!_IsPlaying)
		return 0;

	//
	//_SubsetIdx = static_cast<uint32>(_AccumulateTime);
	//if (MAX_SUBSET_IDX < _SubsetIdx)
	//	Reset();

	//
	Imgui_Modify();

	return 0;
}

UINT StoneDebris::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void StoneDebris::Editor()
{
	GameObject::Editor();
}

void StoneDebris::OnEnable()
{
	GameObject::OnEnable();
}

void StoneDebris::OnDisable()
{
	GameObject::OnDisable();
}