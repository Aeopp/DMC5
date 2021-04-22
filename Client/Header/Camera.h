#pragma once
#include "GameObject.h"

class TestObject;
class Em100;
class BtlPanel;
class Camera : public GameObject
{

private:
	D3DXVECTOR3 m_vRot;
private:
	explicit Camera();
	virtual ~Camera() = default;

	virtual void Free() override;

public:
	static Camera* Create();
	virtual HRESULT Ready() override;
	virtual HRESULT Awake() override;
	virtual HRESULT Start() override;
	virtual UINT Update(const float _fDeltaTime) override;
	virtual UINT LateUpdate(const float _fDeltaTime) override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;

	virtual std::string GetName() override;
	virtual void Editor() override;
public:
	 void MoveCamera();
public:
	 void UpdateCamera();

	 void ShowCameraInfo();

private:
	D3DXMATRIX	m_matView;
	D3DXMATRIX	m_matProjection;
	float		m_fFovY;
	float		m_fNear;
	float		m_fFar;

	float		m_fSensitivityMove;
	float		m_fSensitivityWheel;
	float		m_fSensitivityRot;

	float		m_fDistance;
};

