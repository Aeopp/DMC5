#include "stdafx.h"
#include "MainCamera.h"
#include "Nero.h"
MainCamera::MainCamera()
	:m_fFovY(0.f), m_fAspect(0.f), m_fNear(0.f), m_fFar(0.f), 
	m_fCameraAngle(0.f),m_fDistanceToTarget(0.f),m_bFix(true)
{
	D3DXMatrixIdentity(&m_matProj);
	D3DXMatrixIdentity(&m_matView);
	m_vEye = { 0.f,0.f,0.f };
	m_vAt = { 0.f,0.f,0.f };
	m_vUp = { 0.f,1.f,0.f };
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


	m_fCameraAngle = 35.f;
	m_fDistanceToTarget = OGDistance;
	m_fDistanceToTarget = 1.f;
	m_fRotX = -17.1f;
	m_fFloatingAmount = 0.19f;
	m_fSensitive = 15.f;
	m_fDecreaseFactor = 0.6f;
	m_fIncreaseFactor = 0.07f;

	return S_OK;
}

HRESULT MainCamera::Awake()
{
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

void MainCamera::Set_At_Transform(std::weak_ptr<Transform> _pTransform, UINT _eAtType)
{
	m_pAtTranform = _pTransform;
	m_eAtType = _eAtType;
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
	default:
		break;
	}
}

void MainCamera::Player_Cam_Baisc(float _fDeltaTime)
{
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

	m_vEye = m_vAt + vLook; // ¿øº»

	//static bool Test = true;

	//if (Input::GetKeyDown(DIK_5))
	//	Test = !Test;

	//if(Test)
	//	m_vEye = m_vAt + vLook;

	//m_vLerpEye = m_vAt + vLook;

	//m_vEye = FMath::Lerp(m_vEye, m_vLerpEye, _fDeltaTime * 1.5f);
}

void MainCamera::MoveMent_Trigger(float _fDeltaTime)
{
	switch (m_eTriggerCamMode)
	{
	case STAGE1_WAVE1:
		Trigger_Cam_Stage1_Wave1(_fDeltaTime);
		break;
	}
}

void MainCamera::Trigger_Cam_Stage1_Wave1(float _fDeltaTime)
{
	Vector3 _TempAt;
	_TempAt = m_pAtTranform.lock()->GetPosition();
	_TempAt.y -= 0.7f;
	m_vAt = FMath::Lerp(m_vAt, _TempAt, _fDeltaTime * 0.5f);
	//m_vAt.y += m_fFloatingAmount;
	long    dwMouseMove = 0;

	Vector3 vLook = { -1.5f, 0.f ,1.f };
	Vector3 vUp = Vector3(0.f, 1.f, 0.f);
	Matrix matRotAxis, matRotX;

	vLook *= m_fDistanceToTarget;

	D3DXMatrixRotationX(&matRotX, D3DXToRadian(m_fRotX));
	D3DXMatrixRotationAxis(&matRotAxis, &vUp, D3DXToRadian(m_fAngle));
	D3DXVec3TransformNormal(&vLook, &vLook, &matRotX);
	D3DXVec3TransformNormal(&vLook, &vLook, &matRotAxis);

	m_vLerpEye = m_vAt + vLook;

	m_vEye = FMath::Lerp(m_vEye, m_vLerpEye, _fDeltaTime * 0.5f);
}
