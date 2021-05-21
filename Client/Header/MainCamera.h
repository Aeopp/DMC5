#ifndef MainCamera_h__
#define MainCamera_h__

#include "GameObject.h"
class Nero;
class Trigger;
class MainCamera :
    public GameObject
{
public:
    enum AT_TYPE { AT_PLAYER,AT_TRIGGER,AT_BOSS};
    enum PLAYER_CAM_MODE { CAM_MODE_BASIC,CAM_MODE_WAVE_END,CAM_MODE_RETURN_TO_PLAYER };
	enum TRIGGER_CAM_MODE {	STAGE1_WAVE1, STAGE1_WAVE1_END, STAGE1_WAVE2_ENTER, STAGE1_WAVE2_BATTLE, STAGE1_WAVE2_END,
        STAGE2_BUTTERFLY1, STAGE2_BUTTERFLY1_END, STAGE2_BUTTERFLY2, STAGE2_BUTTERFLY2_END};
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
    void Set_Trigger(std::weak_ptr<Trigger> _Trigger) { m_pTrigger = _Trigger; }
    void Set_PlayerCamMode(UINT _ePlayerCamMode);
    void Set_At_Transform(std::weak_ptr<Transform> _pTransform, UINT _eAtType);
    void Set_TriggerCam(UINT _eTriggerCamMode,const Vector3& _vTriggerPos,const float& _fTriggerTime);
    void SetDistance(float _fDistance) { m_fDistanceToTarget = _fDistance; }
    void SetQliphothBlock(std::vector<weak_ptr<class Effect>> _vecQliphothBlock) { m_vecQliphothBlock = _vecQliphothBlock; }
    void SetLerp(bool _LerpOrNot) { m_bLerp = _LerpOrNot; }
    void SetAngle(const Vector3& _vAngle);
    void SetShakeInfo(float _fShakeTime, float _fShakePower);
    void SetFadeSceneInfo(float _fFadeInAmout);
    void SetStartPos();
public:
    void DecreaseDistance(float _GoalDis,float _fDeltaTime);
    void IncreaseDistance(float _GoalDis, float _fDeltaTime);
public:
    virtual std::string GetName() override;
    virtual void Editor()override;
private:
    void Mouse_Fix();
    void Shaking();
    void CheckFadeOut();
    //플레이어 카메라 함수
    void MoveMent_Player(float _fDeltaTime);
    void Player_Cam_Baisc(float _fDeltaTime);
    void Player_Cam_WaveEnd(float _fDeltaTime);
    void Player_Cam_ReturnToPlayer(float _fDeltaTime);
    //트리거 카메라 함수
    void MoveMent_Trigger(float _fDeltaTime);
    void Trigger_Cam_Stage1_Wave1(float _fDeltaTime);
    void Trigger_Cam_Stage1_Wave1_End(float _fDeltaTime);
	void Trigger_Cam_Stage1_Wave2(float _fDeltaTime);
    void Trigger_Cam_Stage1_Wave2_Battle(float _fDeltaTime);
	void Trigger_Cam_Stage1_Wave2_End(float _fDeltaTime);
    void Trigger_Cam_Stage2_ButterFly1(float _fDeltaTime);
    void Trigger_Cam_Stage2_ButterFly1_End(float _fDeltaTime);
    void Trigger_Cam_Stage2_ButterFly2(float _fDeltaTime);
    void Trigger_Cam_Stage2_ButterFly2_End(float _fDeltaTime);
    //보스 카메라 함수
    void Boss_Cam_Em5000(float _fDeltaTime);
    void Boss_Cam_Em5300(float _fDeltaTime);
private:
    std::weak_ptr<Transform>        m_pAtTranform;
    std::weak_ptr<Nero>             m_pNero;
    std::weak_ptr<Trigger>          m_pTrigger;
    std::vector<weak_ptr<class Effect>> m_vecQliphothBlock;

    UINT                            m_eAtType = AT_PLAYER;
    UINT                            m_ePlayerCamMode = CAM_MODE_BASIC;
    UINT                            m_eTriggerCamMode = STAGE1_WAVE1;


    //////////////////////////////////////////
	Vector3	m_vEye, m_vAt, m_vUp,m_vLerpEye;
	float	m_fFovY, m_fAspect, m_fNear, m_fFar;
	Matrix	m_matView, m_matProj;


	float	m_fDistanceToTarget = 0.f;
    float   m_fAngle = 0.f;
    float   m_fSensitive = 0.f;
    bool	m_bFix = false;
    bool    m_bLerp = true;
    float   m_fRotX = 0.f;
    float   m_fFloatingAmount = 0.f;
    //////////////////////////////////////////
    float   m_fDecreaseFactor = 0.f;
    float   m_fIncreaseFactor = 0.f;

    float   m_fTriggerTime = 0.f;
    Vector3 m_vTriggerPos;
    Vector3 m_vTriggerAngle;

    float   m_fLerpSpeed = 0.f;
    float   m_fAtLerpTime = 0.f;

    float   m_fShakingTime = 0.f;
    float   m_fShakePower = 0.f;

    bool    m_bFadeOut = false;
    float   m_fFadeInAmout = 0.f;
};



#endif // MainCamera_h__