#include "stdafx.h"
#include "..\Header\BreakableObject.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include "StoneDebris.h"

void BreakableObject::Free()
{
	m_pStaticMesh.reset();

	for (auto& Element : m_pStoneDebrisVec)
		Destroy(Element);
	m_pStoneDebrisVec.clear();
	m_pStoneDebrisVec.shrink_to_fit();

	GameObject::Free();
}

std::string BreakableObject::GetName()
{
	return "BreakableObject";
};

BreakableObject* BreakableObject::Create()
{
	return new BreakableObject{};
};

void BreakableObject::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		const Vector3 Scale = _SpTransform->GetScale();
		//_RenderProperty.bRender = true;
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
		if (m_pStaticMesh)
		{
			const uint32  Numsubset = m_pStaticMesh->GetNumSubset();  
			_RenderUpdateInfo.SubsetCullingSphere.resize(Numsubset);

			for (uint32 i = 0; i < Numsubset ;  ++i)
			{
				const auto& _Subset = m_pStaticMesh->GetSubset(i); 
				const auto& _CurBS =_Subset.lock()->GetVertexBufferDesc().BoundingSphere;

				_RenderUpdateInfo.SubsetCullingSphere[i]= _CurBS.Transform(_RenderUpdateInfo.World, Scale.x); 
			}
		}
	}
}

void BreakableObject::RenderInit()
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
		{"gbuffer_ds",
		[this](const DrawInfo& _Info)
			{
				RenderGBuffer(_Info);
			}
		},
	};
	_InitRenderProp.RenderOrders[RenderProperty::Order::Shadow]
		=
	{
		{"Shadow" ,
		[this](const DrawInfo& _Info)
		{
			RenderShadow(_Info);
		}
	} };

	_InitRenderProp.RenderOrders[RenderProperty::Order::Debug]
		=
	{
		{"Debug" ,
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

	RenderInterface::Initialize(_InitRenderProp);
	_InitRenderProp.bShadowCache = true;

	
	
};

void BreakableObject::RenderGBuffer(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset =m_pStaticMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pStaticMesh->GetSubset(i).lock();
			SpSubset)
		{
			if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
			{
				continue; 
			}
			

			SpSubset->BindProperty(TextureType::DIFFUSE, 0, 0, _Info._Device);
			SpSubset->BindProperty(TextureType::NORMALS, 0, 1, _Info._Device);
			SpSubset->Render(_Info.Fx);
		};
	};
}
void BreakableObject::RenderShadow(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = m_pStaticMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pStaticMesh->GetSubset(i).lock();
			SpSubset)
		{
			if (false == _Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
			{
				continue;
			}

			SpSubset->Render(_Info.Fx);
		};
	};
}

void BreakableObject::SetUp(
	const TSTRING _sMesh, 
	const D3DXVECTOR3& _vScale, 
	const D3DXVECTOR3& _vRotation, 
	const D3DXVECTOR3 _vPosition)
{
	Mesh::InitializeInfo _InitInfo{};
	_InitInfo.bLocalVertexLocationsStorage = true;


	m_pStaticMesh = Resources::Load<ENGINE::StaticMesh>(_sMesh, _InitInfo);


	filesystem::path sFileName = _sMesh;
	sFileName = sFileName.filename();

	if (sFileName == "OrbTree_Red1.fbx")
		m_iStoneCount = 4;
	else if (sFileName == "OrbTree_Red4.fbx")
		m_iStoneCount = 12;
	else if (sFileName == "OrbTree_White1.fbx")
		m_iStoneCount = 6;

	D3DXVECTOR3 vScale = _vScale * GScale;
	D3DXVECTOR3 vPos = _vPosition * GScale;
	m_pTransform.lock()->SetScale(vScale);
	m_pTransform.lock()->SetRotation(_vRotation);
	m_pTransform.lock()->SetPosition(vPos);

	PushEditEntity(m_pStaticMesh.get());


	m_pCollider = AddComponent<CapsuleCollider>();
	m_pCollider.lock()->ReadyCollider();
	m_pCollider.lock()->SetRadius(0.07f);
	m_pCollider.lock()->SetHeight(0.07f);
	m_pCollider.lock()->SetCenter({ 0.f, 0.05f, 0.f });
	PushEditEntity(m_pCollider.lock().get());

	RenderInit();
	StoneInit();
}

void BreakableObject::StoneInit()
{
	for (auto& Element : m_pStoneDebrisVec)
		Destroy(Element);
	m_pStoneDebrisVec.clear();

	m_pStoneDebrisVec.reserve(m_iStoneCount);

	if (m_iStoneCount == 6)
	{
		for (uint32 i = 0u; i < m_iStoneCount; ++i)
		{
			weak_ptr<StoneDebris> p = AddGameObject<StoneDebris>();
			p.lock()->SetVariationIdx((StoneDebris::VARIATION)FMath::Random<uint32>((uint32)StoneDebris::GREENORB_0, (uint32)StoneDebris::GREENORB_3));
			p.lock()->SetScale(FMath::Random<float>(0.002f, 0.004f));
			p.lock()->SetRotation(FMath::Random<Vector3>(Vector3(0.f, 0.f, 0.f), Vector3(180.f, 180.f, 180.f)));
			// position은 죽을 때 위치
			p.lock()->SetVelocity(FMath::Random<Vector3>(Vector3(-0.09f, 0.145f, -0.09f), Vector3(0.09f, 0.16f, 0.09f)));
			p.lock()->SetActive(false);
			m_pStoneDebrisVec.push_back(p);
		}
	}
	else
	{
		for (uint32 i = 0u; i < m_iStoneCount; ++i)
		{
			weak_ptr<StoneDebris> p = AddGameObject<StoneDebris>();
			p.lock()->SetVariationIdx((StoneDebris::VARIATION)FMath::Random<uint32>((uint32)StoneDebris::REDORB_0, (uint32)StoneDebris::REDORB_3));
			p.lock()->SetScale(FMath::Random<float>(0.002f, 0.004f));
			p.lock()->SetRotation(FMath::Random<Vector3>(Vector3(0.f, 0.f, 0.f), Vector3(180.f, 180.f, 180.f)));
			// position은 죽을 때 위치
			p.lock()->SetVelocity(FMath::Random<Vector3>(Vector3(-0.09f, 0.145f, -0.09f), Vector3(0.09f, 0.16f, 0.09f)));
			p.lock()->SetActive(false);
			m_pStoneDebrisVec.push_back(p);
		}
	}


	m_bStoneDebrisPlayStart = false;
}

void BreakableObject::StoneDebrisPlayStart()
{
	if (m_bStoneDebrisPlayStart)
		return;

	Vector3 CurPos = m_pTransform.lock()->GetPosition();
	for (auto& Element : m_pStoneDebrisVec)
	{
		Element.lock()->SetPosition(CurPos);
		Element.lock()->SetActive(true);
		Element.lock()->PlayStart();
	}

	m_bStoneDebrisPlayStart = true;
}


void BreakableObject::RenderDebug(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);
	const uint32 Numsubset = m_pStaticMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pStaticMesh->GetSubset(i).lock();
			SpSubset)
		{
			if (false == 
				_Info._Frustum->IsIn(_RenderUpdateInfo.SubsetCullingSphere[i]))
			{
				continue;
			}

			SpSubset->Render(_Info.Fx);
		};
	};
};


HRESULT BreakableObject::Ready()
{
	GameObject::Ready();


	return S_OK;
};

HRESULT BreakableObject::Awake()
{
	GameObject::Awake();
		
	return S_OK;
}

HRESULT BreakableObject::Start()
{
	GameObject::Start();
	return S_OK;
}

UINT BreakableObject::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);

	if (m_bDestroyObject)
		m_fStoneTime += _fDeltaTime;

	if (m_fStoneTime >= 3.f)
	{
		m_fStoneTime = 0.f;
		m_bDestroyObject = false;
		Destroy(m_pGameObject);
	}

	return 0;
}

UINT BreakableObject::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);
	return 0;
}

void BreakableObject::Editor()
{
	GameObject::Editor();
}


void BreakableObject::OnEnable()
{
	GameObject::OnEnable();
}

void BreakableObject::OnDisable()
{
	GameObject::OnDisable();
}

void BreakableObject::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
	switch (_pOther.lock()->m_nTag)
	{
	case GAMEOBJECTTAG::TAG_RedQueen:
	case GAMEOBJECTTAG::Tag_Cbs_Short:
	case GAMEOBJECTTAG::Tag_Cbs_Middle:
	case GAMEOBJECTTAG::Tag_Cbs_Long:
		if (m_bDestroyObject == false)
		{
			_RenderProperty.bRender = false;
			StoneDebrisPlayStart();
			m_bDestroyObject = true;
		}
		break;
	}

}
