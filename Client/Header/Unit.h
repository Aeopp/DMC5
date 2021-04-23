#ifndef Unit_h__
#define Unit_h__

#include "GameObject.h"
class Unit abstract :
    public GameObject
{
protected:
	explicit Unit();
	virtual ~Unit() = default;
	// GameObject��(��) ���� ��ӵ�
	virtual void Free() PURE;
public:
	virtual HRESULT Ready()								PURE;
	virtual HRESULT Awake()								PURE;
	virtual HRESULT Start()								PURE;
	virtual UINT	Update(const float _fDeltaTime)		PURE;
	virtual	UINT	LateUpdate(const float _fDeltaTime)	PURE;
	virtual void    Editor()                        override;
	virtual void	OnEnable()							PURE;
	virtual void	OnDisable()							PURE;
public:
	virtual void Hit(BT_INFO _BattleInfo, void* pArg = nullptr) PURE;
public:
	//GET
	BT_INFO Get_BattleInfo();
	bool	Get_Coll();
public:
	//SET
	void	Set_Coll(const bool _bColl);
protected:
	//���� ���õ� ����
	BT_INFO		m_BattleInfo;
	bool		m_bCollEnable;
};


#endif // Unit_h__