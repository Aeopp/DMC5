#include "stdafx.h"
#include "MapAniObject.h"
#include "Subset.h"
#include "SoundSystem.h"
#include "Nero.h"
MapAniObject::MapAniObject()
{
}

void MapAniObject::Free()
{
	m_pMesh.reset();
	GameObject::Free();
}

std::string MapAniObject::GetName()
{
	return "AniObject";
}

MapAniObject* MapAniObject::Create()
{
	MapAniObject* pInstance = new MapAniObject;
	return pInstance;
}

HRESULT MapAniObject::Ready()
{
	GameObject::Ready();
	PushEditEntity(m_pTransform.lock().get());


	return S_OK;
}

HRESULT MapAniObject::Awake()
{
	GameObject::Awake();
	return S_OK;
}

HRESULT MapAniObject::Start()
{
	GameObject::Start();

	if(m_bColl == false)
		m_pMesh->PlayAnimation(0, true, {} , 1.3f);

	if(m_bFlag6 == false)
		m_pTransform.lock()->SetScale({ 0.00002f,0.00002f,0.00002f });

	return S_OK;
}

UINT MapAniObject::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	m_pMesh->Update(_fDeltaTime);
	
	return 0;
}

UINT MapAniObject::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);
	return 0;
}

void MapAniObject::OnEnable()
{
}

void MapAniObject::OnDisable()
{
}

void MapAniObject::Editor()
{
	GameObject::Editor();
	if (bEdit)
	{

	}
}

void MapAniObject::OnCollisionEnter(std::weak_ptr<GameObject> _pOther)
{
	UINT ObjectTag = _pOther.lock()->m_nTag;

	switch (ObjectTag)
	{
	case Monster100:
	case Monster200:
	case Monster0000:
	case Monster1000:
	case Monster5300:
		m_pCollider.lock()->SetActive(false);
		m_pMesh->PlayAnimation(0, false);
		switch (m_iTag)
		{
		case Tag_BrokenBookShelf:
			SoundSystem::GetInstance()->RandSoundKeyPlay("HitTable", { 1,12 }, 0.4f, false);
			SoundSystem::GetInstance()->RandSoundKeyPlay("HitTree", { 1,5 }, 0.4f, false);
			break;
		case Tag_BrokenPeople:
			SoundSystem::GetInstance()->RandSoundKeyPlay("HitPeople", { 1,5 }, 0.8f, false);
			break;
		case Tag_BrokenTable:
			SoundSystem::GetInstance()->RandSoundKeyPlay("HitTable", { 1,12 }, 0.4f, false);
			SoundSystem::GetInstance()->RandSoundKeyPlay("HitTree", { 1,5 }, 0.4f, false);
			break;
		}
	case GAMEOBJECTTAG::Player:
	{
		UINT NeroCurAnimationIndex = static_pointer_cast<Nero>(FindGameObjectWithTag(GAMEOBJECTTAG::Player).lock())->Get_CurAnimationIndex();
		switch (NeroCurAnimationIndex)
		{
		case Nero::ANI_HIT_AIR_AWAY:
		case Nero::ANI_HIT_GROUND_AWAY:
			switch (m_iTag)
			{
			case Tag_BrokenBookShelf:
				SoundSystem::GetInstance()->RandSoundKeyPlay("HitTable", { 1,12 }, 0.4f, false);
				SoundSystem::GetInstance()->RandSoundKeyPlay("HitTree", { 1,5 }, 0.4f, false);
				break;
			case Tag_BrokenPeople:
				SoundSystem::GetInstance()->RandSoundKeyPlay("HitPeople", { 1,5 }, 0.8f, false);
				break;
			case Tag_BrokenTable:
				SoundSystem::GetInstance()->RandSoundKeyPlay("HitTable", { 1,12 }, 0.4f, false);
				SoundSystem::GetInstance()->RandSoundKeyPlay("HitTree", { 1,5 }, 0.4f, false);
				break;
			}
		}
	}
		break;
	}
}

void MapAniObject::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
	if (!(GAMEOBJECTTAG::TAG_RedQueen == _pOther.lock()->m_nTag
		|| GAMEOBJECTTAG::Tag_Cbs_Short == _pOther.lock()->m_nTag
		|| GAMEOBJECTTAG::Tag_Cbs_Middle == _pOther.lock()->m_nTag
		|| GAMEOBJECTTAG::Tag_Cbs_Long == _pOther.lock()->m_nTag
		|| GAMEOBJECTTAG::Overture == _pOther.lock()->m_nTag
		|| GAMEOBJECTTAG::MonsterWeapon == _pOther.lock()->m_nTag
		|| GAMEOBJECTTAG::Eff_NuClear))
		return;
	m_pCollider.lock()->SetActive(false);
	m_pMesh->PlayAnimation(0, false);
	switch (m_iTag)
	{
	case Tag_BrokenBookShelf:
		SoundSystem::GetInstance()->RandSoundKeyPlay("HitTable", { 1,12 }, 0.4f, false);
		SoundSystem::GetInstance()->RandSoundKeyPlay("HitTree", { 1,5 }, 0.4f, false);
		break;
	case Tag_BrokenPeople:
		SoundSystem::GetInstance()->RandSoundKeyPlay("HitPeople", { 1,5 }, 0.8f, false);
		break;
	case Tag_BrokenTable:
		SoundSystem::GetInstance()->RandSoundKeyPlay("HitTable", { 1,12 }, 0.4f, false);
		SoundSystem::GetInstance()->RandSoundKeyPlay("HitTree", { 1,5 }, 0.4f, false);
		break;
	}
}

#pragma region RENDER
void MapAniObject::RenderReady()
{
	if (auto _SpTransform = m_pTransform.lock(); _SpTransform)
	{
		const Vector3 Scale = _SpTransform->GetScale();
		_RenderProperty.bRender = true;
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
		if (m_pMesh)
		{
			const uint32  Numsubset = m_pMesh->GetNumSubset();
			_RenderUpdateInfo.SubsetCullingSphere.resize(Numsubset);

			for (uint32 i = 0; i < Numsubset; ++i)
			{
				const auto& _Subset = m_pMesh->GetSubset(i);
				const auto& _CurBS = _Subset.lock()->GetVertexBufferDesc().BoundingSphere;

				_RenderUpdateInfo.SubsetCullingSphere[i] = _CurBS.Transform(_RenderUpdateInfo.World, Scale.x);
			}
		}
	}
}

void MapAniObject::RenderInit()
{
	// 렌더를 수행해야하는 오브젝트라고 (렌더러에 등록 가능 ) 알림.
	// 렌더 인터페이스 상속받지 않았다면 키지마세요.
	SetRenderEnable(true);

	// 렌더 정보 초기화 ...
	ENGINE::RenderProperty _InitRenderProp;
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	// 렌더 속성 전체 초기화 
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	_InitRenderProp.bRender = true;

	_InitRenderProp.RenderOrders[RenderProperty::Order::GBuffer] =
	{
		{"gbuffer_dsSK",
		[this](const DrawInfo& _Info)
			{
				RenderGBuffer(_Info);
			}
		},
	};
	/*_InitRenderProp.RenderOrders[RenderProperty::Order::Shadow]
		=
	{
		{"ShadowSK" ,
		[this](const DrawInfo& _Info)
		{
			RenderShadow(_Info);
		}
	} };*/
	_InitRenderProp.RenderOrders[RenderProperty::Order::Debug]
		=
	{
		{"DebugSK" ,
		[this](const DrawInfo& _Info)
		{
			RenderDebug(_Info);
		}
	} };
	_InitRenderProp.RenderOrders[RenderProperty::Order::Collider]
		=
	{
		{"Collider" ,
		[this](const DrawInfo& _Info)
		{
			DrawCollider(_Info);
		}
	} };

	/*움직임이 없는 정적 오브젝트는 그림자맵 캐쉬를 켜고 그렇지않다면 꺼주세요. */
	_InitRenderProp.bShadowCache = false;

	RenderInterface::Initialize(_InitRenderProp); 

	m_pMesh->EnableToRootMatricies();
	if (m_bColl == false)
	{
		PushEditEntity(m_pMesh.get());
		m_pMesh->EnablePrevVTF();
	}
	else
	{
		m_pMesh->EnableToRootMatricies();
		m_pMesh->PlayAnimation(0, false);
		m_pMesh->Update(0.0016f);
		m_pMesh->StopAnimation();

		PushEditEntity(m_pMesh.get());
	}
}

void MapAniObject::RenderGBuffer(const DrawInfo& _Info)
{
	DWORD _State{};
	g_pDevice->GetRenderState(D3DRS_CULLMODE, &_State);
	g_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	const Matrix World = _RenderUpdateInfo.World;

	_Info.Fx->SetMatrix("matWorld", &World);

	const uint32 Numsubset = m_pMesh->GetNumSubset();
	if (Numsubset > 0)
	{
		m_pMesh->BindVTF(_Info.Fx);
	}
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
			//if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
			//{
			//	continue;
			//}


			SpSubset->BindProperty(TextureType::DIFFUSE, 0, 0, _Info._Device);
			SpSubset->BindProperty(TextureType::NORMALS, 0, 1, _Info._Device);
			SpSubset->Render(_Info.Fx);
		};
	};
	g_pDevice->SetRenderState(D3DRS_CULLMODE, _State);
}

void MapAniObject::RenderShadow(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = m_pMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
			/*if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
			{
				continue;
			}*/

			SpSubset->Render(_Info.Fx);
		};
	};
}

void MapAniObject::RenderEmissive(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	_Info.Fx->SetFloat("EmissivePower", EmissivePower);
	const uint32 Numsubset = m_pMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
		/*	if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
			{
				continue;
			}*/

			SpSubset->BindProperty(TextureType::DIFFUSE, 0, 0, _Info._Device);
			SpSubset->Render(_Info.Fx);
		};
	};
}

void MapAniObject::RenderDebug(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);
	const uint32 Numsubset = m_pMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
	/*		if (false ==
				_Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
			{
				continue;
			}*/

			SpSubset->Render(_Info.Fx);
		};
	};
}
#pragma endregion

void MapAniObject::SetUp(
	const TSTRING _sMesh,
	const D3DXVECTOR3& _vScale,
	const D3DXVECTOR3& _vRotation,
	const D3DXVECTOR3 _vPosition)
{
	m_pMesh = Resources::Load<ENGINE::SkeletonMesh>(_sMesh);

	{
		filesystem::path sFileName = _sMesh;
		sFileName = sFileName.filename();

		if (sFileName == "electornicbillboard.fbx")
		{
			bEmissive = false;
		};
		if (sFileName == "flag_06.fbx")
		{
			m_pTransform.lock()->SetScale({ 0.008f,0.008f,0.008f });
			m_bFlag6 = true;
			m_iTag = Tag_Flag;
		}
		if (sFileName == "BrokenBookShelf.fbx")
		{
			m_pCollider = AddComponent<BoxCollider>();
			m_pCollider.lock()->ReadyCollider();
			m_pCollider.lock()->SetSize({ 0.3f,0.4f,0.2f });
			m_pCollider.lock()->SetCenter({ 0.f, 0.2f, 0.f });
			PushEditEntity(m_pCollider.lock().get());
			m_bColl = true;
			m_bFlag6 = true;
			m_iTag = Tag_BrokenBookShelf;
		}
		if (sFileName == "BrokenPeople.fbx")
		{
			m_pCollider = AddComponent<BoxCollider>();
			m_pCollider.lock()->ReadyCollider();
			m_pCollider.lock()->SetSize({ 0.1f,0.1f,0.1f });
			PushEditEntity(m_pCollider.lock().get());
			m_bColl = true;
			m_bFlag6 = true;
			m_iTag = Tag_BrokenPeople;
		}
		if (sFileName == "BrokenTable.fbx")
		{
			m_pCollider = AddComponent<BoxCollider>();
			m_pCollider.lock()->ReadyCollider();
			m_pCollider.lock()->SetSize({ 0.35f,0.1f,0.1f });
			m_pCollider.lock()->SetCenter({ 0.f,-0.05f,0.01f });
			PushEditEntity(m_pCollider.lock().get());
			m_bColl = true;
			m_bFlag6 = true;
			m_iTag = Tag_BrokenTable;
		}

	}

	D3DXVECTOR3 vScale = _vScale * GScale;
	D3DXVECTOR3 vPos = _vPosition * GScale;
	m_pTransform.lock()->SetScale(vScale);
	m_pTransform.lock()->SetRotation(_vRotation);
	m_pTransform.lock()->SetPosition(vPos);


	RenderInit();
}

