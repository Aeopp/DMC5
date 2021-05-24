#ifndef __NERO_COAT_H__
#define __NERO_COAT_H__
#include "GameObject.H"
class Nero;
class NeroCoat : public GameObject
{
private:
	typedef struct tagClothBone
	{
		string			sName;
		D3DXMATRIX		matToRoot;
		tagClothBone* pParent;
		Node* pNode;
		UINT			nParticleIndex;
	}CLOTHBONE, * LPCLOTHBONE;
private:
	weak_ptr<Nero> m_pNero;
	weak_ptr<Transform> m_pNeroTransform;

	shared_ptr<SkeletonMesh> m_pMesh;

	/// <summary>
	/// PxCloth와 충돌할 PxClothCollisionSphere의 위치 설정에 사용될 Node 포인터를 담는 컨테이너.
	/// </summary>
	vector<Node*>	m_vecNode;

	/// <summary>
	/// 시뮬레이션 결과를 적용할 Node들을 보관하는 컨테이너
	/// </summary>
	vector<LPCLOTHBONE> m_vecClothBone;

	PxCloth* m_pCloth;


	D3DXQUATERNION	m_tPrevChestQuat;
	D3DXVECTOR3		m_vPrevPos;
private:
	explicit NeroCoat();
	virtual ~NeroCoat() = default;
	// GameObject을(를) 통해 상속됨
	virtual void Free() override;
public:
	static NeroCoat* Create();
public:
	virtual HRESULT Ready()								override;
	virtual HRESULT Awake()								override;
	virtual HRESULT Start()								override;
	virtual UINT	Update(const float _fDeltaTime)		override;
	virtual UINT	LateUpdate(const float _fDeltaTime) override;
	virtual void	OnEnable()							override;
	virtual void	OnDisable()							override;

	virtual void	Editor()override;

public:
	void SetMesh(shared_ptr<SkeletonMesh> _pMesh);
private:
	void CreateCloth();
};
#endif // !__NERO_COAT_H__
