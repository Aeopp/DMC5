#include "stdafx.h"
#include "Em5300Homing.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "Em5300.h"
#include "Nero.h"
#include "ArtemisMissile.h"
#include "Reverberation.h"

void Em5300Homing::Free()
{
	GameObject::Free();
}

std::string Em5300Homing::GetName()
{
	return "Em5300Homing";
};

Em5300Homing* Em5300Homing::Create()
{
	return new Em5300Homing{};
};

void Em5300Homing::RenderReady()
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

			for (uint32 i = 0; i < Numsubset ;  ++i)
			{
				const auto& _Subset = m_pMesh->GetSubset(i); 
				const auto& _CurBS =_Subset.lock()->GetVertexBufferDesc().BoundingSphere;

				_RenderUpdateInfo.SubsetCullingSphere[i]= _CurBS.Transform(_RenderUpdateInfo.World, Scale.x); 
			}
		}
	}
}

void Em5300Homing::RenderInit()
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
};

void Em5300Homing::RenderGBuffer(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset =m_pMesh->GetNumSubset();
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
void Em5300Homing::RenderShadow(const DrawInfo& _Info)
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

void Em5300Homing::Homing(const float _fDeltaTime)
{
	if (m_bStartHoming == false)
	{
		m_bStartHoming = true;
		m_bHomingDir = true;


		Matrix ssibal = *m_pParentBone * m_ParentWorld;

		Vector3 vPos = { ssibal._41, ssibal._42,ssibal._43 };
		Vector3 vLook2;

		switch (m_iCount)
		{
		case 0:
			vLook2 = { ssibal._11, ssibal._12,ssibal._13 };
			D3DXVec3Normalize(&vLook2, &vLook2);
			m_pRever.lock()->PlayStart(vPos, vLook2, 0.0075f * 0.1f, 0.01f * 0.2f);
			break;
		case 1:
			vLook2 = { ssibal._21, ssibal._22,ssibal._23 };
			D3DXVec3Normalize(&vLook2, &vLook2);
			m_pRever.lock()->PlayStart(vPos, vLook2, 0.0075f * 0.1f, 0.01f * 0.2f);
			break;
		case 2:
			vLook2 = { ssibal._31, ssibal._32,ssibal._33 };
			D3DXVec3Normalize(&vLook2, &vLook2);
			m_pRever.lock()->PlayStart(vPos, vLook2, 0.0075f * 0.1f, 0.01f * 0.2f);
			break;
		case 3:
			if (m_bJustOne == false)
			{
				vLook2 = { -ssibal._31, -ssibal._32,-ssibal._33 };
				D3DXVec3Normalize(&vLook2, &vLook2);
				m_pRever.lock()->PlayStart(vPos, vLook2, 0.0075f * 0.1f, 0.01f * 0.2f);
				m_bJustOne = true;
			}
			break;
		}

	}
	else
	{
		if (m_bHomingDir)
		{
			float fRandomX = FMath::Random<float>(-0.399f, 0.399f);
			float fRandomY = FMath::Random<float>(-0.399f, 0.399f);
			float fRandomZ = FMath::Random<float>(-0.399f, 0.399f);
			m_vHomingDir = { fRandomX, fRandomY, fRandomZ };

			Vector3 vLook = m_pEm5300Trasform.lock()->GetLook();
			m_vHomingDir += -vLook;

			D3DXVec3Normalize(&m_vHomingDir, &m_vHomingDir);

			m_bHomingDir = false;
			m_bJustOne = false;

			m_iCount = (m_iCount + 1) % 4;
		}

		
		m_pTransform.lock()->Translate(m_vHomingDir * 0.002f);
	}

	
}

void Em5300Homing::Hit(BT_INFO _BattleInfo, void* pArg)
{
}

void Em5300Homing::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
}


void Em5300Homing::RenderDebug(const DrawInfo& _Info)
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

};


HRESULT Em5300Homing::Ready()
{
	GameObject::Ready();
	m_nTag = MonsterWeapon;
	m_BattleInfo.iAttack = 10;

	RenderInit();
	// 트랜스폼 초기화 .. 
	m_pTransform = GetComponent<ENGINE::Transform>();
	m_pTransform.lock()->SetPosition(Vector3{/* -12.f,-0.9f,-638.f*/0.f,10.f,0.f });

	PushEditEntity(m_pTransform.lock().get());
	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.



	return S_OK;
};

HRESULT Em5300Homing::Awake()
{
	GameObject::Awake();

	m_pEm5300Trasform = m_pEm5300.lock()->GetComponent<Transform>();
	
	m_pRever = AddGameObject<Reverberation>();

	m_pMissile = AddGameObject<ArtemisMissile>();

	if (m_iHomingPos == 0)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("L_Breast_00_01");
	else if (m_iHomingPos == 1)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("L_Breast_00_02");
	else if (m_iHomingPos == 2)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("L_Breast_00_03");
	else if (m_iHomingPos == 3)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("L_Breast_00_04");
	else if (m_iHomingPos == 4)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("R_Breast_00_01");
	else if (m_iHomingPos == 5)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("R_Breast_00_02");
	else if (m_iHomingPos == 6)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("R_Breast_00_03");
	else if (m_iHomingPos == 7)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("R_Breast_00_04");

	

	m_pCollider = AddComponent<SphereCollider>();
	m_pCollider.lock()->ReadyCollider();
	m_pCollider.lock()->SetRigid(true);
	m_pCollider.lock()->SetTrigger(true);
	m_pCollider.lock()->SetGravity(false);;
	PushEditEntity(m_pCollider.lock().get());


	m_pCollider.lock()->SetRadius(0.015f);
	m_pTransform.lock()->SetScale({0.00015f,0.00015f,0.00015f});

	m_pPlayer = std::static_pointer_cast<Nero>(FindGameObjectWithTag(GAMEOBJECTTAG::Player).lock());
	m_pPlayerTrans = m_pPlayer.lock()->GetComponent<ENGINE::Transform>();

	return S_OK;
}

HRESULT Em5300Homing::Start()
{
	GameObject::Start();
	return S_OK;
}

UINT Em5300Homing::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);

	if (m_bReadyHoming == false)
	{
		m_ParentWorld = m_pEm5300Trasform.lock()->GetWorldMatrix();
		m_Result = (*m_pParentBone * m_ParentWorld);
		m_pTransform.lock()->SetPosition({ m_Result._41, m_Result._42, m_Result._43 });
	}

	if (m_bReadyHoming)
		Homing(_fDeltaTime);
	return 0;
}

UINT Em5300Homing::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	m_pMissile.lock()->GetComponent<Transform>().lock()->SetPosition(m_pTransform.lock()->GetPosition());
	return 0;
}

void Em5300Homing::Editor()
{
	GameObject::Editor();
}


void Em5300Homing::OnEnable()
{
	GameObject::OnEnable();
}

void Em5300Homing::OnDisable()
{
	GameObject::OnDisable();
}
