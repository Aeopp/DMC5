#ifndef MainCamera_h__
#define MainCamera_h__

#include "GameObject.h"
class Nero;
class MainCamera :
    public GameObject
{
public:
    enum AT_TYPE { AT_PLAYER,AT_TRIGGER};
    enum PLAYER_CAM_MODE { CAM_MODE_BASIC };
    enum TRIGGER_CAM_MODE { STAGE1_WAVE1 };
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
    float Get_Angle(float _fAddAngle = 0.f) { return m_fAngle + _fAddAngle; }
public:
    void Set_PlayerCamMode(UINT _ePlayerCamMode) { m_ePlayerCamMode = _ePlayerCamMode; }
    void Set_At_Transform(std::weak_ptr<Transform> _pTransform, UINT _eAtType);
    void Set_TriggerCam(UINT _eTriggerCamMode,const Vector3& _vTriggerPos,const float& _fTriggerTime);
    void SetDistance(float _fDistance) { m_fDistanceToTarget = _fDistance; }
public:
    void DecreaseDistance(float _GoalDis,float _fDeltaTime);
    void IncreaseDistance(float _GoalDis, float _fDeltaTime);
public:
    virtual std::string GetName() override;
    virtual void Editor()override;
private:
    void Mouse_Fix();
    //플레이어 카메라 함수
    void MoveMent_Player(float _fDeltaTime);
    void Player_Cam_Baisc(float _fDeltaTime);
    //트리거 카메라 함수
    void MoveMent_Trigger(float _fDeltaTime);
    void Trigger_Cam_Stage1_Wave1(float _fDeltaTime);
private:
    std::weak_ptr<Transform>        m_pAtTranform;
    std::weak_ptr<Nero>             m_pNero;

    UINT                            m_eAtType = AT_PLAYER;
    UINT                            m_ePlayerCamMode = CAM_MODE_BASIC;
    UINT                            m_eTriggerCamMode = STAGE1_WAVE1;


    //////////////////////////////////////////
	Vector3	m_vEye, m_vAt, m_vUp,m_vLerpEye;
	float	m_fFovY, m_fAspect, m_fNear, m_fFar;
	Matrix	m_matView, m_matProj;


	float	m_fDistanceToTarget = 0.f;
	float	m_fCameraAngle = 45.f;
    float   m_fAngle = 0.f;
    float   m_fSensitive = 0.f;
    bool	m_bFix = false;
    float   m_fRotX = 0.f;
    float   m_fFloatingAmount = 0.f;
    //////////////////////////////////////////
    float   m_fDecreaseFactor = 0.f;
    float   m_fIncreaseFactor = 0.f;

    float   m_fTriggerTime = 0.f;
    Vector3 m_vTriggerPos;
    Vector3 m_vTriggerAngle;

    float   m_fLerpSpeed = 0.f;
};



#endif // MainCamera_h__