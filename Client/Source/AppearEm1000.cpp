#include "stdafx.h"
#include "..\Header\AppearEm1000.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"


void AppearEm1000::Free()
{
	GameObject::Free();
}

std::string AppearEm1000::GetName()
{
	return "AppearEm1000";
}

void AppearEm1000::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const Vector3 Scale = _SpTransform->GetScale();
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();

		const auto& _Subset = _DebrisMesh->GetSubset(MAX_SUBSET_IDX);
		const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

		_RenderUpdateInfo.SubsetCullingSphere.resize(1);
		_RenderUpdateInfo.SubsetCullingSphere[0] = _CurBS.Transform(_DebrisChildWorldMatrix * _RenderUpdateInfo.World, 0.0003f * Scale.x);
	}
}

void AppearEm1000::Reset()
{
	_DebrisSubsetIdx = -1.f;
	_DustSpriteIdx = 0.f;
	_SliceAmount = 0.f;
	_DustMinTexUV = Vector2(0.f, 0.f);
	_DustMaxTexUV = Vector2(0.f, 0.f);
	_DustNextSpriteStart = false;

	Effect::Reset();
}

void AppearEm1000::Imgui_Modify()
{
	if (auto Sptransform = GetComponent<ENGINE::Transform>().lock();
		Sptransform)
	{
		ImGui::Text("Eff_AppearEm1000");

		{
			static Vector3 SliderPosition = Sptransform->GetPosition();
			ImGui::SliderFloat3("Pos##AppearEm1000", SliderPosition, -10.f, 10.f);
			Sptransform->SetPosition(SliderPosition);
		}

		{
			static float Scale = Sptransform->GetScale().x;
			ImGui::SliderFloat("Scale##AppearEm1000", &Scale, 0.1f, 1.f);
			//Sptransform->SetScale({ Scale, Scale, Scale });	// x¸¸ À¯È¿
			SetScale(Scale);
		}

		{
			static Vector3 SliderRotation{ 0,0,0 };
			ImGui::SliderFloat3("Rot##AppearEm1000", SliderRotation, 0.f, 360.f);
			Sptransform->SetRotation(SliderRotation);
		}

		{
			static float PlayingSpeed = _PlayingSpeed;
			ImGui::SliderFloat("PlayingSpeed##AppearEm1000", &PlayingSpeed, 0.1f, 10.f);
			_PlayingSpeed = PlayingSpeed;
		}
	}
}

AppearEm1000* AppearEm1000::Create()
{
	return new AppearEm1000{};
}

void AppearEm1000::RenderInit()
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

void AppearEm1000::RenderGBuffer(const DrawInfo& _Info)
{
	if (0.5f > _AccumulateTime)
		return;

	if (MAX_SUBSET_IDX < static_cast<uint32>(_DebrisSubsetIdx))
		return;

	if (!_Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[0]))
		return;

	_Info._Device->SetTexture(0, _DebrisALBMTex->GetTexture());
	_Info._Device->SetTexture(1, _DebrisNRMRTex->GetTexture());
	_Info.Fx->SetFloat("brightScale", _BrightScale * 50.f);

	auto WeakSubset = _DebrisMesh->GetSubset(static_cast<uint32>(_DebrisSubsetIdx));
	if (auto SharedSubset = WeakSubset.lock();
		SharedSubset)
	{
		Matrix World = _DebrisChildWorldMatrix * _RenderUpdateInfo.World;
		_Info.Fx->SetMatrix("matWorld", &World);
		SharedSubset->Render(_Info.Fx);

		D3DXMatrixRotationY(&World, D3DXToRadian(180.f));
		World = _DebrisChildWorldMatrix * World * _RenderUpdateInfo.World;
		_Info.Fx->SetMatrix("matWorld", &World);
		SharedSubset->Render(_Info.Fx);
	}
}

void AppearEm1000::RenderAlphaBlendEffect(const DrawInfo& _Info)
{
	if (!_Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[0]))
		return;

	if (0.5f > _AccumulateTime)
	{
		auto WeakSubset = _PlaneMesh->GetSubset(0u);
		if (auto SharedSubset = WeakSubset.lock();
			SharedSubset)
		{
			Matrix Temp;
			D3DXMatrixScaling(&Temp, 0.5f, 0.5f, 0.5f);
			Temp._42 -= 0.12f;
			const Matrix World = Temp * _DustSpriteChildWorldMatrix * _RenderUpdateInfo.World;
			_Info.Fx->SetMatrix("World", &World);

			_Info.Fx->SetTexture("ALB0Map", _DustMultiTex->GetTexture());
			_Info.Fx->SetBool("_UsingNoise", false);
			_Info.Fx->SetFloat("_SliceAmount", _SliceAmount);
			_Info.Fx->SetFloat("_BrightScale", _BrightScale);
			_Info.Fx->SetFloatArray("_ExtraColor", Vector3(0.349f, 0.302f, 0.27f), 3u);
			_Info.Fx->SetFloatArray("_MinTexUV", _DustMinTexUV, 2u);
			_Info.Fx->SetFloatArray("_MaxTexUV", _DustMaxTexUV, 2u);

			SharedSubset->Render(_Info.Fx);
		}

		return;
	}
	else
	{
		auto WeakSubset = _PlaneMesh->GetSubset(0u);
		if (auto SharedSubset = WeakSubset.lock();
			SharedSubset)
		{
			const Matrix World = _DustSpriteChildWorldMatrix * _RenderUpdateInfo.World;
			_Info.Fx->SetMatrix("World", &World);

			_Info.Fx->SetTexture("ALB0Map", _DustSpurtTex->GetTexture());
			_Info.Fx->SetBool("_UsingNoise", false);
			_Info.Fx->SetFloat("_SliceAmount", _SliceAmount);
			_Info.Fx->SetFloat("_BrightScale", _BrightScale);
			_Info.Fx->SetFloatArray("_ExtraColor", Vector3(1.f, 1.f, 1.f), 3u);
			_Info.Fx->SetFloatArray("_MinTexUV", _DustMinTexUV, 2u);
			_Info.Fx->SetFloatArray("_MaxTexUV", _DustMaxTexUV, 2u);

			SharedSubset->Render(_Info.Fx);
		}

		return;
	}
}


HRESULT AppearEm1000::Ready()
{
	RenderInit();

	m_nTag = GAMEOBJECTTAG::Eff_AppearEm1000;

	_DebrisMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Effect\\Stone\\mesh_capcom_debris_stone00_multi.fbx");
	_PlaneMesh = Resources::Load<ENGINE::StaticMesh>(L"..\\..\\Resource\\Mesh\\Static\\Primitive\\plane00.fbx");
	_DebrisALBMTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\mesh_capcom_debris_stone00_ALBM.tga");
	_DebrisNRMRTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\mesh_capcom_debris_stone00_NRMR.tga");
	_DustMultiTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\tex_capcom_dust_multiple_0009_alpg.tga");
	_DustSpurtTex = Resources::Load<ENGINE::Texture>(L"..\\..\\Resource\\Texture\\Effect\\tex_capcom_impact_directional_0000_alpg.tga");

	D3DXMatrixScaling(&_DebrisChildWorldMatrix, 0.0008f, 0.0008f, 0.0008f);
	Matrix Temp;
	D3DXMatrixRotationX(&Temp, D3DXToRadian(180.f));
	_DebrisChildWorldMatrix *= Temp;
	_DebrisChildWorldMatrix._42 += 0.03f;

	D3DXMatrixScaling(&_DustSpriteChildWorldMatrix, 0.003f, 0.003f, 0.003f);
	_DustSpriteChildWorldMatrix._42 += 0.1f;

	_PlayingSpeed = 1.f;
	_BrightScale = 0.002f;

	Reset();

	return S_OK;
};

HRESULT AppearEm1000::Awake()
{
	return S_OK;
}

HRESULT AppearEm1000::Start()
{
	return S_OK;
}

UINT AppearEm1000::Update(const float _fDeltaTime)
{
	Effect::Update(_fDeltaTime);

	if (!_IsPlaying)
		return 0;

	if (2.f < _AccumulateTime)
		Reset();
	else if (0.5f < _AccumulateTime)
	{
		_DebrisSubsetIdx += _PlayingSpeed * 50.f * _fDeltaTime;
		_SliceAmount += 0.1f * _fDeltaTime;

		// sprite
		if (!_DustNextSpriteStart)
		{
			_DustSpriteIdx = 0.f;
			_DustNextSpriteStart = true;
		}

		_DustSpriteIdx += _PlayingSpeed * 40.f * _fDeltaTime;

		float cx = 8.f;	// °¡·Î °¹¼ö
		float cy = 8.f; // ¼¼·Î °¹¼ö

		if (cx * cy >= _DustSpriteIdx)
		{
			int Frame = static_cast<int>(_DustSpriteIdx);
			int w = Frame % static_cast<int>(cx);
			int h = Frame / static_cast<int>(cx);
			_DustMinTexUV.x = 1.f / cx * static_cast<float>(w);
			_DustMinTexUV.y = 1.f / cy * static_cast<float>(h);
			_DustMaxTexUV.x = _DustMinTexUV.x + 1.f / cx;
			_DustMaxTexUV.y = _DustMinTexUV.y + 1.f / cy;
		}
	}
	else
	{
		// sprite
		_DustSpriteIdx += _PlayingSpeed * 40.f * _fDeltaTime;
	
		float cx = 8.f;	// °¡·Î °¹¼ö
		float cy = 4.f; // ¼¼·Î °¹¼ö

		if (cx * cy < _DustSpriteIdx)
			_DustSpriteIdx = 0.f;

		int Frame = static_cast<int>(_DustSpriteIdx);
		int w = Frame % static_cast<int>(cx);
		int h = Frame / static_cast<int>(cx);
		_DustMinTexUV.x = 1.f / cx * static_cast<float>(w);
		_DustMinTexUV.y = 1.f / cy * static_cast<float>(h);
		_DustMaxTexUV.x = _DustMinTexUV.x + 1.f / cx;
		_DustMaxTexUV.y = _DustMinTexUV.y + 1.f / cy;
	}

	// yÃà ºôº¸µå
	Matrix TempMat, BillMat;
	TempMat = Renderer::GetInstance()->_RenderInfo.View;
	D3DXMatrixIdentity(&BillMat);
	BillMat._11 = TempMat._11;
	BillMat._13 = TempMat._13;
	BillMat._31 = TempMat._31;
	BillMat._33 = TempMat._33;
	D3DXMatrixInverse(&BillMat, 0, &BillMat);
		
	if (auto Sptransform = GetComponent<ENGINE::Transform>().lock();
		Sptransform)
		Sptransform->SetBillBoard(BillMat);

	return 0;
}

UINT AppearEm1000::LateUpdate(const float _fDeltaTime)
{
	return 0;
}

void AppearEm1000::Editor()
{
	GameObject::Editor();

	if (bEdit)
	{
		Imgui_Modify();
	}
}

void AppearEm1000::OnEnable()
{
	GameObject::OnEnable();
}

void AppearEm1000::OnDisable()
{
	GameObject::OnDisable();
}