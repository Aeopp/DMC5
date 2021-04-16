#include "stdafx.h"
#include "FSMBase.h"
#include "FSMState.h"

FSMBase::FSMBase()
{
};

FSMBase::~FSMBase()
{
	for (auto& pState : m_vecState)
	{
		pState.reset();
	}
	m_vecState.clear();
	m_vecState.shrink_to_fit();
}

HRESULT FSMBase::ReadyFSM()
{
	return S_OK;
}

HRESULT FSMBase::UpdateFSM(const float _fDeltaTime)
{
	//if (nullptr == m_pCurrentState.lock())
	if (m_pCurrentState.expired()) //널체크 
		return S_OK;

	return m_pCurrentState.lock()->StateUpdate(_fDeltaTime);
}

void FSMBase::ReserveContainer(const UINT _nCount)
{
	//Vector 메모리 공간 확보.
	m_vecState.reserve(_nCount);
}

HRESULT FSMBase::AddState(FSMState* _pState, const UINT _nIndex)
{
	if (m_vecState.size() <= _nIndex || nullptr != m_vecState[_nIndex].get())
		return E_FAIL;

	m_vecState[_nIndex].reset(_pState);

	return S_OK;
}

HRESULT FSMBase::ChangeState(const UINT _nIndex)
{
	if (_nIndex > m_vecState.size())
		return E_FAIL;

	m_iCurrentIndex = _nIndex;

	if (m_pCurrentState.expired())
	{
		m_pCurrentState = m_vecState[_nIndex];
		m_pCurrentState.lock()->StateEnter();
	}
	else
	{
		if (_nIndex == m_pCurrentState.lock()->GetIndex())
			return S_OK;
		m_pCurrentState.lock()->StateExit();
		m_pCurrentState = m_vecState[_nIndex];
		m_pCurrentState.lock()->StateEnter();
	}
	return S_OK;
}

UINT FSMBase::GetCurrentIndex()
{
	return m_iCurrentIndex;
}
