#ifndef MainCamera_h__
#define MainCamera_h__

#include "GameObject.h"
class Nero;
class MainCamera :
    public GameObject
{
public:
    enum AT_TYPE { AT_PLAYER };
    enum PLAYER_CAM_MODE { CAM_MODE_BASIC };
private:
    explicit MainCamera();
    virtual ~MainCamera();
    // GameObject을(를) 통해 상속됨
    virtual void Free() override;
public:
    static MainCamera* Create();
    virtual HRESULT Ready() override;
    virtual HRESULT Awake() override;
    virtual HRESULT Start() override;
    virtual UINT Update(const float _fDeltaTime) override;
    virtual UINT LateUpdate(const float _fDeltaTime) override;
    virtual void OnEnable() override;
    virtual void OnDisable() override;
public:
    float Get_Angle() { return m_fAngle; }
public:
    void Set_PlayerCamMode(UINT _ePlayerCamMode) { m_ePlayerCamMode = _ePlayerCamMode; }
    void Set_At_Transform(std::weak_ptr<Transform> _pTransform, UINT _eAtType);
public:
    virtual std::string GetName() override;
    virtual void Editor()override;
private:
    void Mouse_Fix();
    //플레이어 카메라 함수
    void MoveMent_Player(float _fDeltaTime);
    void Player_Cam_Baisc(float _fDeltaTime);
private:
    std::weak_ptr<Transform>        m_pAtTranform;
    std::weak_ptr<Nero>             m_pNero;

    UINT                            m_eAtType = AT_PLAYER;
    UINT                            m_ePlayerCamMode = CAM_MODE_BASIC;



    //////////////////////////////////////////
	Vector3	m_vEye, m_vAt, m_vUp;
	float	m_fFovY, m_fAspect, m_fNear, m_fFar;
	Matrix	m_matView, m_matProj;


	float	m_fDistanceToTarget = 10.f;
	float	m_fCameraAngle = 45.f;
    float   m_fAngle = 0.f;
    float   m_fSensitive = 10.f;
    bool	m_bFix = false;
    float   m_fRotX = 0.f;
    float   m_fFloatingAmount = 0.f;
    //////////////////////////////////////////






};



#endif // MainCamera_h__