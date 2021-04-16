#ifndef FSMState_h__
#define FSMState_h__

#pragma once
using namespace std;
class FSMBase;
class FSMState
{
protected:
	FSMBase*	m_pFSM;
	UINT		m_nIndex;

public:
	explicit FSMState(FSMBase* const _pFSM, const UINT _nIndex);
	virtual ~FSMState();

public:
	virtual HRESULT StateEnter()							PURE;
	virtual HRESULT StateExit()								PURE;
	virtual HRESULT StateUpdate(const float _fDeltaTime)	PURE;
public:
	HRESULT ChangeState(const UINT _nIndex);
public:
	UINT GetIndex();
};



#endif // FSMState_h__