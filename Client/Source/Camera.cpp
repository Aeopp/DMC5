#include "stdafx.h"
#include "Camera.h"


#include "TestObject.h"
#include "Em100.h"
#include "BtlPanel.h"

void Camera::Free()
{
}

Camera* Camera::Create()
{
    return new Camera{};
}

HRESULT Camera::Ready()
{
    m_vEye = Vector3(0.f, 0.f, -5.f);
    m_vAt = Vector3(0.f, 0.f, 0.f);
    m_vUp = Vector3(0.f, 1.f, 0.f);

    m_fFovY = D3DXToRadian(60.f);
    m_fAspect = float(g_nWndCX) / g_nWndCY;
    m_fNear = 1.f;
    m_fFar = 1000.f;


    D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
    Graphic::GetDevice()->SetTransform(D3DTS_VIEW, &m_matView);

    D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFovY, m_fAspect, m_fNear, m_fFar);
    Graphic::GetDevice()->SetTransform(D3DTS_PROJECTION, &m_matProj);

    return S_OK;
}

HRESULT Camera::Awake()
{
	m_pPlayer = std::static_pointer_cast<TestObject>(FindGameObjectWithTag(Player).lock());
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
    //////////////////////////////////////////////////
	D3DXMatrixLookAtLH(&m_matView, &m_vEye, &m_vAt, &m_vUp);
	Graphic::GetDevice()->SetTransform(D3DTS_VIEW, &m_matView);

	D3DXMatrixPerspectiveFovLH(&m_matProj, m_fFovY, m_fAspect, m_fNear, m_fFar);
	Graphic::GetDevice()->SetTransform(D3DTS_PROJECTION, &m_matProj);
    //////////////////////////////////////////////////
    if (true == m_bFix)
    {
		Mouse_Fix();
		Move_Mouse(_fDeltaTime);
    }
   Move(_fDeltaTime);
    


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
}

void Camera::OnDisable()
{
}

void Camera::LockOn()
{

    Vector3 vPlayerLook, vPlayerRight;

    m_vAt = m_pEm100Trans.lock()->GetPosition();
    vPlayerLook = m_pPlayerTrans.lock()->GetLook();
	D3DXVec3Normalize(&vPlayerLook, &vPlayerLook);

	vPlayerLook *= m_fCameraAngle;

    vPlayerRight = m_pPlayerTrans.lock()->GetRight();
	D3DXVec3Normalize(&vPlayerRight, &vPlayerRight);

	Matrix matRotAxis;

	D3DXMatrixRotationAxis(&matRotAxis, &vPlayerRight, D3DXToRadian(m_fCameraAngle));
	D3DXVec3TransformNormal(&vPlayerLook, &vPlayerLook, &matRotAxis);

    D3DXVECTOR3 vDir = m_pPlayerTrans.lock()->GetPosition() - m_pEm100Trans.lock()->GetPosition();

    D3DXVec3Normalize(&vDir, &vDir);

	//m_vEye = m_pPlayerTrans.lock()->GetPosition()+(-m_pPlayerTrans.lock()->GetLook() * 5);


	m_vEye = m_pPlayerTrans.lock()->GetPosition() + (vDir * 5) + Vector3(0.f, m_fCameraAngle, 0.f);

	//m_pTransformCom->m_vInfo[INFO_POS] = m_vEye;


    long    dwMouseMove = 0;

	if (dwMouseMove = Input::GetMouseMove(DIM_Z))
		m_fCameraAngle += (dwMouseMove / 100);

}

void Camera::Move(const float& _fTimeDelta)
{
    Matrix      matCameraWorld;
    D3DXMatrixInverse(&matCameraWorld, NULL, &m_matView);
        
    if (Input::GetKey(DIK_UP))
    {
        Vector3 vLook;
        memcpy(&vLook, &matCameraWorld.m[2][0], sizeof(Vector3));

        Vector3 vLength = *D3DXVec3Normalize(&vLook, &vLook) * 50.f * _fTimeDelta;

        m_vEye += vLength;
        m_vAt += vLength;
    }

    if (Input::GetKey(DIK_DOWN))
    {
        Vector3 vLook;
        memcpy(&vLook, &matCameraWorld.m[2][0], sizeof(Vector3));

		Vector3 vLength = *D3DXVec3Normalize(&vLook, &vLook) * 50.f * _fTimeDelta;

		m_vEye -= vLength;
		m_vAt -= vLength;
    }
    
    if (Input::GetKey(DIK_LEFT))
    {
        Vector3 vRight;
        memcpy(&vRight, &matCameraWorld.m[0][0], sizeof(Vector3));

		Vector3 vLength = *D3DXVec3Normalize(&vRight, &vRight) * 25.f * _fTimeDelta;

		m_vEye -= vLength;
		m_vAt -= vLength;
    }

	if (Input::GetKey(DIK_RIGHT))
	{
		Vector3 vRight;
		memcpy(&vRight, &matCameraWorld.m[0][0], sizeof(Vector3));

		Vector3 vLength = *D3DXVec3Normalize(&vRight, &vRight) * 25.f * _fTimeDelta;

		m_vEye += vLength;
		m_vAt += vLength;
	}

    if (Input::GetKeyDown(DIK_RCONTROL))
    {
        if (true == m_bClick)
            return;

        m_bClick = true;

        if (true == m_bFix)
            m_bFix = false;
        else
            m_bFix = true;
    }
    else
        m_bClick = false;

    if (false == m_bFix)
        return;
 }

void Camera::Move_Mouse(const float& _fTimeDelta)
{
    Matrix  matCameraWorld;
    D3DXMatrixInverse(&matCameraWorld, NULL, &m_matView);

    long    dwMouseMove = 0;

    if (dwMouseMove = Input::GetMouseMove(DIM_Y))
    {
        Vector3  vRight;
        memcpy(&vRight, &matCameraWorld.m[0][0], sizeof(Vector3));

        Vector3 vLook = m_vAt - m_vEye;

        Matrix  matRot;
        D3DXMatrixRotationAxis(&matRot, &vRight, D3DXToRadian(dwMouseMove) / 10.f);
        D3DXVec3TransformNormal(&vLook, &vLook, &matRot);

        m_vAt = m_vEye + vLook;
    }

    if (dwMouseMove = Input::GetMouseMove(DIM_X))
    {
        Vector3 vUp = Vector3(0.f, 1.f, 0.f);
        Vector3 vLook = m_vAt - m_vEye;

        Matrix matRot;
        D3DXMatrixRotationAxis(&matRot, &vUp, D3DXToRadian(dwMouseMove) / 10.f);
        D3DXVec3TransformNormal(&vLook, &vLook, &matRot);


        m_vAt = m_vEye + vLook;
    }

  

 }

void Camera::Mouse_Fix()
{
    POINT   ptMouse{ g_nWndCX >> 1, g_nWndCY >> 1 };

    ClientToScreen(g_hWnd, &ptMouse);
    SetCursorPos(ptMouse.x, ptMouse.y);

}
