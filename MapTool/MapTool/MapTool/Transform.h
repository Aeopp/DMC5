#ifndef __TRANSFORM_H__
#define __TRANSFORM_H__
class  Transform
{
public:
	enum class SPACE { LOCAL, WORLD };
public:
	D3DXVECTOR3		m_vWorldScale;

	D3DXVECTOR3		m_vWorldRotation;
	D3DXQUATERNION	m_tWorldQuaternion;

	D3DXVECTOR3		m_vWorldPosition;

	D3DXMATRIX		m_matScale;
	D3DXMATRIX		m_matRotation;
	D3DXMATRIX		m_matTranlation;
	D3DXMATRIX		m_matWorld;

public:
	explicit Transform();
	virtual ~Transform();
public:
	D3DXVECTOR3		GetScale(const SPACE & _eSpace = SPACE::WORLD);
	void			SetScale(const D3DXVECTOR3 & _vScale, const SPACE & _eSpace = SPACE::WORLD);

	D3DXVECTOR3		GetRotation(const SPACE& _eSpace = SPACE::WORLD);
	void			SetRotation(const D3DXVECTOR3 & _vRotation, const SPACE & _eSpace = SPACE::WORLD);

	D3DXQUATERNION	GetQuaternion(const SPACE & _eSpace = SPACE::WORLD);
	void			SetQuaternion(const D3DXQUATERNION & _tQuaternion, const SPACE & _eSpace = SPACE::WORLD);

	D3DXVECTOR3		GetPosition(const SPACE & _eSpace = SPACE::WORLD);
	void			SetPosition(const D3DXVECTOR3 & _vPosition, const SPACE & _eSpace = SPACE::WORLD);

	D3DXMATRIX		GetWorldMatrix();
	void			SetWorldMatrix(const D3DXMATRIX & _matWorld);

	D3DXVECTOR3		GetRight();
	D3DXVECTOR3		GetUp();
	D3DXVECTOR3		GetLook();

};
#endif // !__TRANSFORM_H__