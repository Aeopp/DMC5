#ifndef __RESOURCES_HPP__
#define __RESOURCES_HPP__

BEGIN(ENGINE)
template<typename TYPE>
inline std::shared_ptr<TYPE> Resources::Load(const std::filesystem::path _Path , 
											const std::any& InitParams)
{
	if (true == m_pResourceSystem.expired())
		return std::shared_ptr<TYPE>();
	return m_pResourceSystem.lock()->Load<TYPE>(_Path,InitParams);
}
END
#endif // !__RESOURCES_HPP__
