#include "stdafx.h"
#include "Gauntlet.h"


Gauntlet::Gauntlet()
{
}

void Gauntlet::Free()
{
    Unit::Free();
}

HRESULT Gauntlet::Ready()
{
	Unit::Ready();
    return S_OK;
}

HRESULT Gauntlet::Awake()
{
	Unit::Awake();
    m_pNero = std::static_pointer_cast<Nero>(FindGameObjectWithTag(Player).lock());
    //부모 뼈 설정 해주고
		//R_Forearm
	m_pParentMat = m_pNero.lock()->Get_BoneMatrixPtr("R_Forearm");

    return S_OK;
}

HRESULT Gauntlet::Start()
{
	Unit::Start();


    return S_OK;
}

UINT Gauntlet::Update(const float _fDeltaTime)
{
	Unit::Update(_fDeltaTime);
	m_pMesh->Update(_fDeltaTime);
    return 0;
}

UINT Gauntlet::LateUpdate(const float _fDeltaTime)
{
	Unit::LateUpdate(_fDeltaTime);

	Matrix								ParentWorldMatrix, FinalWorld, RotX;
	Matrix RotY,Scale;
	ParentWorldMatrix = m_pNero.lock()->Get_NeroWorldMatrix();

	D3DXMatrixRotationX(&RotX, D3DXToRadian(-90));
	D3DXMatrixRotationY(&RotY, D3DXToRadian(180));
	D3DXMatrixScaling(&Scale, 1.1f, 1.1f, 1.1f);
	if (nullptr != m_pParentMat)
	{
		FinalWorld = Scale * *m_pParentMat * ParentWorldMatrix;
		m_pTransform.lock()->SetWorldMatrix(FinalWorld);
	}

    return 0;
}

void Gauntlet::Editor()
{
	Unit::Editor();
}

void Gauntlet::OnEnable()
{
	Unit::OnEnable();
}

void Gauntlet::OnDisable()
{
	Unit::OnDisable();
}

void Gauntlet::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
}

void Gauntlet::OnTriggerExit(std::weak_ptr<GameObject> _pOther)
{
}

void Gauntlet::ChangeAnimation(const std::string& InitAnimName, const bool bLoop, const AnimNotify& _Notify)
{
	m_pMesh->PlayAnimation(InitAnimName, bLoop, _Notify);
}

void Gauntlet::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
	}
}

void Gauntlet::RenderGBufferSK(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = m_pMesh->GetNumSubset();
	if (Numsubset > 0)
	{
		m_pMesh->BindVTF(_Info.Fx);
	};
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->BindProperty(TextureType::DIFFUSE, 0, 0, _Info._Device);
			SpSubset->BindProperty(TextureType::NORMALS, 0, 1, _Info._Device);
			SpSubset->Render(_Info.Fx);
		};
	};
}


void Gauntlet::RenderShadowSK(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = m_pMesh->GetNumSubset();
	if (Numsubset > 0)
	{
		m_pMesh->BindVTF(_Info.Fx);
	};
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
}

void Gauntlet::RenderDebugBone(const DrawInfo& _Info)
{
	const Matrix ScaleOffset = FMath::Scale({ 0.01,0.01 ,0.01 });
	m_pMesh->BoneDebugRender(_RenderUpdateInfo.World, _Info.Fx);
}

void Gauntlet::RenderDebugSK(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);
	const uint32 Numsubset = m_pMesh->GetNumSubset();

	if (Numsubset > 0)
	{
		m_pMesh->BindVTF(_Info.Fx);
	};
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
}

void Gauntlet::RenderInit()
{
}
