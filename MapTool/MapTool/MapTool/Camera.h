#ifndef __CAMERA_H__
#define __CAMERA_H__
class Transform;
class Camera
{
public:
	static Camera Instance;
public:
	D3DXMATRIX	m_matView;
	D3DXMATRIX	m_matProjection;
	float		m_fFovY;
	float		m_fNear;
	float		m_fFar;

	float		m_fSensitivityMove;
	float		m_fSensitivityWheel;
	float		m_fSensitivityRot;

	float		m_fDistance;
public:
	Transform* m_pTransform;
public:
	Camera();
	~Camera();
public:
	static void Update();
public:
	static void MoveCamera();
public:
	static void UpdateCamera();

	static void ShowCameraInfo();

	static void SetCameraFocus(D3DXVECTOR3 _vPosition);
};

#endif // !__CAMERA_H__
