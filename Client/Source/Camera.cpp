#include "stdafx.h"
#include "Camera.h"


#include "TestObject.h"
#include "Em100.h"
#include "BtlPanel.h"

Camera::Camera()
    : m_fFovY(45.f)
    , m_fNear(0.1f)
    , m_fFar(30.f)
    , m_fSensitivityMove(0.05f)
    , m_fSensitivityRot(0.2f)
    , m_fSensitivityWheel(0.005f)
    , m_fDistance(10.f)
{
    D3DXMatrixIdentity(&m_matView);
    D3DXMatrixIdentity(&m_matProjection);
    m_vRot = D3DXVECTOR3(45.f, 0.f, 0.f);
}

void Camera::Free()
{
    GameObject::Free();
}

Camera* Camera::Create()
{
    return new Camera{};
}

HRESULT Camera::Ready()
{
    m_pTransform.lock()->SetPosition(D3DXVECTOR3(0.f, 10.f, 0.f));
    m_pTransform.lock()->SetRotation(D3DXVECTOR3(45.f, 0.f, 0.f));

    return S_OK;
}

HRESULT Camera::Awake()
{
 	//m_pPlayerTrans = m_pPlayer.lock()->GetComponent<ENGINE::Transform>();

    //m_pBtlPanel = std::static_pointer_cast<BtlPanel>(FindGameObjectWithTag(BTLPANEL).lock());


	//m_pEm100 = std::static_pointer_cast<Em100>(FindGameObjectWithTag(Monster100).lock());
	//m_pEm100Trans = m_pEm100.lock()->GetComponent<ENGINE::Transform>();
    return S_OK;
}

HRESULT Camera::Start()
{
    return S_OK;
}

UINT Camera::Update(const float _fDeltaTime)
{
    ShowCameraInfo();
    MoveCamera();
    UpdateCamera();



    //////LockOn////////////////
    //if(m_bLockon)
    //    LockOn();

    //if (Input::GetKeyDown(DIK_LSHIFT))
    //{
    //    if (m_bLockon)
    //    {
    //        m_bLockon = false;
    //        m_pBtlPanel.lock()->SetTargetActive(false);
    //    }
    //    else
    //    {
    //        m_bLockon = true;
    //        m_pBtlPanel.lock()->SetTargetActive(true);
    //        m_pBtlPanel.lock()->SetTargetPos(m_pEm100Trans.lock()->GetPosition() + Vector3(0.f,3.f,0.f));
    //    }
    //}
    ////////////////////////////
    return 0;
}

UINT Camera::LateUpdate(const float _fDeltaTime)
{
   /* if (Input::GetKeyDown(DIK_T))
        ++m_iTest;
    if (Input::GetKeyDown(DIK_Y))
        --m_iTest;*/


    return 0;
}

void Camera::OnEnable()
{
    GameObject::OnEnable();
}

void Camera::OnDisable()
{
    GameObject::OnDisable();
}

std::string Camera::GetName()
{
    return "Camera";
}

void Camera::Editor()
{
    ShowCameraInfo();
}

void Camera::MoveCamera()
{
    if (Input::GetKey(DIK_LALT))
    {
        if (Input::GetMouse(DIM_M))
        {
            D3DXVECTOR3 vPosition = m_pTransform.lock()->GetPosition();
            D3DXVECTOR3 vTrans(0.f, 0.f, 0.f);
            LONG lMove = 0;
            if (lMove = Input::GetMouseMove(DIM_X))
            {
                D3DXVECTOR3 vRight = m_pTransform.lock()->GetRight();
                vTrans -= lMove * vRight;
            }
            if (lMove = Input::GetMouseMove(DIM_Y))
            {
                D3DXVECTOR3 vUp = m_pTransform.lock()->GetUp();
                vTrans += lMove * vUp;
            }
            m_pTransform.lock()->SetPosition(vPosition + vTrans * m_fSensitivityMove);

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

            m_vRot += vRot * m_fSensitivityRot;

            m_pTransform.lock()->SetRotation(m_vRot);
        }
        else if (Input::GetMouse(DIM_R))
        {

        }
    }

    LONG lWheel = 0;
    if (lWheel = Input::GetMouseMove(DIM_Z))
    {
        D3DXVECTOR3 vLook = m_pTransform.lock()->GetLook();

        D3DXVECTOR3 vPosition = m_pTransform.lock()->GetPosition();

        m_pTransform.lock()->SetPosition(vPosition + vLook * lWheel * m_fSensitivityWheel);
    }
}

void Camera::UpdateCamera()
{
    D3DXMATRIX matWorld = m_pTransform.lock()->GetWorldMatrix();
    D3DXMatrixInverse(&(m_matView), nullptr, &matWorld);
    D3DXMatrixPerspectiveFovLH(&m_matProjection, D3DXToRadian(m_fFovY), g_nWndCX / (float)g_nWndCY, m_fNear, m_fFar);

    g_pDevice->SetTransform(D3DTS_VIEW, &m_matView);
    g_pDevice->SetTransform(D3DTS_PROJECTION, &m_matProjection);

}

void Camera::ShowCameraInfo()
{
    ImGui::InputFloat("FovY", &m_fFovY);
    ImGui::InputFloat("Near", &m_fNear);
    ImGui::InputFloat("Far", &m_fFar);

    ImGui::Text("Transform");

    D3DXVECTOR3 vPosition = m_pTransform.lock()->GetPosition();
    //D3DXVECTOR3 vRotation = m_pTransform.lock()->GetRotation();
    D3DXVECTOR3 vScale = m_pTransform.lock()->GetScale();

    ImGui::InputFloat3("Scale", vScale);
    //ImGui::InputFloat3("Rotation", vRotation);
    ImGui::InputFloat3("Position", vPosition);

    m_pTransform.lock()->SetScale(vScale);
    //m_pTransform.lock()->SetRotation(vRotation);
    m_pTransform.lock()->SetPosition(vPosition);

    ImGui::Text("Sensitivity");
    ImGui::InputFloat("Wheel", &m_fSensitivityWheel);
    ImGui::InputFloat("Move", &m_fSensitivityMove);
    ImGui::InputFloat("Rotation", &m_fSensitivityRot);

    ImGui::Text("");
    ImGui::InputFloat("Distance##Cam", &m_fDistance);

}