#include "framework.h"
#include "Camera.h"
#include "Transform.h"

Camera Camera::Instance;

Camera::Camera()
	: m_fFovY(90.f)
	, m_fNear(0.1f)
	, m_fFar(1000.f)
	, m_fSensitivityMove(1.f)
	, m_fSensitivityRot(1.f)
	, m_fSensitivityWheel(1.f)
	, m_fDistance(10.f)
{
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProjection);
	m_pTransform = new Transform;
	m_pTransform->SetPosition(D3DXVECTOR3(0.f, 10.f, 0.f));
	m_pTransform->SetRotation(D3DXVECTOR3(45.f, 0.f, 0.f));
}

Camera::~Camera()
{
	delete m_pTransform;
}

void Camera::Update()
{
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(51.f / 255.f, 51.f / 255.f, 51.f / 255.f, 1.f));
	ImGui::Begin("CAMERA");
	ShowCameraInfo();
	ImGui::PopStyleColor();
	ImGui::End();
	MoveCamera();
	UpdateCamera();

}

void Camera::MoveCamera()
{
	if (Input::GetKey(DIK_LALT))
	{
		if (Input::GetMouse(DIM_M))
		{
			D3DXVECTOR3 vPosition = Instance.m_pTransform->GetPosition();
			D3DXVECTOR3 vTrans(0.f, 0.f, 0.f);
			LONG lMove = 0;
			if (lMove = Input::GetMouseMove(DIM_X))
			{
				D3DXVECTOR3 vRight = Instance.m_pTransform->GetRight();
				vTrans -= lMove * vRight;
			}
			if (lMove = Input::GetMouseMove(DIM_Y))
			{
				D3DXVECTOR3 vUp = Instance.m_pTransform->GetUp();
				vTrans -= lMove * vUp;
			}
			Instance.m_pTransform->SetPosition(vPosition + vTrans * Instance.m_fSensitivityMove);

		}
		else if (Input::GetMouse(DIM_L))
		{
			LONG lMove = 0;
			D3DXVECTOR3 vRot(0.f, 0.f, 0.f);
			if (lMove = Input::GetMouseMove(DIM_X))
			{
				vRot.y += lMove;
			}
			if (lMove = Input::GetMouseMove(DIM_Y))
			{
				vRot.x += lMove;
			}
			D3DXVECTOR3 vRotation = Instance.m_pTransform->GetRotation();
			Instance.m_pTransform->SetRotation(vRotation + vRot * Instance.m_fSensitivityRot);
		}
		else if (Input::GetMouse(DIM_R))
		{

		}
	}

	LONG lWheel = 0;
	if (lWheel = Input::GetMouseMove(DIM_Z))
	{
		D3DXVECTOR3 vLook = Instance.m_pTransform->GetLook();

		D3DXVECTOR3 vPosition = Instance.m_pTransform->GetPosition();

		Instance.m_pTransform->SetPosition(vPosition + vLook * lWheel * Instance.m_fSensitivityWheel );
	}
}

void Camera::UpdateCamera()
{
	D3DXMATRIX matWorld = Instance.m_pTransform->GetWorldMatrix();
	D3DXMatrixInverse(&(Instance.m_matView), nullptr, &matWorld);
	D3DXMatrixPerspectiveFovLH(&Instance.m_matProjection, D3DXToRadian(Instance.m_fFovY), g_nWndCX / (float)g_nWndCY, Instance.m_fNear, Instance.m_fFar);

	g_pDevice->SetTransform(D3DTS_VIEW, &Instance.m_matView);
	g_pDevice->SetTransform(D3DTS_PROJECTION, &Instance.m_matProjection);

}

void Camera::ShowCameraInfo()
{
	ImGui::InputFloat("FovY", &Instance.m_fFovY);
	ImGui::InputFloat("Near", &Instance.m_fNear);
	ImGui::InputFloat("Far", &Instance.m_fFar);

	ImGui::Text("Transform");

	D3DXVECTOR3 vPosition = Instance.m_pTransform->GetPosition();
	D3DXVECTOR3 vRotation = Instance.m_pTransform->GetRotation();
	D3DXVECTOR3 vScale = Instance.m_pTransform->GetScale();

	ImGui::InputFloat3("Scale", vScale);
	ImGui::InputFloat3("Rotation", vRotation);
	ImGui::InputFloat3("Position", vPosition);

	Instance.m_pTransform->SetScale(vScale);
	Instance.m_pTransform->SetRotation(vRotation);
	Instance.m_pTransform->SetPosition(vPosition);

	ImGui::Text("Sensitivity");
	ImGui::InputFloat("Wheel", &Instance.m_fSensitivityWheel);
	ImGui::InputFloat("Move", &Instance.m_fSensitivityMove);
	ImGui::InputFloat("Rotation", &Instance.m_fSensitivityRot);

	ImGui::Text("");
	ImGui::InputFloat("Distance##Cam", &Instance.m_fDistance);

}

void Camera::SetCameraFocus(D3DXVECTOR3 _vPosition)
{
	D3DXVECTOR3 vLook = Instance.m_pTransform->GetLook();

	D3DXVECTOR3 vPos = _vPosition - vLook * Instance.m_fDistance;

	Instance.m_pTransform->SetPosition(vPos);
}
