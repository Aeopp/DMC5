#include "stdafx.h"
#include "..\Header\ShapeParticle.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"


void ShapeParticle::SetShapeIdx(ShapeParticle::SHAPE Idx)
{
	if (Idx > MAX_SHAPE_IDX)
		return;

	_ShapeIdx = Idx;
	_ChangedShape = true;

	Reset();
}

void ShapeParticle::SetColorIdx(ShapeParticle::COLOR Idx)
{
	switch (Idx)
	{
	case RED:
		_ExtraColor = Vector3(0.518f, 0.019f, 0.051f);
		_BrightScale = 0.25f;
		break;
	case GREEN:
		_ExtraColor = Vector3(0.09f, 0.596f, 0.518f);
		_BrightScale = 0.15f;
		break;
	case WHITE:
		_ExtraColor = Vector3(1.f, 1.f, 1.f);
		_BrightScale = 0.15f;
		break;
	}

	Reset();
}

void ShapeParticle::SetCtrlIdx(ShapeParticle::CONTROLPT Idx)
{
	_CtrlPt = Idx;

	Reset();
}

void ShapeParticle::Free()
{
	_BezierCurveDescVec.clear();
	_BezierCurveDescVec.shrink_to_fit();

	GameObject::Free();
}

std::string ShapeParticle::GetName()
{
	return "ShapeParticle";
}

void ShapeParticle::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const Vector3 Scale = _SpTransform->GetScale();
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();

		const auto& _Subset = _ShapeVec[(uint32)_ShapeIdx]->GetSubset(0u);
		const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

		_RenderUpdateInfo.SubsetCullingSphere.resize(1);
		_RenderUpdateInfo.SubsetCullingSphere[0] = _CurBS.Transform(_RenderUpdateInfo.World, Scale.x);
	}
}

void ShapeParticle::Reset()
{
	// dust 모양 4가지중 하나 결정
	uint32 DustSingleIdx = FMath::Random<uint32>(0u, 3u);
	_DustSingleMinTexUV = Vector2(DustSingleIdx * 0.25f, 0.f);
	_DustSingleMaxTexUV = Vector2((DustSingleIdx + 1u) * 0.25f, 0.25f);

	auto WeakSubset = _ShapeVec[(uint32)_ShapeIdx]->GetSubset(0u);
	if (auto SharedSubset = WeakSubset.lock();
		SharedSubset)
	{
		if (_ChangedShape)
		{
			// 정점 수 줄이기
			size_t divide = 1;
			switch (_ShapeIdx)
			{
			case SPHERE:
				divide = 10;
				break;
			case PIPE00:
				divide = 10;
				break;
			case PIPE01:
				divide = 1;
				break;
			}

			_BezierCurveDescVec.clear();
			_BezierCurveDescVec.reserve(SharedSubset->GetVertexBufferDesc().LocalVertexLocation->size() / divide + 1);

			size_t cnt = 0;
			for (auto& StartPos : *SharedSubset->GetVertexBufferDesc().LocalVertexLocation)
			{
				if (0 != cnt++ % divide)
					continue;

				// 메시에 맞는 적절한 EndPos 지정
				Vector3 EndPos = Vector3(0.f, 0.f, 0.f);
				switch (_ShapeIdx)
				{
				case SPHERE:
					EndPos = FMath::Random<Vector3>(Vector3(-100.f, -100.f, -100.f), Vector3(100.f, 100.f, 100.f));
					break;
				case PIPE00:
					EndPos = FMath::Random<Vector3>(Vector3(-100.f, -100.f, -100.f), Vector3(100.f, 100.f, 100.f));
					break;
				case PIPE01:
					EndPos = FMath::Random<Vector3>(Vector3(-100.f, -100.f, -100.f), Vector3(100.f, 100.f, 100.f));
					break;
				}

				_BezierCurveDescVec.push_back({ StartPos, EndPos, Vector3(0.f, 0.f, 0.f) });
			}

			_ChangedShape = false;
		}
		else
		{
			for (auto& Element : _BezierCurveDescVec)
			{
				// 메시에 맞는 적절한 EndPos 지정
				Vector3 EndPos = Vector3(0.f, 0.f, 0.f);
				switch (_ShapeIdx)
				{
				case SPHERE:
					EndPos = FMath::Random<Vector3>(Vector3(-100.f, -100.f, -100.f), Vector3(100.f, 100.f, 100.f));
					break;
				case PIPE00:
					EndPos = FMath::Random<Vector3>(Vector3(-100.f, -100.f, -100.f), Vector3(100.f, 100.f, 100.f));
					break;
				case PIPE01:
					EndPos = FMath::Random<Vector3>(Vector3(-100.f, -100.f, -100.f), Vector3(100.f, 100.f, 100.f));
					break;
				}

				Element.EndPos = EndPos;
			}
		}
	}

	_SliceAmount = 0.f;

	Effect::Reset();
}

void ShapeParticle::Imgui_Modify()
{
	if (auto Sptransform = GetComponent<ENGINE::Transform>().lock();
		Sptransform)
	{
		ImGui::Text("Eff_SphereParticle");

		{
			static Vector3 SliderPosition = Sptransform->GetPosition();
			ImGui::SliderFloat3("Pos##Eff_SphereParticle", SliderPosition, -10.f, 10.f);
			Sptransform->SetPosition(SliderPosition);
		}

		{
			static float Scale = Sptransform->GetScale().x;
			ImGui::SliderFloat("Scale##Eff_SphereParticle", &Scale, 0.001f, 0.01f);
			//Sptransform->SetScale({ Scale, Scale, Scale });	// x만 유효
			SetScale(Scale);
		}

		{
			static Vector3 SliderRotation{ 0,0,0 };
			ImGui::SliderFloat3("Rot##Eff_SphereParticle", SliderRotation, 0.f, 360.f);
			Sptransform->SetRotation(SliderRotation);
		}

		{
			static float PlayingSpeed = _PlayingSpeed;
			ImGui::SliderFloat("PlayingSpeed##Eff_SphereParticle", &PlayingSpeed, 1.f, 100.f);
			_PlayingSpeed = PlayingSpeed;
		}

		if (_Loop)
		{
			static int ShapeIdx = _ShapeIdx;
			ImGui::SliderInt("ShapeIdx##Eff_ShapeParticle", &ShapeIdx, 0, MAX_SHAPE_IDX - 1);
			if(ImGui::Button("Apply##Eff_ShapeParticle##Shape"))
				SetShapeIdx((ShapeParticle::SHAPE)ShapeIdx);

			static int ColorIdx = _ColorIdx;
			ImGui::SliderInt("ColorIdx##Eff_ShapeParticle", &ColorIdx, 0, MAX_COLOR_IDX - 1);
			if (ImGui::Button("Apply##Eff_ShapeParticle##Color"))
				SetColorIdx((ShapeParticle::COLOR)ColorIdx);

			static int CtrlIdx = _CtrlPt;
			ImGui::SliderInt("CtrlIdx##Eff_ShapeParticle", &CtrlIdx, 0, MAX_CONTRLPT_IDX - 1);
			if (ImGui::Button("Apply##Eff_ShapeParticle##Ctrl"))
				SetCtrlIdx((ShapeParticle::CONTROLPT)CtrlIdx);
		}
	}
}

void ShapeParticle::RenderInit()
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

void ShapeParticle::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	auto WeakSubset = _PlaneMesh->GetSubset(0u);
	if (auto SharedSubset = WeakSubset.lock();
		SharedSubset)
	{
		// Single Dust
		_Info.Fx->SetTexture("ALB0Map", _DustSingleTex->GetTexture());
		//_Info.Fx->SetTexture("NoiseMap", _SmokeALB0Tex->GetTexture());
		_Info.Fx->SetBool("_UsingNoise", false);
		_Info.Fx->SetFloat("_SliceAmount", _SliceAmount);
		_Info.Fx->SetFloat("_BrightScale", _BrightScale * (1.f - _SliceAmount));
		//_Info.Fx->SetFloat("SoftParticleDepthScale", _SoftParticleDepthScale);
		_Info.Fx->SetFloatArray("_MinTexUV", _DustSingleMinTexUV, 2u);
		_Info.Fx->SetFloatArray("_MaxTexUV", _DustSingleMaxTexUV, 2u);
		_Info.Fx->SetFloatArray("_ExtraColor", _ExtraColor, 3u);
				
		for (auto& Element : _BezierCurveDescVec)
		{
			Vector3* pPos = reinterpret_cast<Vector3*>(&_DustSingleChildWorldMatrix.m[3][0]);
			D3DXVec3TransformCoord(pPos, &Element.DeltaPos, &_RenderUpdateInfo.World);
			_Info.Fx->SetMatrix("World", &_DustSingleChildWorldMatrix);
			SharedSubset->Render(_Info.Fx);
		}
	}
}

ShapeParticle* ShapeParticle::Create()
{
	return new ShapeParticle{};
}

HRESULT ShapeParticle::Ready()
{
	RenderInit();

	m_nTag = GAMEOBJECTTAG::Eff_ShapeParticle;

	auto InitTransform = GetComponent<ENGINE::Transform>();
	InitTransform.lock()->SetScale({ 0.001f, 0.001f, 0.001f });

	_ShapeVec.reserve(MAX_SHAPE_IDX);
	Mesh::InitializeInfo _Info{};
	_Info.bLocalVertexLocationsStorage = true;
	_ShapeVec.push_back(Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\sphere00.fbx", _Info));
	_ShapeVec.push_back(Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\pipe00.fbx", _Info));
	_ShapeVec.push_back(Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\pipe01.fbx", _Info));

	_PlaneMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx");
	_DustSingleTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\tex_03_dust_single_0003_alpg.tga");

	D3DXMatrixScaling(&_DustSingleChildWorldMatrix, 0.000015f, 0.000015f, 0.000015f);

	//
	SetShapeIdx(_ShapeIdx);
	SetColorIdx(_ColorIdx);

	_PlayingSpeed = 1.f;

	return S_OK;
};

HRESULT ShapeParticle::Awake()
{
	return S_OK;
}

HRESULT ShapeParticle::Start()
{
	return S_OK;
}

UINT ShapeParticle::Update(const float _fDeltaTime)
{
	Effect::Update(_fDeltaTime);

	if (!_IsPlaying)
		return 0;

	float MaxAccTime = 5.f;

	if (MaxAccTime < _AccumulateTime)
		Reset();
	else
	{
		// 밝기, 알파 감소
		_SliceAmount += 0.2f * _fDeltaTime;
		_BrightScale -= 0.003 * _fDeltaTime;

		// 빌보드
		Matrix ViewMat, BillMat, InvRotMat;
		ViewMat = Renderer::GetInstance()->_RenderInfo.View;
		D3DXMatrixIdentity(&BillMat);
		memcpy(&BillMat.m[0][0], &ViewMat.m[0][0], sizeof(Vector3));
		memcpy(&BillMat.m[1][0], &ViewMat.m[1][0], sizeof(Vector3));
		memcpy(&BillMat.m[2][0], &ViewMat.m[2][0], sizeof(Vector3));
		D3DXMatrixInverse(&BillMat, 0, &BillMat);
		D3DXMatrixInverse(&InvRotMat, 0, &GetComponent<Transform>().lock()->GetRotationMatrix());

		D3DXMatrixScaling(&_DustSingleChildWorldMatrix, 0.000015f, 0.000015f, 0.000015f);
		_DustSingleChildWorldMatrix = _DustSingleChildWorldMatrix * BillMat * InvRotMat;
	
		// BezierCurve
		for (auto& p : _BezierCurveDescVec)
		{
			Vector3 ControlPt0 = Vector3(0.f, 0.f, 0.f);	// zero
			if (CONTROLPT::HALF == _CtrlPt)
				ControlPt0 = FMath::Lerp(p.StartPos, p.EndPos, 0.5f);

			p.DeltaPos = FMath::BezierCurve(
				p.StartPos,
				ControlPt0, 
				p.EndPos, _AccumulateTime / MaxAccTime);
		}
	}

	//
	//Imgui_Modify();

	return 0;
}

UINT ShapeParticle::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void ShapeParticle::Editor()
{
	GameObject::Editor();
}

void ShapeParticle::OnEnable()
{
	GameObject::OnEnable();
}

void ShapeParticle::OnDisable()
{
	GameObject::OnDisable();
}