#include "Resource.h"

USING(ENGINE)

Resource::Resource(LPDIRECT3DDEVICE9 const _pDevice)
	: m_pDevice(_pDevice)
	, bClone(FALSE)
	, m_nRefCount(0)
{
	SafeAddRef(m_pDevice);
}

Resource::Resource(const Resource& _rOther)
	: Object (_rOther) ,
	  m_pDevice(_rOther.m_pDevice) ,
	ResourcePath{ _rOther.ResourcePath },
	m_nRefCount(0)
{
	SafeAddRef(m_pDevice);
}

void Resource::Free()
{
	SafeRelease(m_pDevice);
	
	if (false == m_pOrigin.expired())
		m_pOrigin.lock()->Release();

	Object::Free();
}

void Resource::Editor()
{
	Object::Editor();
	if (bEdit)
	{
		ImGui::Text("Resource Path : %s", ResourcePath.string().c_str());
	}
}

void Resource::AddRef()
{
	++m_nRefCount;
}

void Resource::Release()
{
	--m_nRefCount;
}

int Resource::GetRefCount()
{
	return m_nRefCount;
}

