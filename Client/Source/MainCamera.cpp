#include "stdafx.h"
#include "MainCamera.h"
#include "Nero.h"
#include "TimeSystem.h"
#include "NeroFSM.h"
#include "Trigger.h"
#include "Renderer.h"
#include "QliphothBlock.h"
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
	m_fLerpSpeed = 1.4f;

	return S_OK;
}

HRESULT MainCamera::Awake()
{
	m_pNero = std::static_pointer_cast<Nero>(FindGameObjectWithTag(Player).lock());
	m_pAtTranform = FindGameObjectWithTag(Player).lock()->GetComponent<Transform>();
	m_eAtType = AT_PLAYER;
	m_ePlayerCamMode = CAM_MODE_BASIC;

	m_vAt = m_pAtTranform.lock()->GetPosition();

	Vector3 vLook;
	memcpy(&vLook, m_pAtTranform.lock()->GetWorldMatrix().m[2], sizeof(Vector3));

	m_vEye = m_vAt + vLook * -5.f;
	return S_OK;
}

HRESULT MainCamera::Start()
{
	return S_OK;
}

UINT MainCamera::Update(const float _fDeltaTime)
{
	GameObject::Update(_fDeltaTime);
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
	default:
		break;
	}
	return 0;
}

UINT MainCamera::LateUpdate(const float _fDeltaTime)
{
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
		_LostTimes.emplace_back(Vector3(2.5f, 1.f, 0.3f));
		TimeSystem::GetInstance()->LostTime(_LostTimes);
		
		m_fTriggerTime = 3.5f;
		m_fLerpSpeed = 1.2f;
		Renderer::GetInstance()->FadeOutStart(3.3f);
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
	}
}

void MainCamera::DecreaseDistance(float _GoalDis, float _fDeltaTime)
{
	if (m_fDistanceToTarget <= _GoalDis)
	{
		m_fDecreaseFactor = 0.6f;
		return;
	}
	if (m_fDecreaseFactor >= 0.02f)
		m_fDecreaseFactor -= 0.02f;
	m_fIncreaseFactor = 0.07f;
	m_fDistanceToTarget -= m_fDecreaseFactor * _fDeltaTime;
}

void MainCamera::IncreaseDistance(float _GoalDis, float _fDeltaTime)
{
	if (m_fDistanceToTarget >= _GoalDis)
	{
		m_fIncreaseFactor = 0.07f;
		return;
	}
	m_fDecreaseFactor = 0.6f;
	m_fIncreaseFactor += 0.005f;
	m_fDistanceToTarget += m_fIncreaseFactor * _fDeltaTime;
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
	if (m_fLerpSpeed <= 1.4f)
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
	if (m_fRotX >= 15.f)
		m_fRotX = 15.f;


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
}

void MainCamera::Player_Cam_WaveEnd(float _fDeltaTime)
{
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
	if (m_fRotX >= 15.f)
		m_fRotX = 15.f;


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
			Set_TriggerCam(STAGE1_WAVE2_END, Vector3{ -3.6f,0.48f,11.8f }, 4.f);
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

	if (0.05f >= D3DXVec3Length(&vLength))
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
			m_fLerpSpeed = 0.5f;
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
			m_fRotX = 15.f;
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
