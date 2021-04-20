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
	//���� ������ ���� ��� ���� �Ǵ� ���� ���� ����� �����ϰ� �ִ� ������ ���.
	D3DXMATRIX		m_matRender;

	//Physx���� Simulation�� ����� ����Ǿ����� ����.
	bool			m_bSimulation;
	//Transform�� ��ȭ�� �־����� �Ǵ��ϴ� ����.
	bool			m_bUpdated;
private:
	explicit Transform(std::weak_ptr<GameObject> const _pGameObject);
	virtual ~Transform() = default;
	// Component��(��) ���� ��ӵ�
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
	void	Rotate(const D3DXQUATERNION _tRotate);
public:
	static D3DXQUATERNION EulerToQuaternion(const D3DXVECTOR3 _vEuler);
	static D3DXVECTOR3 QuaternionToEuler(const D3DXQUATERNION _vQuat);
	static float NormalizeAngle(float _fAngle);
};
END
#endif // !__TRANSFORM_H__