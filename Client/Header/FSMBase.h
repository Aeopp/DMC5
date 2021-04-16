#ifndef FSMBase_h__
#define FSMBase_h__

#pragma once
using namespace std;
class FSMState;

class FSMBase
{
protected:
	vector<shared_ptr<FSMState>>	m_vecState;
	weak_ptr<FSMState>				m_pCurrentState;
	UINT							m_iCurrentIndex;

public:
	explicit FSMBase();
	virtual ~FSMBase();

protected:
	virtual HRESULT ReadyFSM()							PURE;
public:
	virtual HRESULT UpdateFSM(const float _fDeltaTime)	PURE;

protected:
	void	ReserveContainer(const UINT _nCount);
public:
	HRESULT AddState(FSMState* _pState, const UINT _nIndex);
	HRESULT ChangeState(const UINT _nIndex);
	UINT	GetCurrentIndex();

};



#endif // FSMBase_h__