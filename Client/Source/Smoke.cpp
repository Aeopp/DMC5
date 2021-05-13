#include "stdafx.h"
#include "..\Header\Smoke.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"


void Smoke::SetVariationIdx(Smoke::VARIATION Idx)
{
	if (Idx > MAX_VARIATION_IDX)
		return;

	switch (Idx)
	{
	case SMOKE_0: default:
		_SliceAmount = 0.f;
		_BrightScale = 0.00005f;
		break;
	case SMOKE_1:
		_SliceAmount = 0.f;
		_BrightScale = 0.01f;
		break;
	case SMOKE_2:
		_SliceAmount = 0.f;
		_BrightScale = 0.01f;
		break;
	case APPEAR_AERIAL_MONSTER:
		_SliceAmount = 1.f;
		_BrightScale = 0.00005f;
		break;
	}

	Reset();

	_VariationIdx = Idx;
}

void Smoke::Free()
{
	GameObject::Free();
}

std::string Smoke::GetName()
{
	return "Smoke";
}

void Smoke::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const Vector3 Scale = _SpTransform->GetScale();
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();

		const auto& _Subset = _SmokeMesh->GetSubset(0);
		const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

		_RenderUpdateInfo.SubsetCullingSphere.resize(1);
		_RenderUpdateInfo.SubsetCullingSphere[0] = _CurBS.Transform(_RenderUpdateInfo.World, Scale.x);
	}
}

void Smoke::Reset()
{
	_SpriteIdx = 0.f;

	if (APPEAR_AERIAL_MONSTER == _VariationIdx)
		_SliceAmount = 1.f;
	else
		_SliceAmount = 0.f;

	Effect::Reset();
}

void Smoke::Imgui_Modify()
{
	if (auto Sptransform = GetComponent<ENGINE::Transform>().lock();
		Sptransform)
	{
		ImGui::Text("Eff_Smoke : %d", _VariationIdx);

		{
			static Vector3 Position = Sptransform->GetPosition();
			ImGui::InputFloat3("Pos##Smoke", Position);
			Sptransform->SetPosition(Position);
		}

		{
			static float Scale = Sptransform->GetScale().x;
			ImGui::SliderFloat("Scale##Smoke", &Scale, 0.001f, 0.3f);
			//Sptransform->SetScale({ Scale, Scale, Scale });	// x만 유효
			SetScale(Scale);
		}

		{
			static Vector3 SliderRotation{ 0,0,0 };
			ImGui::SliderFloat3("Rot##Smoke", SliderRotation, 0.f, 360.f);
			Sptransform->SetRotation(SliderRotation);
		}

		{
			static float PlayingSpeed = _PlayingSpeed;
			ImGui::SliderFloat("PlayingSpeed##Smoke", &PlayingSpeed, 0.1f, 10.f);
			_PlayingSpeed = PlayingSpeed;
		}

		{
			static int VariationIdx = _VariationIdx;
			ImGui::SliderInt("VariationIdx##Smoke", &VariationIdx, 0, 2);
			if (ImGui::Button("Apply##Smoke"))
				SetVariationIdx((Smoke::VARIATION)VariationIdx);
		}

		{
			if (ImGui::Button("PlayStart##Smoke"))
				PlayStart(_PlayingSpeed);
			ImGui::SameLine();
			if (ImGui::Button("Reset##Smoke"))
				Reset();
		}
	}
}

Smoke* Smoke::Create()
{
	return new Smoke{};
}

void Smoke::RenderInit()
{
	SetRenderEnable(true);

	ENGINE::RenderProperty _InitRenderProp;
	_InitRenderProp.bRender = false;
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

void Smoke::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	if (!_Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[0]))
		return;

	//auto RefEffInfo = std::any_cast<EffectInfo>((_Info.BySituation));

	auto WeakSubset = _SmokeMesh->GetSubset(0u);
	if (auto SharedSubset = WeakSubset.lock();
		SharedSubset)
	{
		if (APPEAR_AERIAL_MONSTER == _VariationIdx)
		{
			_Info.Fx->SetMatrix("World", &_RenderUpdateInfo.World);
			_Info.Fx->SetTexture("ALB0Map", _SmokeALB0Tex->GetTexture());
			_Info.Fx->SetBool("_UsingNoise", true);
			_Info.Fx->SetTexture("NoiseMap", _SmokeALB0Tex->GetTexture());
			_Info.Fx->SetFloat("_SliceAmount", _SliceAmount);
			_Info.Fx->SetFloat("_BrightScale", _BrightScale);
			//_Info.Fx->SetFloat("SoftParticleDepthScale", _SoftParticleDepthScale);
			_Info.Fx->SetFloatArray("_MinTexUV", _SmokeMinTexUV, 2u);
			_Info.Fx->SetFloatArray("_MaxTexUV", _SmokeMaxTexUV, 2u);

			SharedSubset->Render(_Info.Fx);
			SharedSubset->Render(_Info.Fx);	// 옅어서 한번 더 그림
		}
		else
		{
			_Info.Fx->SetMatrix("World", &_RenderUpdateInfo.World);
			switch (_VariationIdx)
			{
			case SMOKE_0: default:
				_Info.Fx->SetTexture("ALB0Map", _SmokeALB0Tex->GetTexture());
				break;
			case SMOKE_1:
				_Info.Fx->SetTexture("ALB0Map", _SmokeALB1Tex->GetTexture());
				break;
			case SMOKE_2:
				_Info.Fx->SetTexture("ALB0Map", _SmokeALB2Tex->GetTexture());
				break;
			}
			_Info.Fx->SetBool("_UsingNoise", false);
			_Info.Fx->SetFloat("_SliceAmount", _SliceAmount);
			_Info.Fx->SetFloat("_BrightScale", _BrightScale);
			//_Info.Fx->SetFloat("SoftParticleDepthScale", _SoftParticleDepthScale);
			_Info.Fx->SetFloatArray("_MinTexUV", _SmokeMinTexUV, 2u);
			_Info.Fx->SetFloatArray("_MaxTexUV", _SmokeMaxTexUV, 2u);

			SharedSubset->Render(_Info.Fx);
		}
	}
}


HRESULT Smoke::Ready()
{
	RenderInit();

	m_nTag = GAMEOBJECTTAG::Eff_Smoke;

	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.1f, 0.1f, 0.1f });	// _SmokeMesh 원점이 중앙이 아니라 스케일 늘리면 x축 이동도 필요
	
	//_PlaneMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx");
	_SmokeMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\mesh_03_enviroment_smoke00_03.fbx");

	_SmokeALB0Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\tex_capcom_smoke_00_0016_alpg.tga");
	_SmokeALB1Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\tex_capcom_smoke_00_0049_alpg.tga");
	_SmokeALB2Tex = Resources::Load<ENGINE::Texture>(L"..\\..\\Usable\\Smoke\\02.tga");

	_NoiseTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\noiseInput_ATOS.tga");

	_PlayingSpeed = 1.f;
	_BrightScale = 0.00005f;

	Reset();

	return S_OK;
};

HRESULT Smoke::Awake()
{
	return S_OK;
}

HRESULT Smoke::Start()
{
	return S_OK;
}

UINT Smoke::Update(const float _fDeltaTime)
{
	Effect::Update(_fDeltaTime);

	if (!_IsPlaying)
		return 0;

	// SMOKE_0는 Reset() 호출까지 계속 재생
	if (APPEAR_AERIAL_MONSTER == _VariationIdx)
	{
		//
		if (5.f < _AccumulateTime)
			Reset();
		else if (4.f < _AccumulateTime)
			_SliceAmount = (_AccumulateTime - 4.f) * 0.6f;
		else
			_SliceAmount = 1.f - _AccumulateTime * 0.6f;
	}

	// sprite
	_SpriteIdx += _PlayingSpeed * _fDeltaTime;

	float cx = 8.f;	// 가로 갯수
	float cy = 8.f; // 세로 갯수

	if (cx * cy < _SpriteIdx)
		_SpriteIdx = 0.f;

	int Frame = static_cast<int>(_SpriteIdx);
	int w = Frame % static_cast<int>(cx);
	int h = Frame / static_cast<int>(cx);
	_SmokeMinTexUV.x = 1.f / cx * static_cast<float>(w);
	_SmokeMinTexUV.y = 1.f / cy * static_cast<float>(h);
	_SmokeMaxTexUV.x = _SmokeMinTexUV.x + 1.f / cx;
	_SmokeMaxTexUV.y = _SmokeMinTexUV.y + 1.f / cy;

	// y축 빌보드
	//Matrix TempMat, BillMat;
	//TempMat = Renderer::GetInstance()->_RenderInfo.View;
	//D3DXMatrixIdentity(&BillMat);
	//BillMat._11 = TempMat._11;
	//BillMat._13 = TempMat._13;
	//BillMat._31 = TempMat._31;
	//BillMat._33 = TempMat._33;
	//D3DXMatrixInverse(&BillMat, 0, &BillMat);
	////BillMat._42 = -1.2f;
	//
	//if (auto Sptransform = GetComponent<ENGINE::Transform>().lock();
	//	Sptransform)
	//	Sptransform->SetBillBoard(BillMat);

	return 0;
}

UINT Smoke::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void Smoke::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		Imgui_Modify();
	}
}

void Smoke::OnEnable()
{
	GameObject::OnEnable();
}

void Smoke::OnDisable()
{
	GameObject::OnDisable();
}