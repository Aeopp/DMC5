#include "Shader.h"

USING(ENGINE)

Shader::Shader(LPDIRECT3DDEVICE9 const _pDevice)
	: Resource(_pDevice)
	, m_pEffect(nullptr)
	, m_pErrMsg(nullptr)
{
}

Shader::Shader(const Shader& _rOther)
	: Resource(_rOther.m_pDevice)
	, m_pEffect(_rOther.m_pEffect)
	, m_pErrMsg(_rOther.m_pErrMsg)
{
	SafeAddRef(m_pEffect);
	SafeAddRef(m_pErrMsg);
}

void Shader::Free()
{
	Resource::Free();
	SafeRelease(m_pEffect);
	SafeRelease(m_pErrMsg);
}

Shader* Shader::Create(LPDIRECT3DDEVICE9 const _pDevice, const TSTRING& _sFilePath , const std::any& InitParams)
{
	Shader* pInstance = new Shader(_pDevice);

	if (FAILED(pInstance->CreateEffectFromFile(_sFilePath)))
	{
		pInstance->Free();
		delete pInstance;
		return nullptr;
	}
	return pInstance;
}

Resource* Shader::Clone()
{
	Shader* pClone = new Shader(*this);
	return pClone;
}

HRESULT Shader::CreateEffectFromFile(const TSTRING _sFilePath)
{
	if (FAILED(D3DXCreateEffectFromFile(m_pDevice, _sFilePath.c_str(), NULL, NULL, D3DXSHADER_DEBUG, NULL, &m_pEffect, &m_pErrMsg)))
	{
		MessageBoxA(NULL, (char*)m_pErrMsg->GetBufferPointer(), "Shader Error", MB_OK);
		return E_FAIL;
	}
	else if (nullptr != m_pErrMsg)
	{
		MessageBoxA(NULL, (char*)m_pErrMsg->GetBufferPointer(), "Shader Warning", MB_OK);
	}
	return S_OK;
}

LPD3DXEFFECT Shader::GetEffect()
{
	return m_pEffect;
}
