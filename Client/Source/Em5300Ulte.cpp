#include "stdafx.h"
#include "Em5300.h"
#include "Em5300Ulte.h"
#include "Transform.h"
#include "Subset.h"
#include "Renderer.h"
#include "Nero.h"
#include "TextureType.h"
#include "NuClear.h"

void Em5300Ulte::Free()
{
	GameObject::Free();
}

Em5300Ulte* Em5300Ulte::Create()
{
	return new Em5300Ulte;
}

std::string Em5300Ulte::GetName()
{
	return "Em5300Ulte";
}


HRESULT Em5300Ulte::Ready()
{
	Unit::Ready();
	RenderInit();

	return S_OK;
}

HRESULT Em5300Ulte::Awake()
{
	Unit::Awake();
	m_nTag = MonsterWeapon;
	m_pEm5300Trans = m_pEm5300.lock()->GetComponent<ENGINE::Transform>();
	m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("Head");

	m_pCollider = AddComponent<SphereCollider>();
	m_pCollider.lock()->ReadyCollider();
	PushEditEntity(m_pCollider.lock().get());

	m_pCollider.lock()->SetTrigger(true);

	m_pCollider.lock()->SetCenter({ 0.f, 0.f, 0.f });
	m_pCollider.lock()->SetRadius(0.2f);

	m_pTransform.lock()->SetScale({ 0.00015f, 0.00015f, 0.00015f });


	m_pNuclear = AddGameObject<NuClear>();
	return S_OK;
}

HRESULT Em5300Ulte::Start()
{
	return S_OK;
}

UINT Em5300Ulte::Update(const float _fDeltaTime)
{
	Unit::Update(_fDeltaTime);

	if (m_bUlteStart == false)
	{
		m_ParentWorld = m_pEm5300Trans.lock()->GetWorldMatrix();
		m_Result = *m_pParentBone * m_ParentWorld;

		Vector3 vResult = { m_Result._41, m_Result._42, m_Result._43 };
		Vector3 vLook = m_pEm5300Trans.lock()->GetLook();
		
		vResult.x += -vLook.x * 0.5f;
		vResult.z += -vLook.z * 0.5f;
		

		m_pTransform.lock()->SetPosition(vResult);
	}


	if (m_bUlteStart)
		Ulte(_fDeltaTime);
	
	return 0;
}

UINT Em5300Ulte::LateUpdate(const float _fDeltaTime)
{
	Unit::LateUpdate(_fDeltaTime);

	return 0;
}

void Em5300Ulte::Editor()
{
	Unit::Editor();
	if (false == bEdit)
		return;
}

void Em5300Ulte::OnEnable()
{
	Unit::OnDisable();
}

void Em5300Ulte::OnDisable()
{
	Unit::OnDisable();
}

void Em5300Ulte::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
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

void Em5300Ulte::Hit(BT_INFO _BattleInfo, void* pArg)
{
}

void Em5300Ulte::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
}



void Em5300Ulte::RenderInit()
{
	m_nTag = MonsterWeapon;
	// 렌더를 수행해야하는 오브젝트라고 (렌더러에 등록 가능 ) 알림.
	// 렌더 인터페이스 상속받지 않았다면 키지마세요.
	SetRenderEnable(true);

	// 렌더 정보 초기화 ...
	ENGINE::RenderProperty _InitRenderProp;
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	// 렌더 속성 전체 초기화 
	// 이값을 런타임에 바꾸면 렌더를 켜고 끌수 있음. 
	_InitRenderProp.bRender = true;
	//_InitRenderProp.RenderOrders[RenderProperty::Order::GBuffer] =
	//{
	//	{"gbuffer_ds",
	//	[this](const DrawInfo& _Info)
	//		{
	//			RenderGBuffer(_Info);
	//		}
	//	},
	//};
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

	/// <summary>
	/// DrawCollider
	/// </summary>
	_InitRenderProp.RenderOrders[RenderProperty::Order::Collider]
		=
	{
		{"Debug" ,
		[this](const DrawInfo& _Info)
		{
			DrawCollider(_Info);
		}
	} };
	RenderInterface::Initialize(_InitRenderProp);

	// 
	// 스태틱 메쉬 로딩

	m_pMesh = Resources::Load<ENGINE::StaticMesh>(
		L"..\\..\\Resource\\Mesh\\Static\\Sphere.fbx");
	PushEditEntity(m_pMesh.get());
}

void Em5300Ulte::RenderDebug(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);
	const uint32 Numsubset = m_pMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
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
}

void Em5300Ulte::RenderGBuffer(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = m_pMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
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

void Em5300Ulte::RenderShadow(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = m_pMesh->GetNumSubset();
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
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

void Em5300Ulte::Ulte(const float _fDeltaTime)
{
	if (m_pEm5300.lock()->Get_State() == Em5300::Attack_Ulte_Loop)
	{
		m_fUlteSize += _fDeltaTime * 0.00008f;
		m_pNuclear.lock()->PlayStart(m_pTransform.lock()->GetPosition(),-1.f);
	}

	m_pTransform.lock()->SetScale({ m_fUlteSize, m_fUlteSize, m_fUlteSize });

	if (m_pEm5300.lock()->Get_State() == Em5300::Attack_Ulte_Loop2 && m_pEm5300Mesh.lock()->IsAnimationEnd())
		m_bUlte = true;
	if(m_bUlte)
		m_pTransform.lock()->Translate({ 0.f, -0.01f, 0.f });

	if (m_pTransform.lock()->GetPosition().y <= -1.f)
	{
		m_bUlte = false;
		m_bUlteEnd = true;
	}

	if (m_pNuclear.lock()->IsBlackOut() && m_bOne[0] == false)
	{
		SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Ulte3", { 1,1 }, 3.f, false);
		m_bOne[0] = true;
	}
	if (m_pNuclear.lock()->IsKaboom() && m_bOne[1] == false)
	{
		SoundSystem::GetInstance()->RandSoundKeyPlay("Em5300Ulte4", { 1,1 }, 1.f, false);
		m_bOne[1] = true;
	}
}


