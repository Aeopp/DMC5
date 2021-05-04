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

	switch (Idx)
	{
	case REDORB_0:
	case GREENORB_0:
		_SubsetIdx = 0u;
		_BrightScale = 1.f;
		break;
	case REDORB_1:
	case GREENORB_1:
		_SubsetIdx = 1u;
		_BrightScale = 1.f;
		break;
	case REDORB_2:
	case GREENORB_2:
		_SubsetIdx = 2u;
		_BrightScale = 1.f;
		break;
	case REDORB_3:
	case GREENORB_3:
		_SubsetIdx = 3u;
		_BrightScale = 1.f;
		break;
	}
	
	if (GREENORB_0 <= Idx)
		_SmokeExtraColor = Vector3(0.09f, 0.596f, 0.518f);
	else
		_SmokeExtraColor = Vector3(0.518f, 0.019f, 0.051f);

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
	//_SubsetIdx = 0u;
	_SmokeSpriteIdx = 0.f;
	_SmokeMinTexUV = Vector2(0.f, 0.f);
	_SmokeMaxTexUV = Vector2(0.f, 0.f);
	_SmokeSliceAmount = 0.f;
	_SmokeDeltaPosY = 0.f;

	uint32 DustSingleIdx = FMath::Random<uint32>(0u, 3u);
	_DustSingleMinTexUV = Vector2(DustSingleIdx * 0.25f, 0.f);
	_DustSingleMaxTexUV = Vector2((DustSingleIdx + 1u) * 0.25f, 0.25f);

	_DustSingleVelocity = Vector3(0.f, 0.f, 0.f);
	_DustSingleDeltaPos = Vector3(0.f, 0.f, 0.f);

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

		if (_Loop)
		{
			static int VariationIdx = _VariationIdx;
			ImGui::SliderInt("VariationIdx##Eff_StoneDebris", &VariationIdx, 0, 7);
			if(ImGui::Button("Apply##Eff_StoneDebris"))
				SetVariationIdx((StoneDebris::VARIATION)VariationIdx);	
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
	if (2.f < _AccumulateTime)
		return;

	if (!_Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[0]))
		return;

	_Info.Fx->SetMatrix("matWorld", &_RenderUpdateInfo.World);

	switch (_VariationIdx)
	{
	case REDORB_0:
	case REDORB_1:
	case REDORB_2:
	case REDORB_3:
		_Info._Device->SetTexture(0, _RedOrbALBMTex->GetTexture());
		_Info._Device->SetTexture(1, _RedOrbNRMRTex->GetTexture());
		break;
	case GREENORB_0:
	case GREENORB_1:
	case GREENORB_2:
	case GREENORB_3:
		_Info._Device->SetTexture(0, _GreenOrbALBMTex->GetTexture());
		_Info._Device->SetTexture(1, _GreenOrbNRMRTex->GetTexture());
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
	if (2.f > _AccumulateTime)
		return;

	auto WeakSubset_Plane = _PlaneMesh->GetSubset(0u);
	if (auto SharedSubset_Plane = WeakSubset_Plane.lock();
		SharedSubset_Plane)
	{
		// Smoke
		const Matrix World = _SmokeChildWorldMatrix * _RenderUpdateInfo.World;
		_Info.Fx->SetMatrix("World", &World);

		_Info.Fx->SetTexture("ALB0Map", _SmokeTex->GetTexture());
		//_Info.Fx->SetTexture("NoiseMap", _SmokeALB0Tex->GetTexture());
		_Info.Fx->SetBool("_UsingNoise", false);
		_Info.Fx->SetFloat("_SliceAmount", _SmokeSliceAmount);
		_Info.Fx->SetFloat("_BrightScale", _BrightScale * 0.05f);
		//_Info.Fx->SetFloat("SoftParticleDepthScale", _SoftParticleDepthScale);
		_Info.Fx->SetFloatArray("_MinTexUV", _SmokeMinTexUV, 2u);
		_Info.Fx->SetFloatArray("_MaxTexUV", _SmokeMaxTexUV, 2u);
		_Info.Fx->SetFloatArray("_ExtraColor", _SmokeExtraColor, 3u);

		SharedSubset_Plane->Render(_Info.Fx);


		// Single Dust
		auto WeakSubset = _DebrisMesh->GetSubset(_SubsetIdx);
		if (auto SharedSubset = WeakSubset.lock();
			SharedSubset)
		{
			_Info.Fx->SetTexture("ALB0Map", _DustSingleTex->GetTexture());
			//_Info.Fx->SetTexture("NoiseMap", _SmokeALB0Tex->GetTexture());
			_Info.Fx->SetBool("_UsingNoise", false);
			//_Info.Fx->SetFloat("_SliceAmount", _SmokeSliceAmount);
			_Info.Fx->SetFloat("_BrightScale", _BrightScale * (1.f - _SmokeSliceAmount) * 7.f);
			//_Info.Fx->SetFloat("SoftParticleDepthScale", _SoftParticleDepthScale);
			_Info.Fx->SetFloatArray("_MinTexUV", _DustSingleMinTexUV, 2u);
			_Info.Fx->SetFloatArray("_MaxTexUV", _DustSingleMaxTexUV, 2u);
			//_Info.Fx->SetFloatArray("_ExtraColor", _SmokeExtraColor, 3u);
						
			for (auto& p : *SharedSubset->GetVertexBufferDesc().LocalVertexLocation)
			{
				Vector3* pPos = reinterpret_cast<Vector3*>(&_DustSingleChildWorldMatrix.m[3][0]);
				D3DXVec3TransformCoord(pPos, &p, &_RenderUpdateInfo.World);
				*pPos += _DustSingleDeltaPos;
				_Info.Fx->SetMatrix("World", &_DustSingleChildWorldMatrix);
				SharedSubset_Plane->Render(_Info.Fx);
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

	_RedOrbALBMTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\orb000_0_ALBM.tga");
	_RedOrbNRMRTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\orb000_0_NRMR.tga");
	_GreenOrbALBMTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\orb001_0_Low_ALBM.tga");
	_GreenOrbNRMRTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\orb001_0_Low_NRMR.tga");

	_PlaneMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx");
	_SmokeTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\tex_capcom_smoke_00_0049_alpg.tga");
	_DustSingleTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\tex_03_dust_single_0003_alpg.tga");

	D3DXMatrixScaling(&_SmokeChildWorldMatrix, 0.2f, 0.2f, 0.2f);
	D3DXMatrixScaling(&_DustSingleChildWorldMatrix, 0.000015f, 0.000015f, 0.000015f);

	uint32 DustSingleIdx = FMath::Random<uint32>(0u, 3u);
	_DustSingleMinTexUV = Vector2(DustSingleIdx * 0.25f, 0.f);
	_DustSingleMaxTexUV = Vector2((DustSingleIdx + 1u) * 0.25f, 0.25f);

	_PlayingSpeed = 1.f;
	_BrightScale = 1.f;

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

	if (4.f < _AccumulateTime)
		Reset();
	else if (2.f < _AccumulateTime)
	{
		Matrix ViewMat, BillMat, InvRotMat;
		ViewMat = Renderer::GetInstance()->_RenderInfo.View;
		D3DXMatrixIdentity(&BillMat);
		memcpy(&BillMat.m[0][0], &ViewMat.m[0][0], sizeof(Vector3));
		memcpy(&BillMat.m[1][0], &ViewMat.m[1][0], sizeof(Vector3));
		memcpy(&BillMat.m[2][0], &ViewMat.m[2][0], sizeof(Vector3));
		D3DXMatrixInverse(&BillMat, 0, &BillMat);
		D3DXMatrixInverse(&InvRotMat, 0, &GetComponent<Transform>().lock()->GetRotationMatrix());

		// Smoke
		_SmokeSpriteIdx += (20.f * _PlayingSpeed * _fDeltaTime);
		_SmokeSliceAmount += (0.8f * _PlayingSpeed * _fDeltaTime);

		float cx = 8.f;	// 가로 갯수
		float cy = 8.f; // 세로 갯수

		if (cx * cy < _SmokeSpriteIdx)
			_SmokeSpriteIdx = 0.f;

		int Frame = static_cast<int>(_SmokeSpriteIdx);
		int w = Frame % static_cast<int>(cx);
		int h = Frame / static_cast<int>(cx);
		_SmokeMinTexUV.x = 1.f / cx * static_cast<float>(w);
		_SmokeMinTexUV.y = 1.f / cy * static_cast<float>(h);
		_SmokeMaxTexUV.x = _SmokeMinTexUV.x + 1.f / cx;
		_SmokeMaxTexUV.y = _SmokeMinTexUV.y + 1.f / cy;

		D3DXMatrixScaling(&_SmokeChildWorldMatrix, 0.2f, 0.2f, 0.2f);
		_SmokeChildWorldMatrix = _SmokeChildWorldMatrix * BillMat * InvRotMat;

		_SmokeDeltaPosY += 5.f * _fDeltaTime;
		_SmokeChildWorldMatrix._42 = _SmokeDeltaPosY;

		// Single Dust
		D3DXMatrixScaling(&_DustSingleChildWorldMatrix, 0.000015f, 0.000015f, 0.000015f);
		_DustSingleChildWorldMatrix = _DustSingleChildWorldMatrix * BillMat * InvRotMat;
	
		_DustSingleVelocity.x = 0.01f * cosf(_AccumulateTime * 2.f);
		_DustSingleVelocity.y = 0.01f;
		_DustSingleVelocity.z = 0.01f * sinf(_AccumulateTime * 2.f);

		_DustSingleDeltaPos.x += _DustSingleVelocity.x * _fDeltaTime;
		_DustSingleDeltaPos.y += _DustSingleVelocity.y * _fDeltaTime;
		_DustSingleDeltaPos.z += _DustSingleVelocity.z * _fDeltaTime;
	}

	//
	//Imgui_Modify();

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