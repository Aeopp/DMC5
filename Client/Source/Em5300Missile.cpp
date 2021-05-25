#include "stdafx.h"
#include "Em5300Missile.h"
#include "Transform.h"
#include "Subset.h"
#include "TextureType.h"
#include "Renderer.h"
#include <iostream>
#include "Em5300.h"
#include "Nero.h"
#include "ArtemisMissile.h"
#include "Reverberation.h"

void Em5300Missile::Free()
{
	GameObject::Free();
}

std::string Em5300Missile::GetName()
{
	return "Em5300Missile";
};

Em5300Missile* Em5300Missile::Create()
{
	return new Em5300Missile{};
};

void Em5300Missile::RenderReady()
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

void Em5300Missile::RenderInit()
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

void Em5300Missile::RenderGBuffer(const DrawInfo& _Info)
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
void Em5300Missile::RenderShadow(const DrawInfo& _Info)
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

void Em5300Missile::Missile()
{
	if (m_bStartMissile == false)
	{
		Matrix* pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("Tail8");
		m_ParentWorld = m_pEm5300Trasform.lock()->GetWorldMatrix();

		m_Result = (*pParentBone * m_ParentWorld);

		Vector3 vLook = m_pEm5300Trasform.lock()->GetLook();
		Vector3 vParentBone = { m_Result._41, m_Result._42, m_Result._43 };

		Vector3 vDir = vParentBone - m_pTransform.lock()->GetPosition();

		vDir.x += -vLook.x * 0.05f;
		vDir.z += -vLook.z * 0.05f;
		if (m_iMissilePos >= 16)
		{
			vDir.x += -vLook.x * 0.05f;
			vDir.z += -vLook.z * 0.05f;
			vDir.y += 0.2f;
		}

		D3DXVec3Normalize(&vDir, &vDir);

		if(m_iMissilePos <= 15)
			m_pTransform.lock()->Translate(vDir * 0.005f);
		else
			m_pTransform.lock()->Translate(vDir * 0.003f);

		Vector3 vTest = vParentBone - m_pTransform.lock()->GetPosition();
		
		float fTest = D3DXVec3Length(&vTest);
		Vector3 LowPos, HighPos{};

		if (m_iMissilePos <= 15)
		{
			if (fTest <= 0.05f)
			{
				LowPos = m_pTransform.lock()->GetPosition();

				m_bStartMissile = true;
				m_bMissileDir = true;
				m_bRotMissile = true;
			}
		}
		else
		{
			if (fTest <= 0.23f)
			{
				HighPos = m_pTransform.lock()->GetPosition();

				m_bStartMissile = true;
				m_bMissileDir = true;
				m_bRotMissile = true;
			}
		}

		m_vTest = FMath::Lerp(HighPos, LowPos, 0.5f);

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
		if (m_bMissileDir)
		{
			m_vMissileDir = m_pPlayerTrans.lock()->GetPosition() - m_pEm5300Trasform.lock()->GetPosition();
			D3DXVec3Normalize(&m_vMissileDir, &m_vMissileDir);
			m_bMissileDir = false;
			m_bJustOne = false;
			m_iCount = (m_iCount + 1) % 4;
		}

		if (m_bRotMissile)
		{
			switch (m_iMissilePos)
			{
			case 0:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(27.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 1:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(24.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 2:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(21.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 3:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(18.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 4:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(15.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 5:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(12.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 6:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(9.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 7:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(6.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 8:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(-21.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 9:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(-18.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 10:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(-15.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 11:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(-12.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 12:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(-9.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 13:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(-6.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 14:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(-3.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 15:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(0.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 16:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(27.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 17:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(24.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 18:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(21.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 19:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(18.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 20:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(15.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 21:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(12.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 22:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(9.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 23:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(6.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 24:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(-21.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 25:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(-18.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 26:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(-15.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 27:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(-12.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 28:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(-9.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 29:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(-6.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 30:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(-3.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			case 31:
			{
				Matrix	matRot;
				D3DXMatrixRotationY(&matRot, D3DXToRadian(0.f));
				D3DXVec3TransformNormal(&m_vMissileDir, &m_vMissileDir, &matRot);
				break;
			}
			}
			m_bRotMissile = false;
		}



		m_pTransform.lock()->Translate((Vector3( m_vMissileDir.x, 0., m_vMissileDir.z) * 0.04f));
	}

	
}

void Em5300Missile::Missile2()
{
	if (m_iMissilePos == 0 || m_iMissilePos == 1 || m_iMissilePos == 2
		|| m_iMissilePos == 8 || m_iMissilePos == 9 || m_iMissilePos == 10)
		m_bReadyMissile2 = false;


	if (m_bStartMissile2 == false)
	{
		Matrix* pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("Tail8");
		m_ParentWorld = m_pEm5300Trasform.lock()->GetWorldMatrix();

		m_Result = (*pParentBone * m_ParentWorld);

		Vector3 vLook = m_pEm5300Trasform.lock()->GetLook();
		Vector3 vParentBone = { m_Result._41, m_Result._42, m_Result._43 };

		Vector3 vDir = vParentBone - m_pTransform.lock()->GetPosition();

		vDir.x += -vLook.x * 0.05f;
		vDir.z += -vLook.z * 0.05f;
		float fTest2 = D3DXVec3Length(&vDir);


		D3DXVec3Normalize(&vDir, &vDir);
	
		m_pTransform.lock()->Translate(vDir * 0.005f);

		Vector3 vTest = vParentBone - m_pTransform.lock()->GetPosition();

		float fTest = D3DXVec3Length(&vTest);
	
		if (fTest2 <= 0.005f)
		{
			m_bStartMissile2= true;
			m_bMissileDir2 = true;
			m_bRotMissile2 = true;
		}

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
		if (m_bMissileDir2)
		{
			m_vMissileDir = m_pPlayerTrans.lock()->GetPosition() - m_pEm5300Trasform.lock()->GetPosition();
			D3DXVec3Normalize(&m_vMissileDir, &m_vMissileDir);
			m_bMissileDir = false;
			m_bJustOne = false;
			m_iCount = (m_iCount + 1) % 4;
		}
		if (m_bRotMissile2)
		{
			switch (m_iMissilePos)
			{
			case 3:
				m_vMissileDir.y = 0;
				break;
			case 4:
				m_vMissileDir.y = 0.1f;
				break;
			case 5:
				m_vMissileDir.y = 0.2f;
				break;
			case 6:
				m_vMissileDir.y = 0.3f;
				break;
			case 7:
				m_vMissileDir.y = 0.4f;
				break;
			case 11:
				m_vMissileDir.y = 0.5f;
				break;
			case 12:
				m_vMissileDir.y = 0.6f;
				break;
			case 13:
				m_vMissileDir.y = 0.7f;
				break;
			case 14:
				m_vMissileDir.y = 0.8f;
				break;
			case 15:
				m_vMissileDir.y = 0.9f;
				break;
			default:
				return;
			}
			m_bRotMissile2 = false;
		}

		switch (m_iMissilePos)
		{
		case 3:
			m_pTransform.lock()->Translate((Vector3(m_vMissileDir.x, 0.14f, m_vMissileDir.z) * 0.04f));
			break;
		case 4:
			m_pTransform.lock()->Translate((Vector3(m_vMissileDir.x, 0.1f, m_vMissileDir.z) * 0.04f));
			break;
		case 5:
			m_pTransform.lock()->Translate((Vector3(m_vMissileDir.x, 0.06f, m_vMissileDir.z) * 0.04f));
			break;
		case 6:
			m_pTransform.lock()->Translate((Vector3(m_vMissileDir.x, 0.02f, m_vMissileDir.z) * 0.04f));
			break;
		case 7:
			m_pTransform.lock()->Translate((Vector3(m_vMissileDir.x, 0.18f, m_vMissileDir.z) * 0.04f));
			break;
		case 11:
			m_pTransform.lock()->Translate((Vector3(m_vMissileDir.x, 0.16f, m_vMissileDir.z) * 0.04f));
			break;
		case 12:
			m_pTransform.lock()->Translate((Vector3(m_vMissileDir.x, 0.12f, m_vMissileDir.z) * 0.04f));
			break;
		case 13:
			m_pTransform.lock()->Translate((Vector3(m_vMissileDir.x, 0.08f, m_vMissileDir.z) * 0.04f));
			break;
		case 14:
			m_pTransform.lock()->Translate((Vector3(m_vMissileDir.x, 0.04f, m_vMissileDir.z) * 0.04f));
			break;
		case 15:
			m_pTransform.lock()->Translate((Vector3(m_vMissileDir.x, 0.f, m_vMissileDir.z) * 0.04f));
			break;
		default:
			return;
		}


		
	}
}

void Em5300Missile::Hit(BT_INFO _BattleInfo, void* pArg)
{
}

void Em5300Missile::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
}


void Em5300Missile::RenderDebug(const DrawInfo& _Info)
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


HRESULT Em5300Missile::Ready()
{
	GameObject::Ready();
	m_nTag = MonsterWeapon;

	m_BattleInfo.iAttack = 15;
	RenderInit();
	// 트랜스폼 초기화 .. 
	m_pTransform = GetComponent<ENGINE::Transform>();
	m_pTransform.lock()->SetPosition(Vector3{/* -12.f,-0.9f,-638.f*/0.f,10.f,0.f });

	PushEditEntity(m_pTransform.lock().get());
	// 에디터의 도움을 받고싶은 오브젝트들 Raw 포인터로 푸시.



	return S_OK;
};

HRESULT Em5300Missile::Awake()
{
	GameObject::Awake();

	m_pEm5300Trasform = m_pEm5300.lock()->GetComponent<Transform>();
	
	m_pMissile = AddGameObject<ArtemisMissile>();

	if(m_iMissilePos == 0 || m_iMissilePos == 16)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("R_UpperArm_01_03");
	else if (m_iMissilePos == 1 || m_iMissilePos == 17)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("R_Forearm_01_04");
	else if(m_iMissilePos == 2 || m_iMissilePos == 18)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("R_Forearm_01_05");
	else if (m_iMissilePos == 3 || m_iMissilePos == 19)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("R_Forearm_01_06");
	else if (m_iMissilePos == 4 || m_iMissilePos == 20)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("R_Hand_01_07");
	else if (m_iMissilePos == 5 || m_iMissilePos == 21)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("R_Hand_01_08");
	else if (m_iMissilePos == 6 || m_iMissilePos == 22)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("R_Hand_01_09");
	else if (m_iMissilePos == 7 || m_iMissilePos == 23)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("R_Hand_01_010");
	else if (m_iMissilePos == 8 || m_iMissilePos == 24)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("L_UpperArm_01_03");
	else if (m_iMissilePos == 9 || m_iMissilePos == 25)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("L_Forearm_01_04");
	else if (m_iMissilePos == 10 || m_iMissilePos == 26)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("L_Forearm_01_05");
	else if (m_iMissilePos == 11 || m_iMissilePos == 27)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("L_Forearm_01_06");
	else if (m_iMissilePos == 12 || m_iMissilePos == 28)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("L_Hand_01_07");
	else if (m_iMissilePos == 13 || m_iMissilePos == 29)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("L_Hand_01_08");
	else if (m_iMissilePos == 14 || m_iMissilePos == 30)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("L_Hand_01_09");
	else if (m_iMissilePos == 15 || m_iMissilePos == 31)
		m_pParentBone = m_pEm5300Mesh.lock()->GetToRootMatrixPtr("L_Hand_01_010");

	m_pCollider = AddComponent<SphereCollider>();
	m_pCollider.lock()->ReadyCollider();
	m_pCollider.lock()->SetTrigger(true);
	m_pCollider.lock()->SetGravity(false);;
	PushEditEntity(m_pCollider.lock().get());


	m_pCollider.lock()->SetRadius(0.015f);
	m_pTransform.lock()->SetScale({0.00015f,0.00015f,0.00015f});

	m_pPlayer = std::static_pointer_cast<Nero>(FindGameObjectWithTag(GAMEOBJECTTAG::Player).lock());
	m_pPlayerTrans = m_pPlayer.lock()->GetComponent<ENGINE::Transform>();



	m_pRever = AddGameObject<Reverberation>();

	return S_OK;
}

HRESULT Em5300Missile::Start()
{
	GameObject::Start();
	return S_OK;
}

UINT Em5300Missile::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
		

	if (m_bReadyMissile == false && m_bReadyMissile2 == false)
	{
		m_ParentWorld = m_pEm5300Trasform.lock()->GetWorldMatrix();
		m_Result = (*m_pParentBone * m_ParentWorld);
		m_pTransform.lock()->SetPosition({ m_Result._41, m_Result._42, m_Result._43 });
	}

	if (m_bReadyMissile)
		Missile();
	if (m_bReadyMissile2)
		Missile2();
	return 0;
}

UINT Em5300Missile::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);

	m_pMissile.lock()->GetComponent<Transform>().lock()->SetPosition(m_pTransform.lock()->GetPosition());
	return 0;
}

void Em5300Missile::Editor()
{
	GameObject::Editor();
}


void Em5300Missile::OnEnable()
{
	GameObject::OnEnable();
}

void Em5300Missile::OnDisable()
{
	GameObject::OnDisable();
}
