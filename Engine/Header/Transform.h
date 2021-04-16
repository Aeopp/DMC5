#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__
#include "Component.h"
BEGIN(ENGINE)
class ENGINE_DLL Transform final : public Component
{
public:
	enum class SPACE { LOCAL, WORLD };
private:
	D3DXVECTOR3		m_vWorldScale;

	D3DXVECTOR3		m_vWorldRotation;
	D3DXQUATERNION	m_tWorldQuaternion;

	D3DXVECTOR3		m_vWorldPosition;

	D3DXMATRIX		m_matScale;
	D3DXMATRIX		m_matRotation;
	D3DXMATRIX		m_matTranlation;
	D3DXMATRIX		m_matWorld;

	D3DXMATRIX		m_matBillboard;
	//직전 프레임 월드 행렬 정보 또는 물리 연산 결과를 저장하고 있는 렌더용 행렬.
	D3DXMATRIX		m_matRender;

	//Physx에서 Simulation된 결과가 적용되었는지 여부.
	bool			m_bSimulation;
	//Transform에 변화가 있었는지 판단하는 변수.
	bool			m_bUpdated;
private:
	explicit Transform(std::weak_ptr<GameObject> const _pGameObject);
	virtual ~Transform() = default;
	// Component을(를) 통해 상속됨
	virtual void Free() override;
	virtual void Editor() override;
public:
	static Transform* Create(std::weak_ptr<GameObject> const _pGameObject);
public:
	void UpdateTransform();
	void SetSimulationResult(const D3DXQUATERNION& _tQuaternion, const D3DXVECTOR3& _vPosition);
public:
	virtual std::string GetName()override;
public:
	D3DXVECTOR3		GetScale(const SPACE& _eSpace = SPACE::WORLD);
	void			SetScale(const D3DXVECTOR3& _vScale, const SPACE& _eSpace = SPACE::WORLD);

	void			SetRotation(const D3DXVECTOR3& _vRotation, const SPACE& _eSpace = SPACE::WORLD);

	D3DXQUATERNION	GetQuaternion(const SPACE& _eSpace = SPACE::WORLD);
	void			SetQuaternion(const D3DXQUATERNION& _tQuaternion, const SPACE& _eSpace = SPACE::WORLD);

	D3DXVECTOR3		GetPosition(const SPACE& _eSpace = SPACE::WORLD);
	void			SetPosition(const D3DXVECTOR3& _vPosition, const SPACE& _eSpace = SPACE::WORLD);

	D3DXMATRIX		GetBiilBoard();
	void			SetBillBoard(const D3DXMATRIX& _matBillboard);

	D3DXMATRIX		GetWorldMatrix();
	void			SetWorldMatrix(const D3DXMATRIX& _matWorld);

	D3DXMATRIX		GetRenderMatrix();

	D3DXVECTOR3		GetRight();
	D3DXVECTOR3		GetUp();
	D3DXVECTOR3		GetLook();

	bool IsUpdated();
public:
	void	Translate(const D3DXVECTOR3 _vTranslate);
	void	Rotate(const D3DXVECTOR3 _vRotate);
public:
	static D3DXQUATERNION EulerToQuaternion(const D3DXVECTOR3 _vEuler);
	static D3DXVECTOR3 QuaternionToEuler(const D3DXQUATERNION _vQuat);
	static float NormalizeAngle(float _fAngle);
};
END
#endif // !__TRANSFORM_H__