#include "stdafx.h"
#include "Nero.h"
#include "Renderer.h"
#include "Subset.h"
#include "NeroFSM.h"
#include "RedQueen.h"
#include "Nero_LWing.h"
#include "Nero_RWing.h"
#include "Buster_Arm.h"
#include "Wire_Arm.h"
#include "WIngArm_Left.h"
#include "WingArm_Right.h"
#include "MainCamera.h"
#include "BtlPanel.h"
#include "GT_Overture.h"
#include "GT_Rockman.h"
#include "Monster.h"
#include "OvertureHand.h"
#include "Liquid.h"
#include "Cbs_Short.h"
#include "Cbs_Middle.h"
#include "Cbs_Long.h"
#include "Buster_Arm_Left.h"
#include "AirHike.h"
#include "Trail.h"
#include "IceAge.h"
#include "FireCircle.h"
#include "CircleWave.h"
#include "ShapeParticle.h"
#include "JudgementSword.h"
#include "JudgementShadow1.h"
#include "JudgementShadow2.h"
#include "JudgementShadow3.h"
#include "CbsTrail.h"
#include "NewWingSword.h"
#include "CbsMidTrail.h"
#include "BiAttack.h"
#include "BlitzAttack.h"
#include "LongBarrel.h"
#include "Satellite.h"
#include "CbsLongTrail.h"
#include "WhirlWind.h"
#include "Change.h"
#include "SoundSystem.h"
#include "StoneDebrisMulti.h"
#include "NeroCoat.h"
#include "Revelion.h"
#include "Judgement.h"
#include "JudgementSwordTrail.h"

Nero::Nero()
	:m_iCurAnimationIndex(ANI_END)
	, m_iPreAnimationIndex(ANI_END)
	, m_iCurWeaponIndex(NeroCom_RedQueen)
	, m_iJumpDirIndex(Basic)
	, m_fRedQueenGage(0.f)
	, m_iCurDirIndex(Dir_Front)
	, m_iPreDirIndex(Dir_Front)
	, m_IsMajin(false)
	, m_fFlySpeed(0.f)
	, m_iMeshIndex(ORIGIN_DANTE)
{
	m_nTag = Player;
	m_BattleInfo.iMaxHp = 100;
	m_BattleInfo.iHp = 100;
	//m_bDebugButton = false;
}
void Nero::Free()
{
	Unit::Free();
	m_pFSM = nullptr;
}

void Nero::Set_PlayingTime(float NewTime)
{
	m_pMesh[m_iMeshIndex]->SetPlayingTime(NewTime);
}


void Nero::Set_Weapon_Coll(NeroComponentID _eNeroComID, bool _ActiveOrNot)
{
	if (_ActiveOrNot)
	{
		std::list<std::weak_ptr<Monster>> Monsters = GetAliveMonster();
		for (auto& pMonster : Monsters)
			pMonster.lock()->Set_Coll(true);
	}
	switch (_eNeroComID)
	{
	case Nero::NeroCom_RedQueen:
		m_pRedQueen.lock()->GetComponent<CapsuleCollider>().lock()->SetActive(_ActiveOrNot);
		break;
	case Nero::NeroCom_Cbs_Short:
		m_pCbsShort.lock()->GetComponent<SphereCollider>().lock()->SetActive(_ActiveOrNot);
		break;
	case Nero::NeroCom_Cbs_Middle:
		m_pCbsMiddle.lock()->GetComponent<SphereCollider>().lock()->SetActive(_ActiveOrNot);
		break;
	case Nero::NeroCom_Cbs_Long:
		m_pCbsLong.lock()->GetComponent<CapsuleCollider>().lock()->SetActive(_ActiveOrNot);
		break;
	case Nero::NeroCom_Overture:
		m_pOverture.lock()->GetComponent<SphereCollider>().lock()->SetActive(_ActiveOrNot);
		break;
	case Nero::NeroCom_BusterArm:
		m_pBusterArm.lock()->SetActive(_ActiveOrNot);
		break;
	case Nero::NeroCom_WireArm:
		m_pWireArm.lock()->SetActive(_ActiveOrNot);
		break;
	case Nero::NeroCom_End:
		break;
	default:
		break;
	}
}

void Nero::Set_Weapon_AttType(NeroComponentID _eNeroComID, ATTACKTYPE _eAttDir)
{
	switch (_eNeroComID)
	{
	case Nero::NeroCom_RedQueen:
		m_pRedQueen.lock()->SetAttType(_eAttDir);
		m_pRedQueen.lock()->SetWeaponState(Nero::WS_Battle);
		break;
	case Nero::NeroCom_Overture:
		m_pOverture.lock()->Set_AttackType(_eAttDir);
		break;
	case Nero::NeroCom_Cbs_Short:
		m_pCbsShort.lock()->SetAttType(_eAttDir);
		break;
	case Nero::NeroCom_Cbs_Middle:
		m_pCbsMiddle.lock()->Set_AttackType(_eAttDir);
		break;
	case Nero::NeroCom_Cbs_Long:
		m_pCbsLong.lock()->Set_AttackType(_eAttDir);
	case Nero::NeroCom_End:
		break;
	default:
		break;
	}
}

void Nero::Set_Weapon_State(NeroComponentID _eNeroComID, UINT _StateIndex)
{
	switch (_eNeroComID)
	{
	case Nero::NeroCom_RedQueen:
		m_pRedQueen.lock()->SetWeaponState(_StateIndex);
		break;
	case Nero::NeroCom_Cbs_Short:
		m_pCbsShort.lock()->SetWeaponState(_StateIndex);
		break;
	case Nero::NeroCom_Cbs_Middle:
		m_pCbsMiddle.lock()->SetWeaponState(_StateIndex);
		break;
	case Nero::NeroCom_Cbs_Long:
		break;
	case Nero::NeroCom_All_Weapon:
		m_pRedQueen.lock()->SetWeaponState(_StateIndex);
		m_pCbsShort.lock()->SetWeaponState(_StateIndex);
		break;
	case Nero::NeroCom_Revelion:
		m_pRevelion.lock()->SetWeaponState(_StateIndex);
		break;
	case Nero::NeroCom_End:
		break;
	default:
		break;
	}
}

void Nero::CreateOvertureEff(EffDircetion eDir)
{
	m_pEffOverture.lock()->PlayStart();
	Matrix World = m_pTransform.lock()->GetWorldMatrix();
	Vector3 Pos, Rot;
	memcpy(&Pos, World.m[3], sizeof(Vector3));
	switch (eDir)
	{
	case EffDircetion::EffDir_Front:
		Pos += (m_pTransform.lock()->GetLook()) * -0.25f;
		Pos.y += 0.08f;
		Rot = { 0.f,0.f,0.f };
		break;
	case EffDircetion::EffDir_Up:
		Rot = { 90.f,180.f,0.f };
		Pos += (m_pTransform.lock()->GetLook()) * -0.08f;
		Pos.y += 0.23f;
		break;
	case EffDircetion::EffDir_Down:
		Rot = { -90.f,0.f,0.f };
		break;
	default:
		break;
	}
	Vector3 MyRotation = (Transform::QuaternionToEuler(m_pTransform.lock()->GetQuaternion()));

	m_pEffOverture.lock()->SetRotation(MyRotation + Rot);
	m_pEffOverture.lock()->SetPosition(Pos);
}

std::list<std::weak_ptr<Monster>> Nero::GetAllMonster()
{
	return FindGameObjectsWithType<Monster>();
}

std::list<std::weak_ptr<Monster>> Nero::GetAliveMonster()
{
	std::list<std::weak_ptr<Monster>> _AllMonsters = FindGameObjectsWithType<Monster>();
	std::list<std::weak_ptr<Monster>> _AliveMonsters;

	for (auto& pMonster : _AllMonsters)
	{
		if (!pMonster.lock()->Get_Dead())
			_AliveMonsters.emplace_back(pMonster);
	}


	return _AliveMonsters;
}

std::string Nero::GetName()
{
	return "Nero";
}

Nero* Nero::Create()
{
	return new Nero;
}

HRESULT Nero::Ready()
{
	Unit::Ready();
	RenderInit();

	vDegree = D3DXVECTOR3(0.f, 0.f, 0.f);
	vRotationDegree = D3DXVECTOR3(0.f, 0.f, 0.f);
	vAccumlatonDegree = D3DXVECTOR3(0.f, 0.f, 0.f);

	m_fAngle = 0.f;

	m_pTransform.lock()->SetScale({ 0.001f,0.001f,0.001f });

	PushEditEntity(m_pTransform.lock().get());

	m_pRedQueen = AddGameObject<RedQueen>();
	m_pLWing = AddGameObject<Nero_LWing>();
	m_pRWing = AddGameObject<Nero_RWing>();
	m_pBusterArm = AddGameObject<Buster_Arm>();
	m_pBusterArmLeft = AddGameObject<Buster_Arm_Left>();
	m_pWireArm = AddGameObject<Wire_Arm>();
	m_pWingArm_Left = AddGameObject <WIngArm_Left>();
	m_pWingArm_Right = AddGameObject<WingArm_Right>();
	m_pOverture = AddGameObject<GT_Overture>();
	m_pEffOverture = AddGameObject<OvertureHand>();
	m_pCbsShort = AddGameObject<Cbs_Short>();
	m_pCbsMiddle = AddGameObject<Cbs_Middle>();
	m_pCbsLong = AddGameObject<Cbs_Long>();
	m_pNewWingSword = AddGameObject<NewWingSword>();
	m_pBlood = AddGameObject<Liquid>();
	m_pBlood.lock()->SetScale(0.007f);
	m_pBlood.lock()->SetVariationIdx(Liquid::BLOOD_0);

	m_pAirHike = AddGameObject<AirHike>();
	m_pTrail = AddGameObject<Trail>();
	m_pIceAge = AddGameObject<IceAge>();
	m_pCbsMidTrail = AddGameObject<CbsMidTrail>();
	m_pBiAttack = AddGameObject<BiAttack>();
	m_pBlitzAttack = AddGameObject<BlitzAttack>();
	m_pLongBarrel = AddGameObject<LongBarrel>();
	m_pSatellite = AddGameObject<Satellite>();
	m_pCbsLongTrail = AddGameObject<CbsLongTrail>();
	m_pWhirlWind = AddGameObject<WhirlWind>();
	m_pJudgementSword = AddGameObject<JudgementSword>();
	m_pJudgementShadow1 = AddGameObject<JudgementShadow1>();
	m_pJudgementShadow2 = AddGameObject<JudgementShadow2>();
	m_pJudgementShadow3 = AddGameObject<JudgementShadow3>();
	m_pShockWave = AddGameObject<ShockWave>();
	m_pChange = AddGameObject<Change>();
	m_pRevelion = AddGameObject<Revelion>();
	m_pEffJudgement = AddGameObject<Judgement>();
	m_pJudgementTrail = AddGameObject<JudgementSwordTrail>();

	m_pCbsTrail = AddGameObject<CbsTrail>();
	for (int i = 0; i < 3; ++i)
	{
		m_pFireCircle[i] = AddGameObject<FireCircle>();
	}
	m_pCircleWave = AddGameObject<CircleWave>();
	m_pShapeParticle[SP_RED] = AddGameObject<ShapeParticle>();
	if (!m_pShapeParticle[SP_RED].expired())
	{
		m_pShapeParticle[SP_RED].lock()->SetShapeIdx(ShapeParticle::SPHERE);
		m_pShapeParticle[SP_RED].lock()->SetColorIdx(ShapeParticle::RED);
		m_pShapeParticle[SP_RED].lock()->SetCtrlIdx(ShapeParticle::ZERO);
		m_pShapeParticle[SP_RED].lock()->SetScale(0.0009f);
	}
	m_pShapeParticle[SP_GREEN] = AddGameObject<ShapeParticle>();
	if (!m_pShapeParticle[SP_GREEN].expired())
	{
		m_pShapeParticle[SP_GREEN].lock()->SetShapeIdx(ShapeParticle::SPHERE);
		m_pShapeParticle[SP_GREEN].lock()->SetColorIdx(ShapeParticle::GREEN);
		m_pShapeParticle[SP_GREEN].lock()->SetCtrlIdx(ShapeParticle::ZERO);
		m_pShapeParticle[SP_GREEN].lock()->SetScale(0.0009f);
	}
	m_pShapeParticle[SP_WHITE] = AddGameObject<ShapeParticle>();
	if (!m_pShapeParticle[SP_WHITE].expired())
	{
		m_pShapeParticle[SP_WHITE].lock()->SetShapeIdx(ShapeParticle::SPHERE);
		m_pShapeParticle[SP_WHITE].lock()->SetColorIdx(ShapeParticle::WHITE);
		m_pShapeParticle[SP_WHITE].lock()->SetCtrlIdx(ShapeParticle::ZERO);
		m_pShapeParticle[SP_WHITE].lock()->SetScale(0.0009f);
	}

	for (int i = 0; i < 8; ++i)
		m_pStone[i] = AddGameObject<StoneDebrisMulti>();

	m_pFSM.reset(NeroFSM::Create(static_pointer_cast<Nero>(m_pGameObject.lock())));

	m_iCurAnimationIndex = ANI_END;
	m_iPreAnimationIndex = ANI_END;

	return S_OK;
}


HRESULT Nero::Awake()
{
	Unit::Awake();
	m_pFSM->ChangeState(NeroFSM::IDLE);

	m_pCollider = AddComponent<CapsuleCollider>();
	m_pCollider.lock()->ReadyCollider();
	m_pCollider.lock()->SetRigid(true);
	m_pCollider.lock()->SetGravity(true);
	m_pCollider.lock()->SetCenter(D3DXVECTOR3(0.f, 0.09f, 0.f));
	m_pCollider.lock()->SetRadius(0.04f);
	m_pCollider.lock()->SetHeight(0.1f);
	m_pCollider.lock()->SetOffsetRadius(0.04f);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, true);
	m_pCollider.lock()->SetLockFlag(PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, true);


	PushEditEntity(m_pCollider.lock().get());

	m_pNeroCoat = AddGameObject<NeroCoat>();
	m_pNeroCoat.lock()->SetMesh(m_pMesh[ORIGIN_DANTE]);

	return S_OK;
}

HRESULT Nero::Start()
{
	Unit::Start();
	m_pCamera = std::static_pointer_cast<MainCamera>(FindGameObjectWithTag(TAG_Camera).lock());
	m_pBtlPanel = std::static_pointer_cast<BtlPanel>(FindGameObjectWithTag(UI_BtlPanel).lock());
	return S_OK;
}

UINT Nero::Update(const float _fDeltaTime)
{
	Unit::Update(_fDeltaTime);
	

	Update_Majin(_fDeltaTime);

	if (nullptr != m_pFSM && m_bDebugButton)
		m_pFSM->UpdateFSM(_fDeltaTime);

	auto [Scale, Rot, Pos] = m_pMesh[m_iMeshIndex]->Update(_fDeltaTime);
	Pos.y = 0.f;

	vAccumlatonDegree += Transform::QuaternionToEuler(Rot);

	m_pTransform.lock()->SetRotation(vDegree + vRotationDegree + vAccumlatonDegree);

	D3DXMATRIX matRot;
	D3DXQUATERNION tQuat = m_pTransform.lock()->GetQuaternion();
	D3DXMatrixRotationQuaternion(&matRot, &tQuat);
	D3DXVec3TransformCoord(&Pos, &Pos, &matRot);

	m_pTransform.lock()->Translate(Pos * m_pTransform.lock()->GetScale().x);


	//m_pBtlPanel.lock()->AccumulateTDTGauge(0.00005f);
	if(!m_pBtlPanel.expired())
		m_pBtlPanel.lock()->AccumulateTDTGauge(0.003f * _fDeltaTime);
	//m_pBtlPanel.lock()->AccumulateTDTGauge(1.f);


	/* ShapeParticle 위치 갱신 */
	for (int i = 0; i < SP_END; ++i)
	{
		//if (m_pShapeParticle[i].lock()->IsPlaying())
		m_pShapeParticle[i].lock()->SetPosition(Get_NeroBoneWorldPos("Waist"));
	}
	/* ----------------------------------- */

	if (Input::GetKeyDown(DIK_9))
	{
		m_pFSM->ChangeState(NeroFSM::TRANSFORM_SHINMAJIN);
	}

	if (Input::GetKeyDown(DIK_1))
	{
		BuyUpgradedOverture();
		BuyCbsMiddle();
		BuyCbsLong();
	}

	if (Input::GetKeyDown(DIK_2))
	{
		UseRevelion();
	}


	if (80 <= SoundSystem::GetInstance()->CurrentPosition("GetOrb1"))
	{
		SoundSystem::GetInstance()->Play("GetOrb2", 0.6f, false);
	}
	if (90 <= SoundSystem::GetInstance()->CurrentPosition("GetOrb2"))
	{
		SoundSystem::GetInstance()->Play("GetOrb3", 0.6f, false);
	}
	if (70 <= SoundSystem::GetInstance()->CurrentPosition("GetOrb3"))
	{
		SoundSystem::GetInstance()->Play("GetOrb4", 0.6f, false);
	}
	if (90 <= SoundSystem::GetInstance()->CurrentPosition("GetOrb4"))
	{
		SoundSystem::GetInstance()->Play("GetOrb5", 0.6f, false);
	}
	if (80 <= SoundSystem::GetInstance()->CurrentPosition("GetOrb5"))
	{
		SoundSystem::GetInstance()->Play("GetOrb6", 0.6f, false);
	}
	if (90 <= SoundSystem::GetInstance()->CurrentPosition("GetOrb6"))
	{
		SoundSystem::GetInstance()->Play("GetOrb7", 0.6f, false);
	}
	

	return 0;
}

UINT Nero::LateUpdate(const float _fDeltaTime)
{
	Unit::LateUpdate(_fDeltaTime);

	return 0;
}

void Nero::OnEnable()
{
	Unit::OnEnable();
	_RenderProperty.bRender = true;
}

void Nero::OnDisable()
{
	Unit::OnDisable();
	_RenderProperty.bRender = false;
}

void Nero::Hit(BT_INFO _BattleInfo, void* pArg)
{
	StopEffect(Tag_END);
	m_pBlood.lock()->SetPosition(Get_NeroBoneWorldPos("Waist"));
	m_pBlood.lock()->PlayStart(40.f);
	m_BattleInfo.iHp -= _BattleInfo.iAttack;
	float fHpRatio = float(float(m_BattleInfo.iHp) / float(m_BattleInfo.iMaxHp));
	if (!m_pBtlPanel.expired())
		m_pBtlPanel.lock()->SetPlayerHPRatio(fHpRatio);
	RotateToHitMonster(*(std::weak_ptr<GameObject>*)(pArg));
	if (m_IsFly)
	{
		m_pFSM->ChangeState(NeroFSM::HIT_AIR_AWAY);
		if (NeroCom_RedQueen == m_iCurWeaponIndex)
			m_pRedQueen.lock()->SetWeaponState(WS_Idle);
		else
			SetCbsIdle();

		return;
	}
	switch (_BattleInfo.eAttackType)
	{
	case Attack_Front:
		m_pFSM->ChangeState(NeroFSM::STATERESET);
		m_pFSM->ChangeState(NeroFSM::HIT_FRONT);
		SoundSystem::GetInstance()->RandSoundKeyPlay("DanteHit", { 2,12 }, 1.f, true);
		break;
	case Attack_Hard:
		break;
	case Attack_KnocBack:
		SoundSystem::GetInstance()->Play("DanteHit_1", 1.f, true);
		m_pFSM->ChangeState(NeroFSM::HIT_GROUND_AWAY);
		break;
	case Attack_Stun:
		SoundSystem::GetInstance()->RandSoundKeyPlay("DanteHit", { 2,12 }, 1.f, true);
		m_pFSM->ChangeState(NeroFSM::STUN_START);
		break;
	default:
		SoundSystem::GetInstance()->RandSoundKeyPlay("DanteHit", { 2,12 }, 1.f, true);
		m_pFSM->ChangeState(NeroFSM::HIT_FRONT);
		break;
	}
}

void Nero::OnCollisionEnter(std::weak_ptr<GameObject> _pOther)
{
	UINT iFsmTag = m_pFSM->GetCurrentIndex();
	if (NeroFSM::SKILL_STREAK_LOOP == iFsmTag
		|| NeroFSM::SKILL_STREAK == iFsmTag)
	{
		m_pFSM->ChangeState(NeroFSM::SKILL_STREAK_END);
		return;
	}
	if (NeroFSM::SKILL_AIR_DIVE_SLASH_LOOP == iFsmTag
		|| NeroFSM::SKILL_AIR_DIVE_SLASH_START == iFsmTag)
	{
		m_pFSM->ChangeState(NeroFSM::SKILL_AIR_DIVE_SLASH_END);
		return;
	}
	GAMEOBJECTTAG eTag = GAMEOBJECTTAG(_pOther.lock()->m_nTag);
	switch (eTag)
	{
	case Monster5300:
	{
		//현재 상태가 플라이 루프일때
		//플라이 엔드로 상태 변환
		//m_pLetMeFlyMonster 초기화

		if (NeroFSM::WIRE_HELLHOUND_LOOP == iFsmTag
			|| NeroFSM::WIRE_HELLHOUND_START == iFsmTag)
		{
			m_pLetMeFlyMonster.reset();
			m_pFSM->ChangeState(NeroFSM::WIRE_HELLHOUND_END);
			m_fFlySpeed = 0.f;
			return;
		}
	}
	break;
	case Monster5000:
	{
		//현재 상태가 플라이 루프일때
		//플라이 엔드로 상태 변환
		//m_pLetMeFlyMonster 초기화

		if (NeroFSM::WIRE_HELLHOUND_LOOP == iFsmTag
			|| NeroFSM::WIRE_HELLHOUND_START == iFsmTag)
		{
			m_pLetMeFlyMonster.reset();
			m_pFSM->ChangeState(NeroFSM::WIRE_HELLHOUND_END);
			m_fFlySpeed = 0.f;
			return;
		}
	}
	break;
	case Monster1000:
	{
		//현재 상태가 플라이 루프일때
		//플라이 엔드로 상태 변환
		//m_pLetMeFlyMonster 초기화

		if (NeroFSM::WIRE_HELLHOUND_LOOP == iFsmTag
			|| NeroFSM::WIRE_HELLHOUND_START == iFsmTag)
		{
			m_pLetMeFlyMonster.reset();
			m_pFSM->ChangeState(NeroFSM::WIRE_HELLHOUND_END);
			m_fFlySpeed = 0.f;
			return;
		}
	}
	break;
	case MonsterSnatchPoint:
	{
		if (NeroFSM::WIRE_HELLHOUND_LOOP == iFsmTag
			|| NeroFSM::WIRE_HELLHOUND_START == iFsmTag)
		{
			m_pLetMeFlyMonster.reset();
			m_pFSM->ChangeState(NeroFSM::WIRE_HELLHOUND_END);
			m_fFlySpeed = 0.f;
			return;
		}
	}
	break;
	}
}

void Nero::OnCollisionStay(std::weak_ptr<GameObject> _pOther)
{
	UINT iFsmTag = m_pFSM->GetCurrentIndex();
	GAMEOBJECTTAG eTag = GAMEOBJECTTAG(_pOther.lock()->m_nTag);
	//현재 상태가 플라이 루프일때
	//플라이 엔드로 상태 변환
	//m_pLetMeFlyMonster 초기화
	switch (eTag)
	{
	case Monster5300:
		if (NeroFSM::WIRE_HELLHOUND_LOOP == iFsmTag
			|| NeroFSM::WIRE_HELLHOUND_START == iFsmTag)
		{
			m_pLetMeFlyMonster.reset();
			m_pFSM->ChangeState(NeroFSM::WIRE_HELLHOUND_END);
			m_fFlySpeed = 0.f;
			return;
		}
		break;
	case Monster5000:
		if (NeroFSM::WIRE_HELLHOUND_LOOP == iFsmTag
			|| NeroFSM::WIRE_HELLHOUND_START == iFsmTag)
		{
			m_pLetMeFlyMonster.reset();
			m_pFSM->ChangeState(NeroFSM::WIRE_HELLHOUND_END);
			m_fFlySpeed = 0.f;
			return;
		}
		break;
	case Monster1000:
		if (NeroFSM::WIRE_HELLHOUND_LOOP == iFsmTag
			|| NeroFSM::WIRE_HELLHOUND_START == iFsmTag)
		{
			if (m_pLetMeFlyMonster.lock() == static_pointer_cast<Monster>(_pOther.lock()))
			{
				m_pLetMeFlyMonster.reset();
				m_pFSM->ChangeState(NeroFSM::WIRE_HELLHOUND_END);
				m_fFlySpeed = 0.f;
			}
			return;
		}
	break;
	case MonsterSnatchPoint:
		if (NeroFSM::WIRE_HELLHOUND_LOOP == iFsmTag
			|| NeroFSM::WIRE_HELLHOUND_START == iFsmTag)
		{
			if (m_pLetMeFlyMonster.lock() == static_pointer_cast<Monster>(_pOther.lock()))
			{
				m_pLetMeFlyMonster.reset();
				m_pFSM->ChangeState(NeroFSM::WIRE_HELLHOUND_END);
				m_fFlySpeed = 0.f;
			}
			return;
		}
		break;
	//보스가 아니라 여러마리있는애들이면
	//태그 검사이후에
	//_pOther를 Monster로 캐스팅하고 m_pLetMeFly몬스터랑 같은건지 이중검사해줘야함
	}
}

void Nero::OnTriggerEnter(std::weak_ptr<GameObject> _pOther)
{
	if (m_Nohit)
		return;
	UINT iFsmTag = m_pFSM->GetCurrentIndex();

	GAMEOBJECTTAG eTag = GAMEOBJECTTAG(_pOther.lock()->m_nTag);
	switch (eTag)
	{
	case MonsterWeapon:
		//회피기일때 탈출
		if (NeroFSM::EVADE_L == iFsmTag || NeroFSM::EVADE_R == iFsmTag)
			return;
		if (!static_pointer_cast<Unit>(_pOther.lock())->Get_Coll())
			return;
		Hit(static_pointer_cast<Unit>(_pOther.lock())->Get_BattleInfo(), (void*)&_pOther);
		static_pointer_cast<Unit>(_pOther.lock())->Set_Coll(false);
		break;
	case Eff_NuClear:
		m_BattleInfo.iHp = 0;
		if (!m_pBtlPanel.expired())
			m_pBtlPanel.lock()->SetPlayerHPRatio(0);
		m_pFSM->ChangeState(NeroFSM::DIE);
		break;
	default:
		break;
	}
}

void Nero::OnTriggerExit(std::weak_ptr<GameObject> _pOther)
{

}

void Nero::RenderGBufferSK(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = m_pMesh[m_iMeshIndex]->GetNumSubset();
	if (Numsubset > 0)
	{
		m_pMesh[m_iMeshIndex]->BindVTF(_Info.Fx);
	};
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh[m_iMeshIndex]->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->BindProperty(TextureType::DIFFUSE, 0, 0, _Info._Device);
			SpSubset->BindProperty(TextureType::NORMALS, 0, 1, _Info._Device);
			SpSubset->Render(_Info.Fx);
		};

		/*if (auto SpSubset = m_pMesh[m_iMeshIndex]->GetSubset(i).lock();
			SpSubset)
		{
			if (auto exSpSubset = m_pMesh[m_iMeshIndex_ex]->GetSubset(i).lock();
				exSpSubset)
			{
				exSpSubset->BindProperty(TextureType::DIFFUSE, 0, 0, _Info._Device);
				exSpSubset->BindProperty(TextureType::NORMALS, 0, 1, _Info._Device);
			};
			SpSubset->Render(_Info.Fx);
		};*/
	};
}

void Nero::RenderShadowSK(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("matWorld", &World);
	const uint32 Numsubset = m_pMesh[m_iMeshIndex]->GetNumSubset();
	if (Numsubset > 0)
	{
		m_pMesh[m_iMeshIndex]->BindVTF(_Info.Fx);
	};
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh[m_iMeshIndex]->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
}

void Nero::RenderDebugBone(const DrawInfo& _Info)
{
	const Matrix ScaleOffset = FMath::Scale({ GScale,GScale ,GScale });
	m_pMesh[m_iMeshIndex]->BoneDebugRender(_RenderUpdateInfo.World, _Info.Fx);
}

void Nero::RenderDebugSK(const DrawInfo& _Info)
{
	const Matrix World = _RenderUpdateInfo.World;
	_Info.Fx->SetMatrix("World", &World);
	const uint32 Numsubset = m_pMesh[m_iMeshIndex]->GetNumSubset();

	if (Numsubset > 0)
	{
		m_pMesh[m_iMeshIndex]->BindVTF(_Info.Fx);
	};
	for (uint32 i = 0; i < Numsubset; ++i)
	{
		if (auto SpSubset = m_pMesh[m_iMeshIndex]->GetSubset(i).lock();
			SpSubset)
		{
			SpSubset->Render(_Info.Fx);
		};
	};
}

void Nero::RenderInit()
{
	// ������ �����ؾ��ϴ� ������Ʈ��� (�������� ��� ����) �˸�.
	// ���� �������̽� ��ӹ��� �ʾҴٸ� Ű��������.
	SetRenderEnable(true);

	// ���� �Ӽ� ��ü �ʱ�ȭ 
	ENGINE::RenderProperty _InitRenderProp;
	// �̰��� ��Ÿ�ӿ� �ٲٸ� ������ �Ѱ� ���� ����. 
	_InitRenderProp.bRender = true;
	_InitRenderProp.RenderOrders[RenderProperty::Order::GBuffer] =
	{
		{"gbuffer_dsSK",
		[this](const DrawInfo& _Info)
			{
				RenderGBufferSK(_Info);
			}
		},
	};
	_InitRenderProp.RenderOrders[RenderProperty::Order::Shadow]
		=
	{
		{"ShadowSK" ,
		[this](const DrawInfo& _Info)
		{
			RenderShadowSK(_Info);
		}
	} };
	_InitRenderProp.RenderOrders[RenderProperty::Order::DebugBone]
		=
	{
		{"DebugBone" ,
		[this](const DrawInfo& _Info)
		{
			RenderDebugBone(_Info);
		}
	} };
	_InitRenderProp.RenderOrders[RenderProperty::Order::Debug]
		=
	{
		{"DebugSK" ,
		[this](const DrawInfo& _Info)
		{
			RenderDebugSK(_Info);
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

	// ���̷��� �޽� �ε� ... 
	Mesh::InitializeInfo _InitInfo{};
	// ���ؽ� ���� ������ CPU ������ �ʿ� �Ѱ� ? 
	_InitInfo.bLocalVertexLocationsStorage = false;
	m_pMesh[ORIGIN_DANTE] = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Player.fbx", _InitInfo);

	m_pMesh[ORIGIN_DANTE]->LoadAnimationFromDirectory(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Animation");
	m_pMesh[ORIGIN_DANTE]->AnimationDataLoadFromJsonTable(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\Player.Animation");
	m_pMesh[ORIGIN_DANTE]->EnableToRootMatricies();

	PushEditEntity(m_pMesh[ORIGIN_DANTE].get());

	m_pMesh[SHINMAJIN_DANTE] = Resources::Load<SkeletonMesh>(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\ShinMajin\\Model.fbx", _InitInfo);

	m_pMesh[SHINMAJIN_DANTE]->LoadAnimationFromDirectory(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\ShinMajin\\Ani");
	m_pMesh[SHINMAJIN_DANTE]->AnimationDataLoadFromJsonTable(L"..\\..\\Resource\\Mesh\\Dynamic\\Dante\\ShinMajin\\Model.Animation");
	m_pMesh[SHINMAJIN_DANTE]->EnableToRootMatricies();

	PushEditEntity(m_pMesh[SHINMAJIN_DANTE].get());
}

void Nero::Update_Majin(float _fDeltaTime)
{
	if (!m_IsMajin)
		return;
	if (!m_pBtlPanel.expired() && m_pBtlPanel.lock()->GetTDTGauge() <= 0.f)
	{
		m_IsMajin = false;
		//SetActive_NeroComponent(NeroCom_Wings, false);
		m_pLWing.lock()->SetDissolve();
		m_pRWing.lock()->SetDissolve();
		SetActive_NeroComponent(NeroCom_WIngArm_Left, false);
		SetActive_NeroComponent(NeroCom_WingArm_Right, false);
		SoundSystem::GetInstance()->Play("TransformToDante", 0.3f, false);
	}

}


void Nero::RenderReady()
{
	auto _WeakTransform = GetComponent<ENGINE::Transform>();
	if (auto _SpTransform = _WeakTransform.lock();
		_SpTransform)
	{
		_RenderUpdateInfo.World = _SpTransform->GetRenderMatrix();
	}
}


void Nero::Editor()
{
	Unit::Editor();
	bool MyButton = true;
	float ZeroDotOne = 0.1f;
	if (bEdit)
	{
		// ������ .... 
		ImGui::Checkbox("KeyInput", &m_bDebugButton);
		if (ImGui::Button("ResetAnimation"))
		{
			ChangeAnimation("Idle_Battle", true, ANI_IDLE_BATTLE);
		}
		if (ImGui::Button("BuyItems"))
		{
			BuyUpgradedOverture();
			BuyCbsMiddle();
			BuyCbsLong();
		}
		m_pRWing.lock()->m_DissolveInfo.DissolveEditor();
		m_pLWing.lock()->m_DissolveInfo.DissolveEditor();
		//std::weak_ptr<GameObject> _Monster = FindGameObjectWithTag(Monster5000).lock();
		//if (_Monster.expired())
		//	return;
		//std::weak_ptr<Transform> _MonsterTransform = _Monster.lock()->GetComponent<Transform>();
		//Vector3 vMonsterPos = _MonsterTransform.lock()->GetPosition();
		//Vector3 vMyPos = m_pTransform.lock()->GetPosition();

		//Vector3 vDir = vMonsterPos - vMyPos;
		//vDir.y = 0;
		//D3DXVec3Normalize(&vDir, &vDir);

		//Vector3 vLook = -m_pTransform.lock()->GetLook();

		//float fDot = D3DXVec3Dot(&vDir, &vLook);

		//if (fDot > 1.f)
		//	fDot = 1.f - FLT_EPSILON;
		//else if (fDot < -1.f)
		//	fDot = -1.f + FLT_EPSILON;

		//float fRadian = acosf(fDot);

		//Vector3	vCross;
		//D3DXVec3Cross(&vCross, &vLook, &vDir);

		//if (vCross.y > 0)
		//	fRadian *= -1;
		//if (ImGui::TreeNode("Angle"))
		//{
		//	ImGui::Text("%f", -D3DXToDegree(fRadian));
		//}
		//Vector3 vEye = m_pCamera.lock()->GetEye();
		//Vector3 Length = vEye - m_pTransform.lock()->GetPosition();

		//if (ImGui::TreeNode("Distance22222"))
		//{
		//	ImGui::Text("%f", D3DXVec3Length(&Length));
		//}
	}

}

float Nero::Get_PlayingTime()
{
	return m_pMesh[m_iMeshIndex]->PlayingTime();
}

float Nero::Get_PlayingAccTime()
{
	return m_pMesh[m_iMeshIndex]->PlayingAccTime();
}

optional<Matrix> Nero::Get_BoneMatrix_ByName(std::string _BoneName)
{
	return m_pMesh[m_iMeshIndex]->GetNodeToRoot(_BoneName);
}

Matrix* Nero::GetJudgementWeaponBone()
{
	return m_pMesh[SHINMAJIN_DANTE]->GetToRootMatrixPtr("R_WeaponHand");
}

Matrix* Nero::Get_BoneMatrixPtr(std::string _BoneName)
{
	return m_pMesh[m_iMeshIndex]->GetToRootMatrixPtr(_BoneName);;
}

Matrix Nero::Get_NeroBoneWorldMatrix(std::string _BoneName)
{
	return *m_pMesh[m_iMeshIndex]->GetNodeToRoot(_BoneName) * m_pTransform.lock()->GetWorldMatrix();
}

Vector3 Nero::Get_NeroBoneWorldPos(std::string _BoneName)
{
	Vector3 WorldPos;
	memcpy(&WorldPos, (*m_pMesh[m_iMeshIndex]->GetNodeToRoot(_BoneName) * m_pTransform.lock()->GetWorldMatrix()).m[3], sizeof(Vector3));
	return WorldPos;
}


void Nero::SetActive_NeroComponent(NeroComponentID _eNeroComID, bool ActiveOrNot)
{
	switch (_eNeroComID)
	{
	case Nero::NeroCom_Wings:
		m_pLWing.lock()->SetActive(ActiveOrNot);
		m_pRWing.lock()->SetActive(ActiveOrNot);
		break;
	case Nero::NeroCom_LWing:
		m_pLWing.lock()->SetActive(ActiveOrNot);
		break;
	case Nero::NeroCom_RWing:
		m_pRWing.lock()->SetActive(ActiveOrNot);
		break;
	case Nero::NeroCom_BusterArm:
		m_pBusterArm.lock()->SetActive(ActiveOrNot);
		break;
	case Nero::NeroCom_BusterArm_Left:
		m_pBusterArm.lock()->SetActive(ActiveOrNot);
		break;
	case Nero::NeroCom_WireArm:
		m_pWireArm.lock()->SetActive(ActiveOrNot);
		break;
	case Nero::NeroCom_WIngArm_Left:
		m_pWingArm_Left.lock()->SetActive(ActiveOrNot);
		break;
	case Nero::NeroCom_WingArm_Right:
		m_pWingArm_Right.lock()->SetActive(ActiveOrNot);
		break;
	case Nero::NeroCom_Overture:
		m_pOverture.lock()->SetActive(ActiveOrNot);
		break;
	case Nero::NeroCom_All_Weapon:

		m_pBusterArm.lock()->SetActive(ActiveOrNot);
		m_pBusterArmLeft.lock()->SetActive(ActiveOrNot);
		m_pWireArm.lock()->SetActive(ActiveOrNot);
		m_pWingArm_Left.lock()->SetActive(ActiveOrNot);
		m_pWingArm_Right.lock()->SetActive(ActiveOrNot);
		m_pOverture.lock()->SetActive(ActiveOrNot);
		m_pRedQueen.lock()->SetActive(ActiveOrNot);
		if (m_pLWing.lock()->IsActive())
		{
			m_pLWing.lock()->SetDissolve();
			m_pRWing.lock()->SetDissolve();
		}
		break;
	case Nero::NeroCom_NewWingSword:
		m_pNewWingSword.lock()->SetActive(ActiveOrNot);
		break;
	case Nero::NeroCom_Revelion:
		m_pRevelion.lock()->SetActive(ActiveOrNot);
		break;
	case Nero::NeroCom_End:
		break;
	default:
		break;
	}
}

void Nero::SetAngleFromCamera(float _fAddAngle)
{
	if (m_pCamera.expired())
		return;

	m_fAngle = m_pCamera.lock()->Get_Angle(_fAddAngle);
	vDegree.y = m_fAngle;
	vRotationDegree.y = m_fRotationAngle;
}

void Nero::SetAddForce(Vector3 _vJumpPos)
{
	m_pCollider.lock()->AddForce(_vJumpPos);
}

void Nero::SetAddForce_Dir(NeroDirection _eDir, float _fPower)
{
	switch (_eDir)
	{
	case Nero::Dir_Front:
		m_pCollider.lock()->AddForce(m_pTransform.lock()->GetLook() * -1.f * _fPower);
		break;
	case Nero::Dir_Back:
		break;
	case Nero::Dir_Left:
		break;
	case Nero::Dir_Right:
		break;
	case Nero::Dir_Front_Down:
	{
		Vector3 vFrontDownDir = m_pTransform.lock()->GetLook() * -1.f;
		vFrontDownDir.y = -0.5f;
		m_pCollider.lock()->AddForce(vFrontDownDir * _fPower);
	}
	break;
	default:
		break;
	}
}

void Nero::SetLockOnMonster()
{
	if (!m_pTargetMonster.expired())
		return;

	std::list<std::weak_ptr<Monster>> MonsterList = GetAliveMonster();
	if (0 == MonsterList.size())
		return;

	Vector3 Dir = MonsterList.begin()->lock()->GetComponent<Transform>().lock()->GetPosition() - m_pTransform.lock()->GetPosition();
	float Distance = D3DXVec3Length(&Dir);
	//여기서 조건 검사해야됨 너무 멀면 찾지도못하게
	m_pTargetMonster = MonsterList.begin()->lock();
	if (!m_pTargetMonster.lock()->Get_TargetEnable())
	{
		m_pTargetMonster.reset();
		Distance = 1000.f;
	}
	for (auto& pMonster : MonsterList)
	{
		Vector3 Direction = pMonster.lock()->GetComponent<Transform>().lock()->GetPosition() - m_pTransform.lock()->GetPosition();
		if(!pMonster.lock()->Get_TargetEnable())
			continue;
		float Temp = D3DXVec3Length(&Direction);
		//여기서 조건 검사해야됨 너무 멀면 찾지도못하게
		if (Distance >= Temp)
		{
			Distance = Temp;
			m_pTargetMonster = pMonster;
		}
	}
	if (!m_pBtlPanel.expired())
		m_pBtlPanel.lock()->SetTargetCursorActive(true);
	//RotateToTargetMonster();
}

void Nero::SetOffLockOnMonster()
{
	if (!m_pBtlPanel.expired())
		m_pBtlPanel.lock()->SetTargetCursorActive(false);
	m_pTargetMonster.reset();
}

void Nero::SetDashLoopDir()
{
	m_iDashLoopDir = m_iDashLoopDir == 1 ? -1 : 1;
}

void Nero::SetLinearVelocity(const D3DXVECTOR3 _vLinearVelocity)
{
	if (m_pCollider.expired())
		return;

	m_pCollider.lock()->SetLinearVelocity(_vLinearVelocity);
}

void Nero::Set_GrabEnd(bool _bGrabEnd)
{
	m_pWireArm.lock()->Set_GrabEnd(_bGrabEnd);
}

void Nero::SetCbsIdle()
{
	m_pCbsLong.lock()->SetActive(false);
	m_pCbsMiddle.lock()->SetActive(false);
	m_pCbsShort.lock()->SetActive(true);
	m_pCbsShort.lock()->GetComponent<SphereCollider>().lock()->SetActive(false);
	m_pCbsShort.lock()->SetWeaponState(Nero::WS_Idle);
	m_pCbsShort.lock()->ChangeAnimation("Cbs_Idle", true);
	m_pCbsTrail.lock()->PlayEnd();
	if (!m_pBtlPanel.expired())
		m_pBtlPanel.lock()->ChangeWeaponUI(Cbs, 0);
}

void Nero::SetLetMeFlyMonster(std::weak_ptr<Monster> _pMonster)
{
	m_pLetMeFlyMonster = _pMonster;
}

void Nero::SetPosFireCircle()
{
	for (int i = 0; i < 3; ++i)
	{
		m_pFireCircle[i].lock()->GetComponent<Transform>().lock()->SetPosition(m_pTransform.lock()->GetPosition() + Vector3{ 0.f,0.1f,0.f });
	}
}

void Nero::SetAngle(float _fAngle)
{
	vDegree.y = _fAngle;
	m_fAngle = _fAngle;
}


void Nero::SetEm5300()
{
	m_pTargetMonster = static_pointer_cast<Monster>(FindGameObjectWithTag(GAMEOBJECTTAG::Monster5300).lock());
}

void Nero::CheckAutoRotate()
{
	std::list<std::weak_ptr<Monster>> MonsterList = GetAliveMonster();
	std::weak_ptr<Monster> LockOnMonster;
	if (0 == MonsterList.size())
		return;

	Vector3 Dir = MonsterList.begin()->lock()->GetComponent<Transform>().lock()->GetPosition() - m_pTransform.lock()->GetPosition();
	float Distance = D3DXVec3Length(&Dir);
	LockOnMonster = MonsterList.begin()->lock();

	for (auto& pMonster : MonsterList)
	{
		Vector3 Direction = pMonster.lock()->GetComponent<Transform>().lock()->GetPosition() - m_pTransform.lock()->GetPosition();
		float Temp = D3DXVec3Length(&Direction);
		if (Distance >= Temp)
		{
			Distance = Temp;
			LockOnMonster = pMonster;
		}
	}

	if (Distance <= RotateDistance)
	{
		Vector3 vMonsterPos = LockOnMonster.lock()->GetComponent<Transform>().lock()->GetPosition();
		Vector3 vMyPos = m_pTransform.lock()->GetPosition();

		Vector3 vDir = vMonsterPos - vMyPos;
		vDir.y = 0;
		D3DXVec3Normalize(&vDir, &vDir);

		Vector3 vLook = -m_pTransform.lock()->GetLook();

		float fDot = D3DXVec3Dot(&vDir, &vLook);

		if (fDot > 1.f)
			fDot = 1.f - FLT_EPSILON;
		else if (fDot < -1.f)
			fDot = -1.f + FLT_EPSILON;

		float fRadian = acosf(fDot);

		Vector3	vCross;
		D3DXVec3Cross(&vCross, &vLook, &vDir);

		if (vCross.y > 0)
			fRadian *= -1;
		m_fAngle += -D3DXToDegree(fRadian) + vRotationDegree.y;
		vDegree.y = m_fAngle;
		vRotationDegree.y = m_fRotationAngle = 0;
		Reset_RootRotation();
	}

}

bool Nero::CheckIsGround()
{
	if (m_pCollider.lock()->IsGround())
	{
		m_IsFly = false;
		return true;
	}
	return false;
}

void Nero::Locking()
{
	SetLockOnMonster();
	if (m_pTargetMonster.expired())
		return;
	UINT iMonsterTag = m_pTargetMonster.lock()->m_nTag;
	float fHpRatio = float(float(m_pTargetMonster.lock()->Get_BattleInfo().iHp) / float(m_pTargetMonster.lock()->Get_BattleInfo().iMaxHp));
	switch (iMonsterTag)
	{
	case GAMEOBJECTTAG::Monster1000:
		if (!m_pBtlPanel.expired())
			m_pBtlPanel.lock()->SetTargetCursor(
			m_pTargetMonster.lock()->GetMonsterBoneWorldPos("Vine01_IK"),
			fHpRatio);
		break;
	case GAMEOBJECTTAG::Monster5000:
		if (!m_pBtlPanel.expired())
			m_pBtlPanel.lock()->SetTargetCursor(
			m_pTargetMonster.lock()->GetMonsterBoneWorldPos("Hip"),
			fHpRatio);
		break;
	case GAMEOBJECTTAG::MonsterSnatchPoint:
		m_pBtlPanel.lock()->SetTargetCursor(
			m_pTargetMonster.lock()->GetComponent<Transform>().lock()->GetPosition());
		break;
	default:
		if (!m_pBtlPanel.expired())
			m_pBtlPanel.lock()->SetTargetCursor(
			m_pTargetMonster.lock()->GetMonsterBoneWorldPos("Hip"),
			fHpRatio);
		break;
	}
}

Nero::NeroDirection Nero::RotateToTargetMonster()
{
	if (m_pTargetMonster.expired())
		return NeroDirection::Dir_End;
	Vector3 vMonsterPos{};
	UINT iMonsterTag = m_pTargetMonster.lock()->m_nTag;

	switch (iMonsterTag)
	{
	case GAMEOBJECTTAG::Monster1000:
		vMonsterPos = m_pTargetMonster.lock()->GetComponent<Transform>().lock()->GetPosition();
		break;
	case GAMEOBJECTTAG::MonsterSnatchPoint:
		vMonsterPos = m_pTargetMonster.lock()->GetComponent<Transform>().lock()->GetPosition();
		break;
	default:
		vMonsterPos = m_pTargetMonster.lock()->GetMonsterBoneWorldPos("Hip");
		break;
	}

	//Vector3 vMonsterPos = m_pTargetMonster.lock()->GetComponent<Transform>().lock()->GetPosition();
	Vector3 vMyPos = m_pTransform.lock()->GetPosition();

	Vector3 vDir, vNewDir;
	vDir = vNewDir = vMonsterPos - vMyPos;
	vDir.y = 0;
	D3DXVec3Normalize(&vDir, &vDir);
	D3DXVec3Normalize(&vNewDir, &vNewDir);

	Vector3 vLook = -m_pTransform.lock()->GetLook();

	float fDot = D3DXVec3Dot(&vDir, &vLook);
	if (fDot > 1.f)
		fDot = 1.f - FLT_EPSILON;
	else if (fDot < -1.f)
		fDot = -1.f + FLT_EPSILON;
	float fRadian = acosf(fDot);

	Vector3	vCross;
	D3DXVec3Cross(&vCross, &vLook, &vDir);

	if (vCross.y > 0)
		fRadian *= -1;
	m_fAngle += -D3DXToDegree(fRadian) + vRotationDegree.y;
	vDegree.y = m_fAngle;
	vRotationDegree.y = m_fRotationAngle = 0;
	Reset_RootRotation();

	m_pTransform.lock()->SetRotation(vDegree + vRotationDegree + vAccumlatonDegree);
	//와이어팔 회전할 각도 구해주는곳

	vLook = -m_pTransform.lock()->GetLook();
	fDot = D3DXVec3Dot(&vNewDir, &vLook);
	if (fDot > 1.f)
		fDot = 1.f - FLT_EPSILON;
	else if (fDot < -1.f)
		fDot = -1.f + FLT_EPSILON;
	fRadian = acosf(fDot);
	float fDegree = D3DXToDegree(fRadian);
	if (85.f <= fDegree || fDegree <= -85.f)
	{
		m_pWireArm.lock()->Set_RadianForRotX(0.f);
	}
	else
	{
		if (vNewDir.y < 0.f)
			fRadian *= -1.f;
		m_pWireArm.lock()->Set_RadianForRotX(fRadian);
	}

	if (fDegree > 5.f)
	{
		return NeroDirection::Dir_Down;
	}
	else if (fDegree < -5.f)
	{
		return  NeroDirection::Dir_Up;
	}
	return NeroDirection::Dir_Front;
}

void Nero::RotateToHitMonster(std::weak_ptr<GameObject> _pMonster)
{
	Vector3 vMonsterPos = _pMonster.lock()->GetComponent<Transform>().lock()->GetPosition();
	Vector3 vMyPos = m_pTransform.lock()->GetPosition();

	Vector3 vDir = vMonsterPos - vMyPos;
	vDir.y = 0;
	D3DXVec3Normalize(&vDir, &vDir);

	Vector3 vLook = -m_pTransform.lock()->GetLook();

	float fDot = D3DXVec3Dot(&vDir, &vLook);
	if (fDot > 1.f)
		fDot = 1.f - FLT_EPSILON;
	else if (fDot < -1.f)
		fDot = -1.f + FLT_EPSILON;
	float fRadian = acosf(fDot);

	Vector3	vCross;
	D3DXVec3Cross(&vCross, &vLook, &vDir);

	if (vCross.y > 0)
		fRadian *= -1;
	m_fAngle += -D3DXToDegree(fRadian) + vRotationDegree.y;
	vDegree.y = m_fAngle;
	vRotationDegree.y = m_fRotationAngle = 0;
	Reset_RootRotation();
}

void Nero::NeroMove(NeroDirection _eDir, float _fPower)
{
	switch (_eDir)
	{
	case Nero::Dir_Front:
		m_pTransform.lock()->Translate(m_pTransform.lock()->GetLook() * -1.f * _fPower);
		break;
	case Nero::Dir_Back:
		m_pTransform.lock()->Translate(m_pTransform.lock()->GetLook() * _fPower);
		break;
	case Nero::Dir_Left:
		m_pTransform.lock()->Translate(m_pTransform.lock()->GetRight() * _fPower);
		break;
	case Nero::Dir_Right:
		m_pTransform.lock()->Translate(m_pTransform.lock()->GetRight() * -1.f * _fPower);
		break;
	case Nero::Dir_Front_Down:
		break;
	default:
		break;
	}
}

void Nero::NeroMoveLerf(NeroDirection _eDir, float _fPower, float _fMax)
{
	if (m_fLerfAmount <= _fMax)
		m_fLerfAmount += _fPower;
	switch (_eDir)
	{
	case Nero::Dir_Front:
		m_pTransform.lock()->Translate(m_pTransform.lock()->GetLook() * -1.f * m_fLerfAmount);
		break;
	case Nero::Dir_Back:
		m_pTransform.lock()->Translate(m_pTransform.lock()->GetLook() * m_fLerfAmount);
		break;
	case Nero::Dir_Left:
		m_pTransform.lock()->Translate(m_pTransform.lock()->GetRight() * m_fLerfAmount);
		break;
	case Nero::Dir_Right:
		m_pTransform.lock()->Translate(m_pTransform.lock()->GetRight() * -1.f * m_fLerfAmount);
		break;
	case Nero::Dir_Front_Down:
		break;
	default:
		break;
	}
}

void Nero::WireFly()
{
	if (m_pLetMeFlyMonster.expired())
		return;

	if (m_fFlySpeed <= 0.23f)
		m_fFlySpeed += 0.001f;

	UINT iMonsterTag = m_pLetMeFlyMonster.lock()->m_nTag;
	Vector3 vMonsterPos{};
	switch (iMonsterTag)
	{
	case GAMEOBJECTTAG::Monster1000:
		vMonsterPos = m_pLetMeFlyMonster.lock()->GetComponent<Transform>().lock()->GetPosition();
		break;
	case GAMEOBJECTTAG::Monster5000:
		vMonsterPos = m_pLetMeFlyMonster.lock()->GetMonsterBoneWorldPos("Hip");
		break;
	case GAMEOBJECTTAG::MonsterSnatchPoint:
		vMonsterPos = m_pLetMeFlyMonster.lock()->GetComponent<Transform>().lock()->GetPosition();
		{
			Vector3 vMonsterPos = m_pLetMeFlyMonster.lock()->GetComponent<Transform>().lock()->GetPosition();
			Vector3 vMyPos = m_pTransform.lock()->GetPosition();

			Vector3 vDir = vMonsterPos - vMyPos;
			vDir.y = 0;
			D3DXVec3Normalize(&vDir, &vDir);

			Vector3 vLook = -m_pTransform.lock()->GetLook();

			float fDot = D3DXVec3Dot(&vDir, &vLook);

			if (fDot > 1.f)
				fDot = 1.f - FLT_EPSILON;
			else if (fDot < -1.f)
				fDot = -1.f + FLT_EPSILON;

			float fRadian = acosf(fDot);

			Vector3	vCross;
			D3DXVec3Cross(&vCross, &vLook, &vDir);

			if (vCross.y > 0)
				fRadian *= -1;
			m_fAngle += -D3DXToDegree(fRadian) + vRotationDegree.y;
			vDegree.y = m_fAngle;
			vRotationDegree.y = m_fRotationAngle = 0;
			Reset_RootRotation();
		}
		break;
	default:
		vMonsterPos = m_pLetMeFlyMonster.lock()->GetMonsterBoneWorldPos("Hip");
		break;
	}

	Vector3 vMyPos = m_pTransform.lock()->GetPosition();

	Vector3 vDir = vMonsterPos - vMyPos;

	D3DXVec3Normalize(&vDir, &vDir);

	//Y흔들기
	vDir.y += FMath::Random<float>(-0.1f, 0.1f);


	m_pTransform.lock()->Translate(vDir * m_fFlySpeed);

}

void Nero::DecreaseDistance(float _GoalDis, float _fDeltaTime)
{
	if (m_pCamera.expired())
		return;
	m_pCamera.lock()->DecreaseDistance(_GoalDis, _fDeltaTime);
}

void Nero::IncreaseDistance(float _GoalDis, float _fDeltaTime)
{
	if (m_pCamera.expired())
		return;
	m_pCamera.lock()->IncreaseDistance(_GoalDis, _fDeltaTime);
}

void Nero::IncreaseHp(int _Hp)
{
	m_BattleInfo.iHp += _Hp;

	if (m_BattleInfo.iHp >= m_BattleInfo.iMaxHp)
		m_BattleInfo.iHp = m_BattleInfo.iMaxHp;

	float fHpRatio = float(float(m_BattleInfo.iHp) / float(m_BattleInfo.iMaxHp));
	if (!m_pBtlPanel.expired())
		m_pBtlPanel.lock()->SetPlayerHPRatio(fHpRatio);
}

void Nero::IncreaseMaxHp(const int _Amount)
{
	if (0 >= _Amount)
		return;

	/*
	MaxHP 100 기준 5칸
	한 칸 = 20
	*/

	m_BattleInfo.iMaxHp += _Amount;
	m_BattleInfo.iHp += _Amount;

	float fHpRatio = float(float(m_BattleInfo.iHp) / float(m_BattleInfo.iMaxHp));
	if (!m_pBtlPanel.expired())
		m_pBtlPanel.lock()->SetPlayerHPRatio(fHpRatio);
}

float Nero::Get_ExGauge()
{
	if (!m_pBtlPanel.expired())
		return m_pBtlPanel.lock()->GetExGauge();
	return 0.f;
}

uint32 Nero::Get_ExGaugeCount()
{
	if (!m_pBtlPanel.expired())
		return m_pBtlPanel.lock()->GetExGaugeCount();
	return 0;
}

void Nero::Add_ExGauge(float ExGauge)
{
	if (!m_pBtlPanel.expired())
		m_pBtlPanel.lock()->AddExGauge(ExGauge);
}

void Nero::Use_ExGauge(const uint32 Count)
{
	if (!m_pBtlPanel.expired())
		m_pBtlPanel.lock()->UseExGauge(Count);
}

float Nero::Get_TDTGauge()
{
	if (!m_pBtlPanel.expired())
		return m_pBtlPanel.lock()->GetTDTGauge();
	return 0.f;
}

void Nero::AccumulateTDTGauge(const float Amount)
{
	if (!m_pBtlPanel.expired())
		m_pBtlPanel.lock()->AccumulateTDTGauge(Amount);
}

void Nero::ConsumeTDTGauge(const float Speed)
{
	if (!m_pBtlPanel.expired())
		m_pBtlPanel.lock()->ConsumeTDTGauge(Speed);
}

void Nero::AddRankScore(float Score)
{
	if (!m_pBtlPanel.expired())
		m_pBtlPanel.lock()->AddRankScore(Score);
}

void Nero::BuyUpgradedOverture()
{
	m_pOverture.lock()->GetComponent<SphereCollider>().lock()->SetRadius(0.27f);
	m_pEffOverture.lock()->GetComponent<Transform>().lock()->SetScale({ 0.002f, 0.002f, 0.002f });
}

void Nero::StopAnimation()
{
	m_pMesh[m_iMeshIndex]->StopAnimation();
}

void Nero::ContinueAnimiation()
{
	m_pMesh[m_iMeshIndex]->ContinueAnimation();
}

bool Nero::IsAnimationEnd()
{
	return m_pMesh[m_iMeshIndex]->IsAnimationEnd();
}

void Nero::ChangeNeroDirection(UINT _NeroDirection)
{
	m_iPreDirIndex = m_iCurDirIndex;
	m_iCurDirIndex = _NeroDirection;
}

void Nero::Change_To_MajinMode()
{
	m_IsMajin = true;
	for (auto pMonster : GetAliveMonster())
	{
		BT_INFO _Info;
		ZeroMemory(&_Info, sizeof(BT_INFO));
		_Info.eAttackType = Attack_KnocBack;
		pMonster.lock()->Hit(_Info);
	}
	if (!m_pBtlPanel.expired())
		m_pBtlPanel.lock()->ConsumeTDTGauge(0.1f);
}

void Nero::ChangeAnimation(const std::string& InitAnimName, const bool bLoop, const UINT AnimationIndex, const AnimNotify& _Notify, const bool bOverlap)
{
	m_iPreAnimationIndex = m_iCurAnimationIndex;
	m_iCurAnimationIndex = AnimationIndex;
	m_pMesh[m_iMeshIndex]->PlayAnimation(InitAnimName, bLoop, _Notify, 1.f, 1.f, bOverlap);
}

void Nero::ChangeAnimation_Weapon(NeroComponentID _eNeroComID, const std::string& InitAnimName, const bool bLoop, const AnimNotify& _Notify, const bool bOverlap)
{
	switch (_eNeroComID)
	{
	case Nero::NeroCom_BusterArm:
		m_pBusterArm.lock()->SetActive(true);
		m_pBusterArm.lock()->ChangeAnimation(InitAnimName, bLoop, _Notify);
		break;
	case Nero::NeroCom_BusterArm_Left:
		m_pBusterArmLeft.lock()->SetActive(true);
		m_pBusterArmLeft.lock()->ChangeAnimation(InitAnimName, bLoop, _Notify);
		break;
	case Nero::NeroCom_WireArm:
		m_pWireArm.lock()->ChangeAnimation(InitAnimName, bLoop, _Notify);
		break;
	case Nero::NeroCom_WIngArm_Left:
		m_pWingArm_Left.lock()->SetActive(true);
		m_pWingArm_Left.lock()->ChangeAnimation(InitAnimName, bLoop, _Notify);
		break;
	case Nero::NeroCom_WingArm_Right:
		m_pWingArm_Right.lock()->SetActive(true);
		m_pWingArm_Right.lock()->ChangeAnimation(InitAnimName, bLoop, _Notify);
		break;
	case Nero::NeroCom_Overture:
		m_pOverture.lock()->ChangeAnimation(InitAnimName, bLoop, _Notify);
		break;
	case Nero::NeroCom_RedQueen:
		break;
	case Nero::NeroCom_Cbs_Short:
		ChangeWeapon(Nero::NeroCom_Cbs_Short);
		m_pCbsShort.lock()->ChangeAnimation(InitAnimName, bLoop, _Notify, bOverlap);
		break;
	case Nero::NeroCom_Cbs_Middle:
		ChangeWeapon(Nero::NeroCom_Cbs_Middle);
		m_pCbsMiddle.lock()->ChangeAnimation(InitAnimName, bLoop, _Notify, bOverlap);
		break;
	case Nero::NeroCom_JudgementSword:
		m_pJudgementSword.lock()->SetActive(true);
		m_pJudgementSword.lock()->ChangeAnimation(InitAnimName, bLoop, _Notify);
		break;
	case Nero::NeroCom_JudgementShadow1:
		m_pJudgementShadow1.lock()->SetActive(true);
		m_pJudgementShadow1.lock()->ChangeAnimation(InitAnimName, bLoop, _Notify);
		break;
	case Nero::NeroCom_JudgementShadow2:
		m_pJudgementShadow2.lock()->SetActive(true);
		m_pJudgementShadow2.lock()->ChangeAnimation(InitAnimName, bLoop, _Notify);
		break;
	case Nero::NeroCom_JudgementShadow3:
		m_pJudgementShadow3.lock()->SetActive(true);
		m_pJudgementShadow3.lock()->ChangeAnimation(InitAnimName, bLoop, _Notify);
		break;
	case Nero::NeroCom_End:
		break;
	default:
		break;
	}
}

void Nero::ChangeWeapon(NeroComponentID _iWeaponIndex)
{
	m_iCurWeaponIndex = _iWeaponIndex;

	switch (m_iCurWeaponIndex)
	{
	case Nero::NeroCom_RedQueen:
		m_pCbsShort.lock()->SetActive(false);
		m_pCbsMiddle.lock()->SetActive(false);
		m_pCbsLong.lock()->SetActive(false);
		m_pCbsTrail.lock()->PlayEnd();
		if (m_pRedQueen.lock()->IsActive())
			return;
		m_pRedQueen.lock()->SetActive(true);
		m_pRedQueen.lock()->GetComponent<CapsuleCollider>().lock()->SetActive(false);
		if (!m_pBtlPanel.expired())
			m_pBtlPanel.lock()->ChangeWeaponUI(RQ);
		break;
	case Nero::NeroCom_Cbs_Short:
		m_pRedQueen.lock()->SetActive(false);
		m_pCbsMiddle.lock()->SetActive(false);
		m_pCbsLong.lock()->SetActive(false);
		m_pTrail.lock()->SetActive(false);
		if (m_pCbsShort.lock()->IsActive())
			return;
		m_pCbsShort.lock()->SetActive(true);
		m_pCbsShort.lock()->GetComponent<SphereCollider>().lock()->SetActive(false);
		if (!m_pBtlPanel.expired())
			m_pBtlPanel.lock()->ChangeWeaponUI(Cbs, 0);
		break;
	case Nero::NeroCom_Cbs_Middle:
		m_pRedQueen.lock()->SetActive(false);
		m_pCbsShort.lock()->SetActive(false);
		m_pCbsLong.lock()->SetActive(false);
		m_pTrail.lock()->SetActive(false);
		if (m_pCbsMiddle.lock()->IsActive())
			return;
		m_pCbsMiddle.lock()->SetActive(true);
		m_pCbsMiddle.lock()->GetComponent<SphereCollider>().lock()->SetActive(false);
		if (!m_pBtlPanel.expired())
			m_pBtlPanel.lock()->ChangeWeaponUI(Cbs, 1);
		break;
	case Nero::NeroCom_Cbs_Long:
		m_pRedQueen.lock()->SetActive(false);
		m_pCbsShort.lock()->SetActive(false);
		m_pCbsMiddle.lock()->SetActive(false);
		m_pTrail.lock()->SetActive(false);
		if (m_pCbsLong.lock()->IsActive())
			return;
		m_pCbsLong.lock()->SetActive(true);
		m_pCbsLong.lock()->GetComponent<CapsuleCollider>().lock()->SetActive(false);
		if (!m_pBtlPanel.expired())
			m_pBtlPanel.lock()->ChangeWeaponUI(Cbs, 2);
		break;
	}
}

void Nero::ChangeWeaponUI(NeroComponentID _iWeaponIndex)
{
	if (Nero::NeroCom_RedQueen == _iWeaponIndex)
	{
		if (!m_pBtlPanel.expired())
			m_pBtlPanel.lock()->ChangeWeaponUI(RQ);
	}
	else
	{
		if (!m_pBtlPanel.expired())
			m_pBtlPanel.lock()->ChangeWeaponUI(Cbs);
	}
}

void Nero::ChangeWeaponCollSize(float _fSize)
{
	switch (m_iCurWeaponIndex)
	{
	case NeroCom_Cbs_Short:
		m_pCbsShort.lock()->ChangeColliderSize(_fSize);
		break;
	case NeroCom_Cbs_Middle:
		m_pCbsMiddle.lock()->ChangeColliderSize(_fSize);
		break;
	case NeroCom_Cbs_Long:
		m_pCbsLong.lock()->ChangeColliderSize(_fSize);
		break;
	}
}

void Nero::ChangeNewSword(UINT _eAniList, bool _bLoop, bool _Overlap)
{
	m_pNewWingSword.lock()->SetActive(true);
	m_pNewWingSword.lock()->m_DissolveInfo.DissolveEnd();
	m_pNewWingSword.lock()->m_DissolveInfo.DissolveStart(true, false, 0.1f);
	m_pNewWingSword.lock()->m_bDissovleOnce = true;
	m_pNewWingSword.lock()->ChangeAnimation(_eAniList, _bLoop, _Overlap);
}

void Nero::PlayEffect(GAMEOBJECTTAG _eTag, const Vector3& Rotation, const float CurRoll,
	const int32 StartSpriteRow, const float PlayTime, const Vector3& Scale)
{
	Vector3 vMyPos = m_pTransform.lock()->GetPosition();
	switch (_eTag)
	{
	case Eff_AirHike:
		m_pAirHike.lock()->PlayStart(vMyPos);
		break;
	case Eff_Trail:
		if (!m_pBtlPanel.expired())
		{
			if (0 < m_pBtlPanel.lock()->GetExGaugeCount())
				m_pTrail.lock()->PlayStart(Trail::Mode::Explosion);
			else
				m_pTrail.lock()->PlayStart(Trail::Mode::Non);
		}
		break;
	case Eff_FireCircle:
		switch (StartSpriteRow)
		{
		case 1://젤 밝은거
			m_pFireCircle[0].lock()->PlayStart(Rotation, vMyPos + Vector3{ 0.f,0.1f,0.f }, CurRoll, FMath::PI * 4.f, StartSpriteRow, PlayTime, 0, Scale);
			break;
		case 3://그 담으로 밝은거
			m_pFireCircle[1].lock()->PlayStart(Rotation, vMyPos + Vector3{ 0.f,0.1f,0.f }, CurRoll, FMath::PI * 4.f, StartSpriteRow, PlayTime, 0, Scale);
			break;
		case 5://그 담으로 밝은거
			m_pFireCircle[2].lock()->PlayStart(Rotation, vMyPos + Vector3{ 0.f,0.1f,0.f }, CurRoll, FMath::PI * 4.f, StartSpriteRow, PlayTime, 0, Scale);
			break;
		}
		break;
	case Eff_IceAge:
		m_pIceAge.lock()->PlayStart(vMyPos);
		break;
	case Eff_CircleWave:
		m_pCircleWave.lock()->PlayStart(GScale, vMyPos);
		break;
	case Eff_DashTrail:
		break;
	case Eff_ShapeParticle:	// 오브 획득시 이펙트
		if (1.f < CurRoll)
		{
			if (!m_pShapeParticle[SP_RED].lock()->IsPlaying())
			{
				SoundSystem::GetInstance()->Play("GetOrb1", 0.6f, false);
				m_pShapeParticle[SP_RED].lock()->PlayStart(5.f);
			}
		}
		else if (0.f < CurRoll)
		{
			if (!m_pShapeParticle[SP_GREEN].lock()->IsPlaying())
				m_pShapeParticle[SP_GREEN].lock()->PlayStart(5.f);
		}
		else
		{
			if (!m_pShapeParticle[SP_WHITE].lock()->IsPlaying())
				m_pShapeParticle[SP_WHITE].lock()->PlayStart(5.f);
		}
		break;
	case Eff_CbsTrail:
		if (Nero::ANI_CBS_SKILL_ICEAGE_END <= m_iCurAnimationIndex
			&& m_iCurAnimationIndex <= Nero::ANI_CBS_SKILL_ICEAGE_START)
			m_pCbsTrail.lock()->PlayStart(CbsTrail::IceAge);
		else
			m_pCbsTrail.lock()->PlayStart(CbsTrail::Non);
		break;
	case Eff_CbsMidTrail:
		m_pCbsMidTrail.lock()->PlayStart(CbsMidTrail::Non);
		break;
	case Eff_CbsLongTrail:
		m_pCbsLongTrail.lock()->PlayStart();
		break;
	case Eff_BiAttack:
		m_pBiAttack.lock()->PlayStart(vMyPos, -m_pTransform.lock()->GetLook());
		break;
	case Eff_BlitzAttack:
		m_pBlitzAttack.lock()->PlayStart(vMyPos);
		break;
	case Eff_LongBarrel:
	{
		Vector3 vDir = -m_pTransform.lock()->GetLook();
		Matrix RotX;
		D3DXMatrixRotationX(&RotX, Rotation.x);
		D3DXVec3TransformCoord(&vDir, &vDir, &RotX);
		m_pLongBarrel.lock()->PlayStart(vMyPos, vDir);
	}
	break;
	case Eff_Satellite:
		m_pSatellite.lock()->PlayStart(vMyPos, -m_pTransform.lock()->GetLook());
		break;
	case Eff_WhirlWind:
		vMyPos.y += 0.2f;
		m_pWhirlWind.lock()->PlayStart(vMyPos, -m_pTransform.lock()->GetLook());
		break;
	case Eff_Change:
		m_pChange.lock()->PlayStart(vMyPos);
		break;
	case Eff_SnatchRush:
		m_pShockWave.lock()->PlayStart(vMyPos, ShockWave::Option::SnatchRush);
		break;
	case Eff_Streak:
		m_pShockWave.lock()->PlayStart(vMyPos, ShockWave::Option::Streak);
		break;
	case Eff_Streak_End:
		m_pShockWave.lock()->PlayStart(vMyPos, ShockWave::Option::StreakEnd);
		break;
	case Eff_SplitEnd:
		m_pShockWave.lock()->PlayStart(vMyPos, ShockWave::Option::SplitEnd);
		break;
	case Eff_Split:
		m_pShockWave.lock()->PlayStart(vMyPos, ShockWave::Option::Split);
		break;
	case Eff_Buster:
		m_pShockWave.lock()->PlayStart(vMyPos, ShockWave::Option::Buster);
		break;
	case Eff_Judgement:
		vMyPos.y += 0.005f;
		m_pEffJudgement.lock()->PlayStart(vMyPos);
		break;
	case Eff_JudgementSwordTrail:
		m_pJudgementTrail.lock()->PlayStart(JudgementSwordTrail::Non);
		break;
	default:
		break;
	}
}

void Nero::StopEffect(GAMEOBJECTTAG _eTag)
{
	switch (_eTag)
	{
	case Eff_AirHike:
		m_pAirHike.lock()->PlayEnd();
		break;
	case Eff_Trail:
		m_pTrail.lock()->PlayEnd();
		break;
	case Eff_FireCircle:
		for (int i = 0; i < 3; ++i)
		{
			m_pFireCircle[i].lock()->PlayEnd();
		}
		break;
	case Eff_IceAge:
		m_pIceAge.lock()->PlayEnd();
		break;
	case Eff_CircleWave:
		m_pCircleWave.lock()->PlayEnd();
		break;
	case Eff_DashTrail:
		break;
	case Eff_CbsTrail:
		m_pCbsTrail.lock()->PlayEnd();
		break;
	case Eff_ShapeParticle:
		m_pShapeParticle[SP_GREEN].lock()->Reset();
		m_pShapeParticle[SP_RED].lock()->Reset();
		break;
	case Eff_CbsMidTrail:
		m_pCbsMidTrail.lock()->PlayEnd();
		break;
	case Eff_CbsLongTrail:
		m_pCbsLongTrail.lock()->PlayEnd();
		break;
	case Eff_Satellite:
		m_pSatellite.lock()->PlayEnd();
		break;
	case Eff_Change:
		m_pChange.lock()->PlayEnd();
		break;
	case Eff_JudgementSwordTrail:
		m_pJudgementTrail.lock()->PlayEnd();
		break;
	case Tag_END:
		m_pTrail.lock()->PlayEnd();
		m_pIceAge.lock()->PlayEnd();
		m_pCbsTrail.lock()->PlayEnd();
		m_pCbsMidTrail.lock()->PlayEnd();
		m_pSatellite.lock()->PlayEnd();
		m_pCbsLongTrail.lock()->PlayEnd();
		break;
	default:
		break;
	}
}

void Nero::PlayStone(GAMEOBJECTTAG _eTag)
{
	float fRandom = FMath::Random<float>(0.0005f, 0.0008f);
	Vector3 vRot = FMath::Random<Vector3>(Vector3{ 0.f,0.f,0.f }, Vector3{ 180.f,180.f,180.f });
	Vector3 vPos = m_pTransform.lock()->GetPosition();
	Vector3 vLook = -m_pTransform.lock()->GetLook();
	vPos.y -= 0.05f;
	vLook.y = 0.f;
	switch (_eTag)
	{
	case GAMEOBJECTTAG::TAG_RedQueen:
		
		vPos += vLook * 0.3f;
		for (int i = 0; i < 8; ++i)
		{
			m_pStone[i].lock()->SetPosition(vPos);
			m_pStone[i].lock()->SetScale(fRandom);
			m_pStone[i].lock()->PlayStart(40.f);
			m_pStone[i].lock()->SetRotation(vRot);
		}
		break;
	case GAMEOBJECTTAG::TAG_BusterArm_Right:
		for (int i = 0; i < 8; ++i)
		{
			m_pStone[i].lock()->SetPosition(vPos);
			m_pStone[i].lock()->SetScale(fRandom);
			m_pStone[i].lock()->PlayStart(40.f);
			m_pStone[i].lock()->SetRotation(vRot);
		}
		break;
	}
}
