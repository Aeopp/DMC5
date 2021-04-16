#include "stdafx.h"
#include "FSMState.h"
#include "FSMBase.h"

FSMState::FSMState(FSMBase* const _pFSM, const UINT _nIndex)
	: m_pFSM(_pFSM)
	, m_nIndex(_nIndex)
{
}

FSMState::~FSMState()
{
}

HRESULT FSMState::StateEnter()
{
	return S_OK;
}

HRESULT FSMState::StateExit()
{
	return S_OK;
}

HRESULT FSMState::StateUpdate(const float _fDeltaTime)
{
	return S_OK;
}

HRESULT FSMState::ChangeState(const UINT _nIndex)
{
	if (nullptr == m_pFSM)
		return E_FAIL;

	return m_pFSM->ChangeState(_nIndex);
}

UINT FSMState::GetIndex()
{
	return m_nIndex;
}
