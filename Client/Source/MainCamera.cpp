#include "stdafx.h"
#include "MainCamera.h"
#include "Nero.h"
#include "TimeSystem.h"
#include "NeroFSM.h"
#include "Trigger.h"
#include "Renderer.h"
#include "QliphothBlock.h"
#include "Monster.h"
#include "NuClear.h"

UINT MainCamera::m_eAtType = AT_PLAYER;
UINT MainCamera::m_ePlayerCamMode = CAM_MODE_BASIC;
UINT MainCamera::m_eTriggerCamMode = STAGE1_WAVE1;

MainCamera::MainCamera()
	:m_fFovY(0.f), m_fAspect(0.f), m_fNear(0.f), m_fFar(0.f), 
	m_fDistanceToTarget(0.f),m_bFix(true)
{
	D3DXMatrixIdentity(&m_matProj);
	D3DXMatrixIdentity(&m_matView);
	m_vEye = { 0.f,0.f,0.f };
	m_vAt = { 0.f,0.f,0.f };
	m_vUp = { 0.f,1.f,0.f };
	m_vTriggerPos = { 0.f,0.f,0.f };
	m_vTriggerAngle = { 0.f,0.f,0.f };
	m_vLerpEye = m_vAt = { 0.f,0.f,0.f };
	m_nTag = TAG_Camera;
}

MainCamera::~MainCamera()
{
}

void MainCamera::Free()
{
	GameObject::Free();
}

MainCamera* MainCamera::Create()
{
	return new MainCamera;
}

HRESULT MainCamera::Ready()
{
	m_fFovY = D3DXToRadian(45.f);
	m_fAspect = float(float(g_nWndCX) / float(g_nWndCY));
	m_fNear = 0.05f;
	m_fFar = 15.f;


	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	Graphic::GetDevice()->SetTransform(D3DTS_VIEW, &m_matView);

	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFovY, m_fAspect, m_fNear, m_fFar);
	Graphic::GetDevice()->SetTransform(D3DTS_PROJECTION, &m_matProj);

	m_fDistanceToTarget = OGDistance;
	//m_fDistanceToTarget = 1.f;
	m_fRotX = -17.1f;
	m_fFloatingAmount = 0.19f;
	m_fSensitive = 15.f;
	m_fDecreaseFactor = 0.6f;
	m_fIncreaseFactor = 0.07f;
	m_fLerpSpeed = 1.5f;

	return S_OK;
}

HRESULT MainCamera::Awake()
{
	m_pNero = std::static_pointer_cast<Nero>(FindGameObjectWithTag(Player).lock());
	m_pAtTranform = FindGameObjectWithTag(Player).lock()->GetComponent<Transform>();
	m_eAtType = AT_PLAYER;
	m_ePlayerCamMode = CAM_MODE_BASIC;

	m_vAt = m_pAtTranform.lock()->GetPosition();

	return S_OK;
}

HRESULT MainCamera::Start()
{
	return S_OK;
}

UINT MainCamera::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
	CheckFadeOut();
	if (Input::GetKeyDown(DIK_RSHIFT))
		m_bFix = !m_bFix;
	Mouse_Fix();
	switch (m_eAtType)
	{
	case AT_PLAYER:
		MoveMent_Player(_fDeltaTime);
		break;
	case AT_TRIGGER:
		MoveMent_Trigger(_fDeltaTime);
		break;
	case AT_BOSS1:
		Boss_Cam_Em5000(_fDeltaTime);
		break;
	case AT_BOSS2:
		Boss_Cam_Em5300(_fDeltaTime);
		break;
	default:
		break;
	}
	return 0;
}

UINT MainCamera::LateUpdate(const float _fDeltaTime)
{
	Vector3 PlayerPos = m_pNero.lock()->GetComponent<Transform>().lock()->GetPosition();
	Vector3 vLook = m_vEye - PlayerPos;
	Vector3 vCollEye{};
	if (m_bCollEnable)
	{
		switch (m_eAtType)
		{
		case AT_PLAYER:
			if (CAM_MODE_RETURN_TO_PLAYER == m_ePlayerCamMode)
				return 0;
			if (Physics::RayCast(PlayerPos, vLook, vCollEye))
			{
				Vector3 vLength1 = PlayerPos - vCollEye;
				Vector3 vLength2 = PlayerPos - m_vEye;

				if (D3DXVec3Length(&vLength1) <= D3DXVec3Length(&vLength2))
				{
					m_vEye.x = vCollEye.x;
					m_vEye.z = vCollEye.z;
				}
			}
			break;
		case AT_BOSS1:
			if (Physics::RayCast(PlayerPos, vLook, vCollEye))
			{
				Vector3 vLength1 = PlayerPos - vCollEye;
				Vector3 vLength2 = PlayerPos - m_vEye;

				float fLength1 = D3DXVec3Length(&vLength1);
				float fLength2 = D3DXVec3Length(&vLength2);
				if (fLength1 <= fLength2)
				{
					if (fLength1 <= 0.28f)
					{
						std::weak_ptr<Transform>	_PlayerTransform = m_pNero.lock()->GetComponent<Transform>();
						Vector3 vLook = _PlayerTransform.lock()->GetPosition() - m_vAt;
						vLook.y = 0.f;
						D3DXVec3Normalize(&vLook, &vLook);

						m_vLerpEye = _PlayerTransform.lock()->GetPosition() + vLook * 0.33f;
						m_vLerpEye.y += 0.2f;
						m_vEye = m_vLerpEye;
					}
					else
					{
						m_vEye.x = vCollEye.x;
						m_vEye.z = vCollEye.z;
					}
				}
			}
			break;
		default:
			break;
		}
	}
	Shaking();

	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	Graphic::GetDevice()->SetTransform(D3DTS_VIEW, &m_matView);

	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFovY, m_fAspect, m_fNear, m_fFar);
	Graphic::GetDevice()->SetTransform(D3DTS_PROJECTION, &m_matProj);

	return 0;
}

void MainCamera::OnEnable()
{
	GameObject::OnEnable();
}

void MainCamera::OnDisable()
{
	GameObject::OnDisable();
}

void MainCamera::Set_PlayerCamMode(UINT _ePlayerCamMode)
{
	m_ePlayerCamMode = _ePlayerCamMode;
	if (CAM_MODE_WAVE_END == m_ePlayerCamMode)
	{
		m_fDistanceToTarget = 0.15f;
		vector<Vector3> _LostTimes;
		_LostTimes.emplace_back(Vector3(1.5f, 1.f, 0.3f));
		TimeSystem::GetInstance()->LostTime(_LostTimes);
		
		m_fTriggerTime = 2.5f;
		m_fLerpSpeed = 1.3f;
		
		m_fFadeOutDelayTime = 0.5f;
		m_bPlayOnce = true;
	}
}

void MainCamera::Set_At_Transform(std::weak_ptr<Transform> _pTransform, UINT _eAtType)
{
	m_pAtTranform = _pTransform;
	m_eAtType = _eAtType;
}

void MainCamera::Set_TriggerCam(UINT _eTriggerCamMode, const Vector3& _vTriggerPos, const float& _fTriggerTime)
{
	m_eAtType = AT_TRIGGER;
	m_eTriggerCamMode = _eTriggerCamMode;

	m_vTriggerPos = _vTriggerPos;
	m_fTriggerTime = _fTriggerTime;
	m_vTriggerAngle = { 0.f,0.f,0.f };
	switch (_eTriggerCamMode)
	{
	case STAGE1_WAVE1:
		m_vEye.y -= 0.25f;
		break;
	case STAGE1_WAVE1_END:
		m_vEye = { 0.04f,0.162f,-1.768f };
		break;
	case STAGE2_BUTTERFLY1:
		m_vEye = { -4.094f,0.79f,14.14f };
		break;
	case STAGE4_BOSS_CUTSCENE:
		m_vAt = m_vTriggerPos;
		break;
	}
}

void MainCamera::SetAngle(const Vector3& _vAngle)
{
	m_fRotX = _vAngle.x;
	m_fAngle = _vAngle.y;
}

void MainCamera::SetShakeInfo(float _fShakeTime, float _fShakePower)
{
	m_fShakingTime = _fShakeTime;
	m_fShakePower = _fShakePower;
}

void MainCamera::SetFadeSceneInfo(float _fFadeInAmout)
{
	m_bFadeOut = true;
	m_fFadeInAmout = _fFadeInAmout;
}

void MainCamera::SetStartPos()
{
	m_vAt = m_pAtTranform.lock()->GetPosition();
	m_vAt.y += m_fFloatingAmount;

	Vector3 vLook = { 0.f, 0.f ,1.f };
	Vector3 vUp = Vector3(0.f, 1.f, 0.f);
	Matrix matRot, matTest;

	vLook *= m_fDistanceToTarget;

	D3DXMatrixRotationX(&matTest, D3DXToRadian(m_fRotX));
	D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(m_fAngle));
	D3DXVec3TransformNormal(&vLook, &vLook, &matTest);
	D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

	m_vLerpEye = m_vAt + vLook;

	m_vEye = m_vLerpEye;
}

void MainCamera::CalcEm5300NeroAngle()
{
	Vector3 vPlayerPos = m_pNero.lock()->GetComponent<Transform>().lock()->GetPosition();
	Vector3 vMonsterPos = static_pointer_cast<Monster>(FindGameObjectWithTag(GAMEOBJECTTAG::Monster5300).lock())->GetMonsterBoneWorldPos("Hip");

	Vector3 vLook = -m_pNero.lock()->GetComponent<Transform>().lock()->GetLook();
	Vector3 vDir = vMonsterPos - vPlayerPos;
	vDir.y = 0;
	D3DXVec3Normalize(&vDir, &vDir);

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
	m_fAngle = -D3DXToDegree(fRadian);
	m_fAngle += 45.f;
}

void MainCamera::DecreaseDistance(float _GoalDis, float _fDeltaTime)
{
	//if (m_fDistanceToTarget <= _GoalDis)
	//{
	//	m_fDecreaseFactor = 0.6f;
	//	return;
	//}
	//if (m_fDecreaseFactor >= 0.02f)
	//	m_fDecreaseFactor -= 0.02f;
	//m_fIncreaseFactor = 0.07f;
	//m_fDistanceToTarget -= m_fDecreaseFactor * _fDeltaTime;
}

void MainCamera::IncreaseDistance(float _GoalDis, float _fDeltaTime)
{
	//if (m_fDistanceToTarget >= _GoalDis)
	//{
	//	m_fIncreaseFactor = 0.07f;
	//	return;
	//}
	//m_fDecreaseFactor = 0.6f;
	//m_fIncreaseFactor += 0.005f;
	//m_fDistanceToTarget += m_fIncreaseFactor * _fDeltaTime;
}

void MainCamera::ControlDistance(float _Amount, float _Max)
{
	m_fDistanceToTarget += _Amount;
	if (m_fDistanceToTarget >= _Max)
		m_fDistanceToTarget = _Max;
}

std::string MainCamera::GetName()
{
	return "MainCamera";
}

void MainCamera::Editor()
{
	GameObject::Editor();
	static bool MyButton = true;
	static float ZeroDotOne = 0.1f;
	if (bEdit)
	{
		ImGui::InputScalar("Distance", ImGuiDataType_Float, &m_fDistanceToTarget, MyButton ? &ZeroDotOne : NULL);
		ImGui::InputScalar("Sensitive", ImGuiDataType_Float, &m_fSensitive, MyButton ? &ZeroDotOne : NULL);
		ImGui::InputScalar("RotX", ImGuiDataType_Float, &m_fRotX, MyButton ? &ZeroDotOne : NULL);
		ImGui::InputScalar("FloatingAmount", ImGuiDataType_Float, &m_fFloatingAmount, MyButton ? &ZeroDotOne : NULL);
		ImGui::InputScalar("m_fNear", ImGuiDataType_Float, &m_fNear, MyButton ? &ZeroDotOne : NULL);
		ImGui::InputScalar("m_fFar", ImGuiDataType_Float, &m_fFar, MyButton ? &ZeroDotOne : NULL);
		ImGui::InputScalar("m_fAngle", ImGuiDataType_Float, &m_fAngle, MyButton ? &ZeroDotOne : NULL);
		ImGui::InputScalar("m_fShakingTime", ImGuiDataType_Float, &m_fShakingTime, MyButton ? &ZeroDotOne : NULL);
		ImGui::InputScalar("m_fShakePower", ImGuiDataType_Float, &m_fShakePower, MyButton ? &ZeroDotOne : NULL);
	}
}

void MainCamera::Mouse_Fix()
{
	if (!m_bFix)
		return;
	POINT   ptMouse{ g_nWndCX >> 1, g_nWndCY >> 1 };

	ClientToScreen(g_hWnd, &ptMouse);
	SetCursorPos(ptMouse.x, ptMouse.y);
}

void MainCamera::Shaking()
{
	float fOriginDelta = TimeSystem::GetInstance()->OriginDeltaTime();
	if (m_fShakingTime < 0.f)
	{
		m_fShakePower = FMath::Lerp(m_fShakePower, 0.f, fOriginDelta);
		if(fabs(m_fShakePower) <= FLT_EPSILON)
			return;
	}

	m_fShakingTime -= fOriginDelta;
	static int Test = 2;
	Test -= 1;
	if (Test == 0)
	{
		m_fShakePower *= -1.f;
		Test = 2;
	}

	Vector3 vShake = { 0.001f,0.f,0.001f };
	m_vEye += vShake * m_fShakePower;
	m_vAt += vShake * m_fShakePower;
}

void MainCamera::CheckFadeOut()
{
	if (!m_bFadeOut)
		return;
	
	if (Renderer::GetInstance()->IsBlackOut())
	{
		m_bFadeOut = false;
		if(!m_pTrigger.expired())
			m_pTrigger.lock()->TriggerEnable();
		Renderer::GetInstance()->FadeInStart(m_fFadeInAmout);
	}
}

void MainCamera::MoveMent_Player(float _fDeltaTime)
{
	switch (m_ePlayerCamMode)
	{
	case CAM_MODE_BASIC:
		Player_Cam_Baisc(_fDeltaTime);
		break;
	case CAM_MODE_WAVE_END:
		Player_Cam_WaveEnd(_fDeltaTime);
		break;
	case CAM_MODE_RETURN_TO_PLAYER:
		Player_Cam_ReturnToPlayer(_fDeltaTime);
		break;
	default:
		break;
	}
}

void MainCamera::Player_Cam_Baisc(float _fDeltaTime)
{
	if (m_fLerpSpeed <= 1.5f)
		m_fLerpSpeed += _fDeltaTime * 0.05f;
	m_vAt = m_pAtTranform.lock()->GetPosition();
	m_vAt.y += m_fFloatingAmount;

	long    dwMouseMove = 0;

	if (dwMouseMove = Input::GetMouseMove(DIM_Y))
	{
		m_fRotX -= dwMouseMove / m_fSensitive;
	}

	if (dwMouseMove = Input::GetMouseMove(DIM_X))
	{
		m_fAngle += dwMouseMove / m_fSensitive;
	}

	if (dwMouseMove = Input::GetMouseMove(DIM_Z))
	{
		
		//m_fDistanceToTarget -= dwMouseMove / 100.f;
	}
	if (m_fRotX <= -50.f)
		m_fRotX = -50.f;
	if (m_fDistanceToTarget <= 0.8f)
	{
		if (m_fRotX >= 13.5f)
			m_fRotX = 13.5f;
	}
	else
	{
		if (m_fRotX >= 5.5f)
			m_fRotX = 5.5f;
	}


	Vector3 vLook = { 0.f, 0.f ,1.f };
	Vector3 vUp = Vector3(0.f, 1.f, 0.f);
	Matrix matRot, matTest;

	vLook *= m_fDistanceToTarget;

	D3DXMatrixRotationX(&matTest, D3DXToRadian(m_fRotX));
	D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(m_fAngle));
	D3DXVec3TransformNormal(&vLook, &vLook, &matTest);
	D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

	m_vLerpEye = m_vAt + vLook;

	if (m_bLerp)
		m_vEye = FMath::Lerp(m_vEye, m_vLerpEye, _fDeltaTime * m_fLerpSpeed);
	else
		m_vEye = m_vLerpEye;

	
}

void MainCamera::Player_Cam_WaveEnd(float _fDeltaTime)
{
	m_fFadeOutDelayTime -= TimeSystem::GetInstance()->OriginDeltaTime();

	if (m_fFadeOutDelayTime <= 0.f && m_bPlayOnce)
	{
		m_bPlayOnce = false;
		Renderer::GetInstance()->FadeOutStart(1.f);
	}
	//���� �� �׾����� ���ο� �Ǵ� ī�޶� ó��
	m_vAt = m_pAtTranform.lock()->GetPosition();
	m_vAt.y += m_fFloatingAmount;

	long    dwMouseMove = 0;

	if (dwMouseMove = Input::GetMouseMove(DIM_Y))
	{
		m_fRotX -= dwMouseMove / m_fSensitive;
	}

	if (dwMouseMove = Input::GetMouseMove(DIM_X))
	{
		m_fAngle += dwMouseMove / m_fSensitive;
	}

	if (dwMouseMove = Input::GetMouseMove(DIM_Z))
	{
		//m_fDistanceToTarget -= dwMouseMove / 100.f;
	}
	if (m_fRotX <= -50.f)
		m_fRotX = -50.f;
	if (m_fRotX >= 13.5f)
		m_fRotX = 13.5f;


	Vector3 vLook = { 0.f, 0.f ,1.f };
	Vector3 vUp = Vector3(0.f, 1.f, 0.f);
	Matrix matRot, matTest;

	vLook *= m_fDistanceToTarget;

	D3DXMatrixRotationX(&matTest, D3DXToRadian(m_fRotX));
	D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(m_fAngle));
	D3DXVec3TransformNormal(&vLook, &vLook, &matTest);
	D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

	m_vLerpEye = m_vAt + vLook;

	m_vEye = FMath::Lerp(m_vEye, m_vLerpEye, _fDeltaTime * m_fLerpSpeed);
	m_fTriggerTime -= TimeSystem::GetInstance()->OriginDeltaTime();
	//������ �ð��� �ٵƴ�
	if (m_fTriggerTime <= 0.f)
	{
		switch (m_eTriggerCamMode)
		{
		case STAGE1_WAVE1:
			Renderer::GetInstance()->FadeInStart(0.4f);
			Set_TriggerCam(STAGE1_WAVE1_END, Vector3{ -1.081f,0.818f,0.439f }, 3.f);
			m_fDistanceToTarget = OGDistance;
			for (uint32 i = 1u; i < 4u; ++i)
			{
				if (i < m_vecQliphothBlock.size() && !m_vecQliphothBlock[i].expired())
				{
					m_vecQliphothBlock[i].lock()->Reset();
				}
			}
			break;
		case STAGE1_WAVE2_BATTLE:
			Renderer::GetInstance()->FadeInStart(0.4f);
			Set_TriggerCam(STAGE1_WAVE2_END, Vector3{ -3.6f,0.48f,11.8f }, 5.f);
			m_vAt = m_vTriggerPos;
			m_vEye = { -3.716f,0.485f,10.1f };
			m_fDistanceToTarget = OGDistance;
			for (uint32 i = 5u; i < 7u; ++i)
			{
				if (i < m_vecQliphothBlock.size() && !m_vecQliphothBlock[i].expired())
				{
					m_vecQliphothBlock[i].lock()->Reset();
				}
			}
			break;
		case STAGE3_WAVE_HOLE:
			Renderer::GetInstance()->FadeInStart(0.4f);
			Set_TriggerCam(STAGE3_WAVE_HOLE, Vector3{ 0.351f,0.497f,30.876f }, 2.3f);
			m_vAt = { 0.048f,0.482f,31.346f };
			m_vEye = { 0.176f,0.3f,33.161f };
			m_fDistanceToTarget = OGDistance;
			for (uint32 i = 0; i < 3u; ++i)
			{
				if (i < m_vecQliphothBlock.size() && !m_vecQliphothBlock[i].expired())
				{
					m_vecQliphothBlock[i].lock()->Reset();
				}
			}


			break;
		}
	}
}

void MainCamera::Player_Cam_ReturnToPlayer(float _fDeltaTime)
{

	Vector3 vPlayerPos = m_pNero.lock()->GetComponent<Transform>().lock()->GetPosition();
	vPlayerPos.y += 0.1f;
	Vector3 vLook = vPlayerPos - m_vTriggerPos;
	D3DXVec3Normalize(&vLook, &vLook);
	vLook.y = 0.f;
	vPlayerPos += vLook * 0.8f;

	m_vEye = FMath::Lerp(m_vEye, vPlayerPos, _fDeltaTime * 1.2f);

	Vector3 vLength = vPlayerPos - m_vEye;

	if (0.06f >= D3DXVec3Length(&vLength))
	{
		m_ePlayerCamMode = CAM_MODE_BASIC;
		return;
	}
}

void MainCamera::MoveMent_Trigger(float _fDeltaTime)
{
	
	switch (m_eTriggerCamMode)
	{
	case STAGE1_WAVE1:
		Trigger_Cam_Stage1_Wave1(_fDeltaTime);
		break;
	case STAGE1_WAVE1_END:
		Trigger_Cam_Stage1_Wave1_End(_fDeltaTime);
		break;
	case STAGE1_WAVE2_ENTER:
		Trigger_Cam_Stage1_Wave2(_fDeltaTime);
		break;
	case STAGE1_WAVE2_BATTLE:
		Trigger_Cam_Stage1_Wave2_Battle(_fDeltaTime);
		break;
	case STAGE1_WAVE2_END:
		Trigger_Cam_Stage1_Wave2_End(_fDeltaTime);
		break;
	case STAGE2_BUTTERFLY1:
		Trigger_Cam_Stage2_ButterFly1(_fDeltaTime);
		break;
	case STAGE2_BUTTERFLY1_END:
		Trigger_Cam_Stage2_ButterFly1_End(_fDeltaTime);
		break;
	case STAGE2_BUTTERFLY2:
		Trigger_Cam_Stage2_ButterFly2(_fDeltaTime);
		break;
	case STAGE2_BUTTERFLY2_END:
		Trigger_Cam_Stage2_ButterFly2_End(_fDeltaTime);
		break;
	case STAGE3_WAVE_HOLE:
		Trigger_Cam_Stage3_Hole(_fDeltaTime);
		break;
	case STAGE3_WAVE_HOLE_END:
		Trigger_Cam_Stage3_HoleEnd(_fDeltaTime);
		break;
	case STAGE4_BOSS_CUTSCENE:
		Trigger_Cam_Stage4_BossCutScene(_fDeltaTime);
		break;
	case STAGE6_BOSS_CUTSCENE:
		Trigger_Cam_Stage6_BossCutScene(_fDeltaTime);
		break;
	}
	m_fTriggerTime -= TimeSystem::GetInstance()->OriginDeltaTime();
	//������ �ð��� �ٵƴ�
	if (m_fTriggerTime <= 0.f)
	{
		
		switch (m_eTriggerCamMode)
		{
		case STAGE1_WAVE1:
			m_eAtType = AT_PLAYER;
			m_fDistanceToTarget = 1.f;
			m_fLerpSpeed = 0.6f;
			m_vEye = m_vAt;


			Renderer::GetInstance()->FadeInStart(0.7f);


			m_vAt = m_pAtTranform.lock()->GetPosition();
			m_pNero.lock()->GetFsm().lock()->ChangeState(NeroFSM::PROVOKE1);
			m_pTrigger.lock()->TriggerEnable();
			break;
		case STAGE1_WAVE1_END:
			m_eAtType = AT_PLAYER;
			m_fDistanceToTarget = OGDistance;
			m_ePlayerCamMode = CAM_MODE_RETURN_TO_PLAYER;
			m_fAngle = -200.f;
			m_fRotX = 13.5f;
			break;
		case STAGE1_WAVE2_ENTER:
			Renderer::GetInstance()->FadeOutStart(1.f);
			m_eTriggerCamMode = STAGE1_WAVE2_BATTLE;
			break;
		case STAGE1_WAVE2_BATTLE:
			break;
		case STAGE1_WAVE2_END:
			m_eAtType = AT_PLAYER;
			m_ePlayerCamMode = CAM_MODE_BASIC;
			break;
		case STAGE2_BUTTERFLY1:
			m_eTriggerCamMode = STAGE2_BUTTERFLY1_END;
			Renderer::GetInstance()->FadeOutStart(0.4f);
			
			break;
		case STAGE2_BUTTERFLY2:
			m_eTriggerCamMode = STAGE2_BUTTERFLY2_END;
			Renderer::GetInstance()->FadeOutStart(0.4f);
			break;
		case STAGE3_WAVE_HOLE:
			m_eAtType = AT_PLAYER;
			m_fDistanceToTarget = OGDistance;
			m_ePlayerCamMode = CAM_MODE_RETURN_TO_PLAYER;
			m_fAngle = -5.f;
			m_fRotX = -20.f;
			break;
		case STAGE4_BOSS_CUTSCENE:
			m_eAtType = AT_BOSS1;
			Set_At_Transform(FindGameObjectWithTag(GAMEOBJECTTAG::Monster5000).lock()->GetComponent<Transform>(),
				MainCamera::AT_BOSS1);
			SetDistance(1.1f);
			break;
		case STAGE6_BOSS_CUTSCENE:
			m_eAtType = AT_BOSS2;
			Set_At_Transform(FindGameObjectWithTag(GAMEOBJECTTAG::Monster5300).lock()->GetComponent<Transform>(),
				MainCamera::AT_BOSS2);
			SetDistance(0.9f);
			
			break;
		}

	}
}

void MainCamera::Trigger_Cam_Stage1_Wave1(float _fDeltaTime)
{
	static bool PlayOnce = true;
	if (PlayOnce)
	{
		Renderer::GetInstance()->FadeOutStart(3.f);
		PlayOnce = false;
	}
	m_vAt = FMath::Lerp(m_vAt, m_vTriggerPos, _fDeltaTime * 0.8f);
	m_vTriggerAngle.x -= _fDeltaTime * 25.15f;
	long    dwMouseMove = 0;

	Vector3 vLook = m_vTriggerPos - m_pNero.lock()->GetComponent<Transform>().lock()->GetPosition();
	D3DXVec3Normalize(&vLook, &vLook);
	Vector3 vUp = Vector3(0.f, 1.f, 0.f);
	Matrix matRotAxis, matRotX;

	D3DXMatrixRotationX(&matRotX, D3DXToRadian(m_vTriggerAngle.x));
	D3DXMatrixRotationAxis(&matRotAxis, &vUp, D3DXToRadian(m_vTriggerAngle.y));
	D3DXVec3TransformNormal(&vLook, &vLook, &matRotX);
	D3DXVec3TransformNormal(&vLook, &vLook, &matRotAxis);

	m_vLerpEye = m_vAt + vLook;

	m_vEye = FMath::Lerp(m_vEye, m_vLerpEye, _fDeltaTime * 0.5f);
}

void MainCamera::Trigger_Cam_Stage1_Wave1_End(float _fDeltaTime)
{
	m_vAt = m_vTriggerPos;
	m_vTriggerAngle.x += _fDeltaTime * 0.1f;
	long    dwMouseMove = 0;

	Vector3 vLook = m_vTriggerPos - m_pNero.lock()->GetComponent<Transform>().lock()->GetPosition();
	D3DXVec3Normalize(&vLook, &vLook);
	Vector3 vUp = Vector3(0.f, 1.f, 0.f);
	Matrix matRotAxis, matRotX;

	D3DXMatrixRotationX(&matRotX, D3DXToRadian(m_vTriggerAngle.x));
	D3DXMatrixRotationAxis(&matRotAxis, &vUp, D3DXToRadian(m_vTriggerAngle.y));
	D3DXVec3TransformNormal(&vLook, &vLook, &matRotX);
	D3DXVec3TransformNormal(&vLook, &vLook, &matRotAxis);
}

void MainCamera::Trigger_Cam_Stage1_Wave2(float _fDeltaTime)
{
	m_vAt = FMath::Lerp(m_vAt, m_vTriggerPos, _fDeltaTime * 1.2f);
	//m_vTriggerAngle.x -= _fDeltaTime * 25.15f;
	long    dwMouseMove = 0;

	Vector3 vLook = m_vTriggerPos - m_pNero.lock()->GetComponent<Transform>().lock()->GetPosition();
	D3DXVec3Normalize(&vLook, &vLook);
	Vector3 vUp = Vector3(0.f, 1.f, 0.f);
	Matrix matRotAxis, matRotX;

	D3DXMatrixRotationX(&matRotX, D3DXToRadian(m_vTriggerAngle.x));
	D3DXMatrixRotationAxis(&matRotAxis, &vUp, D3DXToRadian(m_vTriggerAngle.y));
	D3DXVec3TransformNormal(&vLook, &vLook, &matRotX);
	D3DXVec3TransformNormal(&vLook, &vLook, &matRotAxis);

	//m_vLerpEye = m_vAt + vLook;

	m_vEye = FMath::Lerp(m_vEye, Vector3(-3.634f,1.182f,8.895f), _fDeltaTime * 0.6f);
}

void MainCamera::Trigger_Cam_Stage1_Wave2_Battle(float _fDeltaTime)
{
	static bool PlayOnce = true;
	if (Renderer::GetInstance()->IsBlackOut() && PlayOnce)
	{
		PlayOnce = false;
		m_vAt = m_pAtTranform.lock()->GetPosition();
		m_vAt.y += m_fFloatingAmount;
		Vector3 vLook = { 0.f, 0.f ,1.f };
		Vector3 vUp = Vector3(0.f, 1.f, 0.f);
		Matrix matRot, matTest;

		vLook *= m_fDistanceToTarget;

		D3DXMatrixRotationX(&matTest, D3DXToRadian(m_fRotX));
		D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(m_fAngle));
		D3DXVec3TransformNormal(&vLook, &vLook, &matTest);
		D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

		m_vLerpEye = m_vAt + vLook;

		m_vEye = m_vLerpEye;
		Renderer::GetInstance()->FadeInStart(1.f);
		m_eAtType = AT_PLAYER;
		if (!m_pTrigger.expired())
			m_pTrigger.lock()->TriggerEnable();
	}
}

void MainCamera::Trigger_Cam_Stage1_Wave2_End(float _fDeltaTime)
{
	static float RequireTime = 0.f;
	RequireTime += TimeSystem::GetInstance()->OriginDeltaTime();
	if (RequireTime >= 1.f)
	{
		Vector3 vPlayerPos = m_pNero.lock()->GetComponent<Transform>().lock()->GetPosition();
		Vector3 vLook = m_vTriggerPos - vPlayerPos;
		vLook.y = 0.f;
		D3DXVec3Normalize(&vLook, &vLook);
		Vector3 LastPos = vPlayerPos - vLook * m_fDistanceToTarget;
		LastPos.y += m_fFloatingAmount;

		m_vEye = FMath::Lerp(m_vEye, LastPos, _fDeltaTime * 1.5f);

		Vector3 vTestLook = { 0.f, 0.f ,1.f };
		Vector3 vTestLook2 = vPlayerPos - m_vTriggerPos;
		vTestLook2.y = 0;
		float fDot = D3DXVec3Dot(&vTestLook, &vTestLook2);

		if (fDot > 1.f)
			fDot = 1.f - FLT_EPSILON;
		else if (fDot < -1.f)
			fDot = -1.f + FLT_EPSILON;

		float fRadian = acosf(fDot);
		Vector3	vCross;
		D3DXVec3Cross(&vCross, &vTestLook2, &vTestLook);

		if (vCross.y > 0)
			fRadian *= -1;

		m_fAngle = -D3DXToDegree(fRadian);
		
	}
}

void MainCamera::Trigger_Cam_Stage2_ButterFly1(float _fDeltaTime)
{
	m_vAt = m_pAtTranform.lock()->GetPosition();

	Vector3 vLook = m_pAtTranform.lock()->GetLook();
	Vector3 vUp = Vector3(0.f, 1.f, 0.f);
	Matrix matRot, matTest;

	vLook *= 0.22f;

	m_vLerpEye = m_vAt + vLook;

	m_vEye = FMath::Lerp(m_vEye, m_vLerpEye, _fDeltaTime * 0.8f);
}

void MainCamera::Trigger_Cam_Stage2_ButterFly1_End(float _fDeltaTime)
{
	if (Renderer::GetInstance()->IsBlackOut())
	{
		Renderer::GetInstance()->FadeInStart(0.4f);
		m_pAtTranform = m_pNero.lock()->GetComponent<Transform>();
		m_eAtType = AT_PLAYER;
		m_ePlayerCamMode = CAM_MODE_BASIC;
		m_bLerp = false;
	}
}

void MainCamera::Trigger_Cam_Stage2_ButterFly2(float _fDeltaTime)
{
	static float WaitingTime = 0.f;
	static bool PlayOnce = true;
	WaitingTime += _fDeltaTime;
	if (WaitingTime < 0.8f)
	{
		Player_Cam_Baisc(_fDeltaTime);
		return;
	}
	if (PlayOnce)
	{
		PlayOnce = false;
		Renderer::GetInstance()->FadeOutStart(0.4f);
		return;
	}
	if (Renderer::GetInstance()->IsBlackOut())
	{
		Renderer::GetInstance()->FadeInStart(0.4f);
		m_vAt = { -3.291f,1.548f,18.056f };
		m_vEye = { -2.965f,1.778f,16.962f };
	}
}

void MainCamera::Trigger_Cam_Stage2_ButterFly2_End(float _fDeltaTime)
{
	if (Renderer::GetInstance()->IsBlackOut())
	{
		Renderer::GetInstance()->FadeInStart(0.4f);
		m_pAtTranform = m_pNero.lock()->GetComponent<Transform>();
		m_eAtType = AT_PLAYER;
		m_ePlayerCamMode = CAM_MODE_BASIC;
		m_bLerp = false;
	}
}

void MainCamera::Trigger_Cam_Stage3_Hole(float _fDeltaTime)
{
	static bool Playonce = true;
	if (m_fTriggerTime <= 1.5f)
	{
		if (Playonce)
		{
			Playonce = false;
			vector<Vector3> _LostTimes;
			_LostTimes.emplace_back(Vector3(1.48f, 1.f, 0.3f));
			TimeSystem::GetInstance()->LostTime(_LostTimes);
		}
		Vector3 vDir = Vector3{ -0.787f,0.482f,30.880f } - m_vAt;
		
		vDir.y = 0.f;
		D3DXVec3Normalize(&vDir,&vDir);

		m_vAt += vDir * 0.2f  * _fDeltaTime;
		
	}
	m_vEye = FMath::Lerp(m_vEye, Vector3{ 0.351f,0.497f,30.876f }, 0.007f);
}

void MainCamera::Trigger_Cam_Stage3_HoleEnd(float _fDeltaTime)
{
	static float RequireTime = 0.f;
	RequireTime += TimeSystem::GetInstance()->OriginDeltaTime();
	if (RequireTime >= 0.2f)
	{
		Vector3 vPlayerPos = m_pNero.lock()->GetComponent<Transform>().lock()->GetPosition();
		Vector3 vLook = -m_pNero.lock()->GetComponent<Transform>().lock()->GetLook();
		vLook.y = 0.f;
		Vector3 LastPos = vPlayerPos - vLook * 0.8f;
		LastPos.y += m_fFloatingAmount;

		m_vEye = FMath::Lerp(m_vEye, LastPos, _fDeltaTime * 2.5f);
		
		Vector3 vTestLook = { 0.f, 0.f ,1.f };
		Vector3 vTestLook2 = vPlayerPos - m_vTriggerPos;
		vTestLook2.y = 0;
		float fDot = D3DXVec3Dot(&vTestLook, &vTestLook2);

		if (fDot > 1.f)
			fDot = 1.f - FLT_EPSILON;
		else if (fDot < -1.f)
			fDot = -1.f + FLT_EPSILON;

		float fRadian = acosf(fDot);
		Vector3	vCross;
		D3DXVec3Cross(&vCross, &vTestLook2, &vTestLook);

		if (vCross.y > 0)
			fRadian *= -1;

		m_fAngle = -D3DXToDegree(fRadian);

	}
}

void MainCamera::Trigger_Cam_Stage4_BossCutScene(float _fDeltaTime)
{

}

void MainCamera::Trigger_Cam_Stage4_BossCutScene_End(float _fDeltaTime)
{
}

void MainCamera::Trigger_Cam_Stage6_BossCutScene(float _fDeltaTime)
{
	m_vAt = m_pAtTranform.lock()->GetPosition();
	m_vAt.y += m_fFloatingAmount;

	m_vEye = m_vAt + m_pAtTranform.lock()->GetLook() * -1.5f;
}

void MainCamera::Boss_Cam_Em5000(float _fDeltaTime)
{
	if (m_pAtTranform.expired())
		return;
	UINT _CurAnimationIndex = m_pNero.lock()->Get_CurAnimationIndex();
	UINT _CurStateIndex = m_pNero.lock()->GetFsm().lock()->GetCurrentIndex();

	if (_CurStateIndex == NeroFSM::WIRE_HELLHOUND_LOOP
		|| _CurStateIndex == NeroFSM::WIRE_HELLHOUND_START)
	{
		if (m_fDistanceToTarget >= 0.65f)
		{
			m_fDistanceToTarget -= _fDeltaTime;
		}
	}
	else if(_CurStateIndex == NeroFSM::RUNLOOP
		|| _CurStateIndex == NeroFSM::DASHLOOP)
	{
		if (m_fDistanceToTarget <= 1.1f)
		{
			m_fDistanceToTarget += _fDeltaTime * 0.08f;
		}
	}

	std::weak_ptr<Transform>	_PlayerTransform = m_pNero.lock()->GetComponent<Transform>();
	if (!(Nero::ANI_EM5000_BUSTER_START <= _CurAnimationIndex
		&& _CurAnimationIndex <= Nero::ANI_EM5000_BUSTER_SWING_LOOP))
	{
		m_vAt = m_pAtTranform.lock()->GetPosition();
		m_vAt.y += 0.4f;
	}

	long    dwMouseMove = 0;

	if (dwMouseMove = Input::GetMouseMove(DIM_X))
	{
		m_fAngle += dwMouseMove / m_fSensitive;
	}

	if (dwMouseMove = Input::GetMouseMove(DIM_Z))
	{
		m_fDistanceToTarget -= dwMouseMove / 8000.f;
		if (m_fDistanceToTarget <= 0.65f)
		{
			m_fDistanceToTarget = 0.65f;
		}
		if (m_fDistanceToTarget >= 1.1f)
		{
			m_fDistanceToTarget = 1.1f;
		}
	}


	Vector3 vLook = _PlayerTransform.lock()->GetPosition() - m_vAt;
	vLook.y = 0.f;
	D3DXVec3Normalize(&vLook, &vLook);

	m_vLerpEye = _PlayerTransform.lock()->GetPosition() + vLook * m_fDistanceToTarget;
	m_vLerpEye.y += 0.2f;

	if (!(Nero::ANI_EM5000_BUSTER_START <= _CurAnimationIndex
		&& _CurAnimationIndex <= Nero::ANI_EM5000_BUSTER_SWING_LOOP))
	{
		m_vEye = FMath::Lerp(m_vEye, m_vLerpEye, _fDeltaTime * 4.f);
	}
}

void MainCamera::Boss_Cam_Em5300(float _fDeltaTime)
{
	if (m_pAtTranform.expired())
		return;
	UINT _CurAnimationIndex = m_pNero.lock()->Get_CurAnimationIndex();
	UINT _CurStateIndex = m_pNero.lock()->GetFsm().lock()->GetCurrentIndex();

	if (_CurStateIndex == NeroFSM::WIRE_HELLHOUND_LOOP
		|| _CurStateIndex == NeroFSM::WIRE_HELLHOUND_START)
	{
		if (m_fDistanceToTarget >= 0.65f)
		{
			m_fDistanceToTarget -= _fDeltaTime;
		}
	}
	else if (_CurStateIndex == NeroFSM::RUNLOOP
		|| _CurStateIndex == NeroFSM::DASHLOOP)
	{
		if (m_fDistanceToTarget <= 0.9f)
		{
			m_fDistanceToTarget += _fDeltaTime * 0.08f;
		}
	}

	std::weak_ptr<Transform>	_PlayerTransform = m_pNero.lock()->GetComponent<Transform>();
	std::weak_ptr<NuClear>	_Nuclear = static_pointer_cast<NuClear>(FindGameObjectWithTag(GAMEOBJECTTAG::Eff_NuClear).lock());
	//if (!_Nuclear.expired() && !_Nuclear.lock()->IsActive())
	//{
	//	m_vAt = static_pointer_cast<Monster>(FindGameObjectWithTag(GAMEOBJECTTAG::Monster5300).lock())->GetMonsterBoneWorldPos("Hip");
	//	m_vAt.y -= 0.15f;
	//}
	//else if(!_Nuclear.expired() && _Nuclear.lock()->IsFallTime())
	//{
	//	m_vAt = _Nuclear.lock()->GetComponent<Transform>().lock()->GetPosition();
	//	m_vAt.y -= 0.1f;
	//}

	if (!_Nuclear.expired() && _Nuclear.lock()->IsFallTime())
	{
		m_vAt = _Nuclear.lock()->GetComponent<Transform>().lock()->GetPosition();
		m_vAt.y -= 0.1f;
		if (m_fFloatingAmount <= 0.17f)
			m_fFloatingAmount += _fDeltaTime;
		if (m_fDistanceToTarget >= 1.1f)
			m_fDistanceToTarget -= _fDeltaTime * 0.1f;
	}
	else
	{
		m_vAt = static_pointer_cast<Monster>(FindGameObjectWithTag(GAMEOBJECTTAG::Monster5300).lock())->GetMonsterBoneWorldPos("Hip");
		m_vAt.y -= 0.15f;
	}


	long    dwMouseMove = 0;

	if (dwMouseMove = Input::GetMouseMove(DIM_X))
	{
		m_fAngle += dwMouseMove / m_fSensitive;
	}

	if (dwMouseMove = Input::GetMouseMove(DIM_Z))
	{
	}


	Vector3 vLook = _PlayerTransform.lock()->GetPosition() - m_vAt;
	vLook.y = 0.f;
	D3DXVec3Normalize(&vLook, &vLook);

	m_vLerpEye = _PlayerTransform.lock()->GetPosition() + vLook * m_fDistanceToTarget;
	m_vLerpEye.y += m_fFloatingAmount;

	m_vEye = FMath::Lerp(m_vEye, m_vLerpEye, _fDeltaTime * 2.5f);
}
