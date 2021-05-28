#include "stdafx.h"
#include "NeroCoat.h"
#include "Nero.h"

NeroCoat::NeroCoat()
{
}

void NeroCoat::Free()
{
	GameObject::Free();
}

NeroCoat* NeroCoat::Create()
{
	return new NeroCoat();
}

HRESULT NeroCoat::Ready()
{
	GameObject::Ready();
	return S_OK;
}

HRESULT NeroCoat::Awake()
{
	GameObject::Awake();
	m_pNero = FindGameObjectWithType<Nero>();
	m_pNeroTransform = m_pNero.lock()->GetComponent<Transform>();

	CreateCloth();
	return S_OK;
}

HRESULT NeroCoat::Start()
{
	GameObject::Start();
	return S_OK;
}

UINT NeroCoat::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	Node* pNode = m_pMesh->GetNode("Chest");

	D3DXVECTOR3 vScale, vPos;
	D3DXQUATERNION tQuat;
	D3DXMatrixDecompose(&vScale, &tQuat, &vPos, &pNode->ToRoot);

	PxVec3 targetPos;
	memcpy_s(&targetPos, sizeof(PxVec3), m_pNeroTransform.lock()->GetPosition(), sizeof(PxVec3));
	PxQuat targetQuat;
	memcpy_s(&targetQuat, sizeof(PxQuat), &tQuat, sizeof(PxQuat));


	if (m_pNeroTransform.lock()->IsSetPosition())
		m_pCloth->setGlobalPose(PxTransform(targetPos, targetQuat));
	else
		m_pCloth->setTargetPose(PxTransform(targetPos, targetQuat));


	//PxClothCollisionSphere 위치 정보 업데이트
	/*PxClothCollisionSphere* pSpheres = new PxClothCollisionSphere[m_vecNode.size()];

	m_pCloth->getCollisionData(pSpheres, nullptr, nullptr, nullptr, nullptr);

	for (UINT i = 0; i < m_vecNode.size(); ++i)
	{
		PxVec3 vPos(0.f, 0.f, 0.f);

		memcpy_s(&vPos, sizeof(PxVec3), m_vecNode[i]->ToRoot.m[3], sizeof(PxVec3));
		pSpheres[i].pos = vPos;
	}

	m_pCloth->setCollisionSpheres(pSpheres, m_vecNode.size());

	delete[] pSpheres;*/

	//ClothParticle 시뮬레이션 결과를 Node에 적용

	PxClothParticleData* pParticleData = m_pCloth->lockParticleData();

	PxClothParticle* pParticles = pParticleData->particles;

	vector<PxClothParticle> vecParticles;

	for (UINT i = 0; i < m_vecClothBone.size(); ++i)
	{
		if (nullptr == m_vecClothBone[i]->pParent)
		{
			continue;
		}

		D3DXMATRIX matParent = m_vecClothBone[i]->pParent->matToRoot;
		D3DXMatrixInverse(&matParent, nullptr, &matParent);

		memcpy_s(m_vecClothBone[i]->matToRoot.m[3], sizeof(PxVec3), &pParticles[i].pos, sizeof(PxVec3));

		D3DXMATRIX newTransform = m_vecClothBone[i]->matToRoot * matParent;
		
		m_vecClothBone[i]->pNode->ClothTransform = newTransform;

	}

	//PxVec3 vRootDelta;
	//PxVec3 vPrevPos;
	//PxVec3 vCurrPos;
	//for (UINT i = 0; i < m_vecClothBone.size(); ++i)
	//{
	//	if (nullptr == m_vecClothBone[i]->pParent)
	//	{
	//		memcpy_s(&vPrevPos, sizeof(PxVec3), &m_vecClothBone[i]->matToRoot.m[3], sizeof(PxVec3));
	//		memcpy_s(&vCurrPos, sizeof(PxVec3), &m_vecClothBone[i]->pNode->ToRoot.m[3], sizeof(PxVec3));

	//		vRootDelta = vCurrPos - vPrevPos;

	//		pParticles[i].pos += vRootDelta;

	//		m_vecClothBone[i]->matToRoot = m_vecClothBone[i]->pNode->ToRoot;
	//		continue;
	//	}

	//	//D3DXMATRIX matToRoot = m_vecClothBone[i]->pNode->ClothTransform.value() * m_vecClothBone[i]->pNode->Parent->ToRoot;
	//	//D3DXMATRIX matToRoot = m_vecClothBone[i]->pNode->ToRoot;
	//	pParticles[i].pos += vRootDelta;

	//	m_vecClothBone[i]->matToRoot = m_vecClothBone[i]->pNode->ToRoot;
	//}

	pParticleData->unlock();



	//
	/********************************************************************************************/
	//Node* pNode = m_pMesh->GetNode("Chest");

	//D3DXVECTOR3 vScale, vPos;
	//D3DXQUATERNION tQuat;
	//D3DXMatrixDecompose(&vScale, &tQuat, &vPos, &pNode->ToRoot);

	//PxVec3 targetPos;
	//memcpy_s(&targetPos, sizeof(PxVec3), &vPos, sizeof(PxVec3));
	//PxQuat targetQuat;
	//memcpy_s(&targetQuat, sizeof(PxQuat), &tQuat, sizeof(PxQuat));

	//m_pCloth->setTargetPose(PxTransform(m_pCloth->getGlobalPose().p, targetQuat));

	//D3DXMATRIX matInv;

	////matInv = pNode->ToRoot;
	//D3DXMatrixRotationQuaternion(&matInv, &tQuat);

	//D3DXMatrixInverse(&matInv, nullptr, &matInv);

	////PxClothCollisionSphere 위치 정보 업데이트
	//PxClothCollisionSphere* pSpheres = new PxClothCollisionSphere[m_vecNode.size()];

	//m_pCloth->getCollisionData(pSpheres, nullptr, nullptr, nullptr, nullptr);

	//for (UINT i = 0; i < m_vecNode.size(); ++i)
	//{
	//	PxVec3 vPos(0.f, 0.f, 0.f);

	//	D3DXMATRIX matNew = matInv * m_vecNode[i]->ToRoot;
	//	memcpy_s(&vPos, sizeof(PxVec3), matNew.m[3], sizeof(PxVec3));
	//	pSpheres[i].pos = vPos;
	//}

	//m_pCloth->setCollisionSpheres(pSpheres, m_vecNode.size());

	//delete[] pSpheres;

	////ClothParticle 시뮬레이션 결과를 Node에 적용
	//PxU32 nNumParticles = m_pCloth->getNbParticles();

	//PxClothParticleData* pParticleData = m_pCloth->lockParticleData();

	//PxClothParticle* pParticles = pParticleData->particles;

	////TestCode
	//D3DXMATRIX matRot;
	//D3DXMatrixRotationQuaternion(&matRot, &tQuat);

	//for (UINT i = 0; i < m_vecClothBone.size(); ++i)
	//{
	//	if (nullptr == m_vecClothBone[i]->pParent)
	//		continue;

	//	D3DXMATRIX matParent = m_vecClothBone[i]->pParent->matToRoot;
	//	D3DXMatrixInverse(&matParent, nullptr, &matParent);

	//	PxVec3 vParticlePos = pParticles[m_vecClothBone[i]->nParticleIndex].pos;

	//	memcpy_s(m_vecClothBone[i]->matToRoot.m[3], sizeof(PxVec3), &vParticlePos, sizeof(PxVec3));

	//	D3DXMATRIX newTransform = matRot * m_vecClothBone[i]->matToRoot * matParent;

	//	m_vecClothBone[i]->pNode->ClothTransform = newTransform;
	//}
	//pParticleData->unlock();
	return 0;
}

UINT NeroCoat::LateUpdate(const float _fDeltaTime)
{
	GameObject::LateUpdate(_fDeltaTime);
	return 0;
}

void NeroCoat::OnEnable()
{
}

void NeroCoat::OnDisable()
{
}

void NeroCoat::Editor()
{
}

void NeroCoat::SetMesh(shared_ptr<SkeletonMesh> _pMesh)
{
	m_pMesh = _pMesh;
}

void NeroCoat::CreateCloth()
{
	if (nullptr == m_pMesh)
		return;
	LPCLOTHBONE pCurr = nullptr;
	LPCLOTHBONE pNext = nullptr;

	int nCoatClothIdx = 0;
	int nRowIdx = 0;
	int nColumnIdx = 0;

	Node* pNode = m_pMesh->GetNode("CoatCloth_00_03");

	vector<PxVec4> vecClothParticles;

	while (true)
	{
		if (0 == pNode->Childrens.size())
		{
			++nCoatClothIdx;

			//7번재 시작 노드가 마지막 노드.
			if (8 == nCoatClothIdx)
				break;

			//다음 CoatCloth 시작 노드 이름 설정
			string sNextCoatClothNode = "CoatCloth_0";

			char szBuf[16] = "";
			_itoa_s(nCoatClothIdx, szBuf, 10);
			sNextCoatClothNode += szBuf;

			sNextCoatClothNode += (7 == nCoatClothIdx ? "_03" : "_00");

			//설정된 이름의 시작 노드
			pNode = m_pMesh->GetNode(sNextCoatClothNode);

			//새로 설정된 시작노드 탐색을 위해 변수 초기화
			pCurr = nullptr;
			pNext = nullptr;

			++nColumnIdx;
			nRowIdx = 0;

			continue;
		}
		pNext = new CLOTHBONE;

		pNext->sName = pNode->Name;
		pNext->matToRoot = pNode->ToRoot;
		pNext->pNode = pNode;
		pNext->pParent = pCurr;
		//pNext->nParticleIndex = (nRowIdx * 5) + (nColumnIdx * 105);


		m_vecClothBone.push_back(pNext);

		pNode = pNode->Childrens[0];
		pCurr = pNext;

		PxVec4 vPos(0.f, 0.f, 0.f, 0.f);
		memcpy_s(&vPos, sizeof(PxVec3), pCurr->matToRoot.m[3], sizeof(PxVec3));

		if (nRowIdx < 3)
			vPos.w = 0.f;
		else
			vPos.w = 1.f;

		if (nRowIdx == 1 && (nColumnIdx == 0 || nColumnIdx == 7))
			vPos.w = 1.f;


		if (nRowIdx == 2 && (nColumnIdx == 0 || nColumnIdx == 7 || nColumnIdx == 1 || nColumnIdx == 6))
			vPos.w = 1.f;

		vecClothParticles.push_back(vPos);
		++nRowIdx;

	}


	//인덱스 생성
	vector<PxU32> vecIndex;

	for (UINT nColumn = 0; nColumn < 8 - 1; ++nColumn)
	{
		for (UINT nRow = 0; nRow < 5 - 1; ++nRow)
		{
			vecIndex.push_back(nRow + 5 * nColumn);
			vecIndex.push_back(nRow + 1 + 5 * nColumn);
			vecIndex.push_back(nRow + 1 + 5 * (nColumn + 1));

			vecIndex.push_back(nRow + 5 * nColumn);
			vecIndex.push_back(nRow + 1 + 5 * (nColumn + 1));
			vecIndex.push_back(nRow + 5 * (nColumn + 1));
		}
	}

	PxClothMeshDesc meshDesc;

	meshDesc.setToDefault();

	meshDesc.points.data = vecClothParticles.data();
	meshDesc.points.count = vecClothParticles.size();
	meshDesc.points.stride = sizeof(PxVec4);

	meshDesc.invMasses.data = &vecClothParticles.data()->w;
	meshDesc.invMasses.count = vecClothParticles.size();
	meshDesc.invMasses.stride = sizeof(PxVec4);

	meshDesc.triangles.data = vecIndex.data();
	meshDesc.triangles.count = vecIndex.size() / 3;
	meshDesc.triangles.stride = sizeof(PxU32) * 3;

	//for (UINT i = 0; i < meshDesc.invMasses.count; ++i)
	//	((float*)meshDesc.invMasses.data)[i * 4] = 0.5f;

	//for (UINT nColumn = 0; nColumn < 36; ++nColumn)
	//{
	//	for (UINT nRow = 0; nRow < 21; ++nRow)
	//	{
	//		int nIndex = nRow + 21 * nColumn;
	//		if (nRow < 10)
	//		{
	//			vecClothParticles[nIndex].w = 0.f;
	//		}
	//	}
	//}

	PxClothMeshQuadifier quadifier(meshDesc);

	PxClothFabric* pClothFabric = PxClothFabricCreate(*Physics::GetPxPhysics(), meshDesc, PxVec3(0, -1, 0));

	PxClothParticle* pParticles = (PxClothParticle*)meshDesc.points.data;

	PxTransform transform(PxVec3(0.f, 0.f, 0.f), PxQuat(0.f, 0.f, 0.f, 1.f));

	m_pCloth = Physics::GetPxPhysics()->createCloth(transform, *pClothFabric, pParticles, PxClothFlags());

	Physics::AddActor(GetSceneID(), *m_pCloth);

	//m_vecNode.push_back(m_pMesh->GetNode("Waist"));
	//m_vecNode.push_back(m_pMesh->GetNode("Stomach"));
	//m_vecNode.push_back(m_pMesh->GetNode("Chest"));
	//m_vecNode.push_back(m_pMesh->GetNode("L_Thigh"));
	//m_vecNode.push_back(m_pMesh->GetNode("L_Shin"));
	//m_vecNode.push_back(m_pMesh->GetNode("R_Thigh"));
	//m_vecNode.push_back(m_pMesh->GetNode("R_Shin"));


	////2.
	//vector<PxClothCollisionSphere> vecCollisionSphere;
	//vecCollisionSphere.resize(m_vecNode.size());
	//for (UINT i = 0; i < m_vecNode.size(); ++i)
	//{
	//	PxVec3 vPosition(0.f, 0.f, 0.f);
	//	memcpy_s(&vPosition, sizeof(PxVec3), m_vecNode[i]->ToRoot.m[3], sizeof(PxVec3));
	//	vecCollisionSphere[i].pos = vPosition;

	//	vecCollisionSphere[i].radius = i < 3 ? 10.f : 10.f;
	//}

	//m_pCloth->setCollisionSpheres(vecCollisionSphere.data(), vecCollisionSphere.size());
	//m_pCloth->addCollisionCapsule(0, 1);
	//m_pCloth->addCollisionCapsule(1, 2);
	//m_pCloth->addCollisionCapsule(3, 4);
	//m_pCloth->addCollisionCapsule(5, 6);


	m_pCloth->setSolverFrequency(240);

	m_pCloth->setStiffnessFrequency(10.0f);

	// damp global particle velocity to 90% every 0.1 seconds
	m_pCloth->setDampingCoefficient(PxVec3(0.2f)); // damp local particle velocity
	m_pCloth->setLinearDragCoefficient(PxVec3(0.2f)); // transfer frame velocity
	m_pCloth->setAngularDragCoefficient(PxVec3(0.2f)); // transfer frame rotation

	// reduce impact of frame acceleration
	// x, z: cloth swings out less when walking in a circle
	// y: cloth responds less to jump acceleration
	m_pCloth->setLinearInertiaScale(PxVec3(0.8f, 0.6f, 0.8f));

	// leave impact of frame torque at default
	m_pCloth->setAngularInertiaScale(PxVec3(1.0f));

	// reduce centrifugal force of rotating frame
	m_pCloth->setCentrifugalInertiaScale(PxVec3(0.3f));


	m_pCloth->setClothFlag(PxClothFlag::eSWEPT_CONTACT, true);


	PxClothStretchConfig stretchConfig;
	stretchConfig.stiffness = 1.0f;

	m_pCloth->setStretchConfig(PxClothFabricPhaseType::eVERTICAL, PxClothStretchConfig(1.0f));
	m_pCloth->setStretchConfig(PxClothFabricPhaseType::eVERTICAL, PxClothStretchConfig(1.0f));
	m_pCloth->setStretchConfig(PxClothFabricPhaseType::eSHEARING, PxClothStretchConfig(0.75f));
	m_pCloth->setStretchConfig(PxClothFabricPhaseType::eBENDING, PxClothStretchConfig(0.5f));
	m_pCloth->setTetherConfig(PxClothTetherConfig(1.0f));

	/************************************************************************************/

	//LPCLOTHBONE pCurr = nullptr;
	//LPCLOTHBONE pNext = nullptr;

	//int nCoatClothIdx	= 0;
	//int nRowIdx			= 0;
	//int nColumnIdx		= 0;

	//PxVec4 vClothParticles[21][36];

	//Node* pNode = m_pMesh->GetNode("CoatCloth_00_03");

	//while (true)
	//{
	//	//CoatCloth의 마지막 노드인 경우 컨테이너에 추가 X / 다음 시작 노드 설정
	//	if (0 == pNode->Childrens.size())
	//	{
	//		++nCoatClothIdx;

	//		//7번재 시작 노드가 마지막 노드.
	//		if (8 == nCoatClothIdx)
	//			break;

	//		//다음 CoatCloth 시작 노드 이름 설정
	//		string sNextCoatClothNode = "CoatCloth_0";

	//		char szBuf[16] = "";
	//		_itoa_s(nCoatClothIdx, szBuf, 10);
	//		sNextCoatClothNode += szBuf;

	//		sNextCoatClothNode += (7 == nCoatClothIdx ? "_03" : "_00");

	//		//설정된 이름의 시작 노드
	//		pNode = m_pMesh->GetNode(sNextCoatClothNode);

	//		//새로 설정된 시작노드 탐색을 위해 변수 초기화
	//		pCurr = nullptr;
	//		pNext = nullptr;

	//		++nColumnIdx;
	//		nRowIdx = 0;

	//		continue;
	//	}

	//	pNext = new CLOTHBONE;

	//	pNext->sName		= pNode->Name;
	//	pNext->matToRoot = pNode->ToRoot;
	//	pNext->pNode = pNode;
	//	pNext->pParent = pCurr;
	//	pNext->nParticleIndex = (nRowIdx * 5) + (nColumnIdx * 105);


	//	m_vecClothBone.push_back(pNext);

	//	pNode = pNode->Childrens[0];
	//	pCurr = pNext;
	//	++nRowIdx;

	//	PxVec3 vPos(0.f, 0.f, 0.f);
	//	memcpy_s(&vPos, sizeof(PxVec3), pCurr->matToRoot.m[3], sizeof(PxVec3));
	//	vClothParticles[nRowIdx * 5][nColumnIdx * 5] = PxVec4(vPos, (nRowIdx == 0 ? 0.f : 1.f));

	//}

	////값 선형 보간
	//for (UINT nRow = 0; nRow < 21; nRow += 5)
	//{
	//	for (UINT nColumn = 0; nColumn != 35; nColumn += 5)
	//	{
	//		PxVec4 vStart = vClothParticles[nRow][nColumn];
	//		PxVec4 vEnd = vClothParticles[nRow][nColumn + 5];

	//		for (UINT i = 1; i < 5; ++i)
	//			vClothParticles[nRow][nColumn + i] = vStart * (1.f - 0.2f * i) + vEnd * 0.2f * i;
	//	}
	//}

	//for (UINT nRow = 0; nRow != 20; nRow += 5)
	//{
	//	for (UINT nColumn = 0; nColumn < 36; ++nColumn)
	//	{
	//		PxVec4 vStart = vClothParticles[nRow][nColumn];
	//		PxVec4 vEnd = vClothParticles[nRow + 5][nColumn];

	//		for (UINT i = 1; i < 5; ++i)
	//			vClothParticles[nRow + i][nColumn] = vStart * (1.f - 0.2f * i) + vEnd * 0.2f * i;
	//	}
	//}

	////인덱스 생성
	//vector<PxU32> vecIndex;
	//vecIndex.reserve(4200);

	//for (UINT nColumn = 0; nColumn < 35; ++nColumn)
	//{
	//	for (UINT nRow = 0; nRow < 20; ++nRow)
	//	{
	//		vecIndex.push_back(nRow + 21 * nColumn);
	//		vecIndex.push_back(nRow + 1 + 21 * nColumn);
	//		vecIndex.push_back(nRow + 1 + 21 * (nColumn + 1));

	//		vecIndex.push_back(nRow + 21 * nColumn);
	//		vecIndex.push_back(nRow + 1 + 21 * (nColumn + 1));
	//		vecIndex.push_back(nRow + 21 * (nColumn + 1));
	//	}
	//}

	////버텍스 생성
	//vector<PxVec4> vecVertex;

	//for (UINT nColumn = 0; nColumn < 36; ++nColumn)
	//{
	//	for (UINT nRow = 0; nRow < 21; ++nRow)
	//	{
	//		vecVertex.push_back(vClothParticles[nRow][nColumn]);
	//	}
	//}

	////
	//PxClothMeshDesc meshDesc;

	//meshDesc.setToDefault();

	//meshDesc.points.data = vecVertex.data();
	//meshDesc.points.count = vecVertex.size();
	//meshDesc.points.stride = sizeof(PxVec4);

	//meshDesc.invMasses.data = &vecVertex.data()->w;
	//meshDesc.invMasses.count = vecVertex.size();
	//meshDesc.invMasses.stride = sizeof(PxVec4);

	//meshDesc.triangles.data = vecIndex.data();
	//meshDesc.triangles.count = vecIndex.size() / 3;
	//meshDesc.triangles.stride = sizeof(PxU32) * 3;

	////
	//for (UINT i = 0; i < meshDesc.invMasses.count; ++i)
	//	((float*)meshDesc.invMasses.data)[i * 4] = 0.5f;

	//for (UINT nColumn = 0; nColumn < 36; ++nColumn)
	//{
	//	for (UINT nRow = 0; nRow < 21; ++nRow)
	//	{
	//		int nIndex = nRow + 21 * nColumn;
	//		if (nRow < 10)
	//		{
	//			vecVertex[nIndex].w = 0.f;
	//		}
	//	}
	//}

	//PxClothMeshQuadifier quadifier(meshDesc);

	//PxClothFabric* pClothFabric = PxClothFabricCreate(*Physics::GetPxPhysics(), meshDesc, PxVec3(0, -1, 0));

	//PxClothParticle* pParticles = (PxClothParticle*)meshDesc.points.data;

	//PxTransform transform(PxVec3(0.f, 0.f, 0.f), PxQuat(0.f, 0.f, 0.f, 1.f));

	//m_pCloth = Physics::GetPxPhysics()->createCloth(transform, *pClothFabric, pParticles, PxClothFlags());

	//Physics::AddActor(GetSceneID(), *m_pCloth);


	///*
	//	PxCloth 충돌에 사용될 PxClothCollsionSphere 설정
	//*/
	////1.PxClothCollsionSphere의 위치 설정에 사용되는 Node포인터 들을 컨테이너에 보관(NeroCoat Update에서 Node의 위치정보를 바탕으로 PxClothCollsionSphere의 위치 업데이트)
	//m_vecNode.push_back(m_pMesh->GetNode("Waist"));
	//m_vecNode.push_back(m_pMesh->GetNode("Stomach"));
	//m_vecNode.push_back(m_pMesh->GetNode("Chest"));
	//m_vecNode.push_back(m_pMesh->GetNode("L_Thigh"));
	//m_vecNode.push_back(m_pMesh->GetNode("L_Shin"));
	//m_vecNode.push_back(m_pMesh->GetNode("R_Thigh"));
	//m_vecNode.push_back(m_pMesh->GetNode("R_Shin"));


	////2.
	//vector<PxClothCollisionSphere> vecCollisionSphere;
	//vecCollisionSphere.resize(m_vecNode.size());
	//for (UINT i = 0; i < m_vecNode.size(); ++i)
	//{
	//	PxVec3 vPosition(0.f, 0.f, 0.f);
	//	memcpy_s(&vPosition, sizeof(PxVec3), m_vecNode[i]->ToRoot.m[3], sizeof(PxVec3));
	//	vecCollisionSphere[i].pos = vPosition;

	//	vecCollisionSphere[i].radius = i < 3 ? 12.f : 12.f;
	//}

	//m_pCloth->setCollisionSpheres(vecCollisionSphere.data(), vecCollisionSphere.size());
	//m_pCloth->addCollisionCapsule(0, 1);
	//m_pCloth->addCollisionCapsule(1, 2);
	//m_pCloth->addCollisionCapsule(3, 4);
	//m_pCloth->addCollisionCapsule(5, 6);
}
