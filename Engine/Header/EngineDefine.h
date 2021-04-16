#ifndef __ENGINE_DEFINE_H__
#define __ENGINE_DEFINE_H__

namespace ENGINE
{

#pragma region EXPORT_DLL
#ifdef ENGINE_EXPORT
#define ENGINE_DLL _declspec(dllexport)
#else
#define ENGINE_DLL _declspec(dllimport)
#endif // !ENGINE_EXPORT
#pragma endregion

#pragma region LOG
#define PRINT_LOG(caption, message) ::MessageBox(0, message, caption, MB_OK);
#pragma endregion

#pragma region NAMESPACE
#define BEGIN(Name) namespace Name {
#define END }
#define USING(Name) using namespace Name;
#pragma endregion

#pragma region PREVENT_COPY_CONSTRUCTOR
#define NO_COPY(ClassName)								\
private:												\
	ClassName(const ClassName&) = delete;				\
	ClassName& operator=(const ClassName&) = delete;
#pragma endregion

#pragma region SINGLETON

#define DECLARE_SINGLETON(ClassName)						\
		NO_COPY(ClassName)									\
private:													\
	static std::shared_ptr<ClassName>	m_pInstance;		\
	static std::once_flag				m_tOnceFlag;		\
public:														\
	static std::shared_ptr<ClassName>	GetInstance();		\
	static void							DeleteInstance();

#define	IMPLEMENT_SINGLETON(ClassName)																		\
std::shared_ptr<ClassName>	ClassName::m_pInstance = nullptr;												\
std::once_flag				ClassName::m_tOnceFlag;															\
std::shared_ptr<ClassName>	ClassName::GetInstance()														\
{																											\
	std::call_once(ClassName::m_tOnceFlag, []{ m_pInstance.reset(new ClassName, Deleter<Object>()); });		\
	return m_pInstance;																						\
}																											\
void						ClassName::DeleteInstance()														\
{																											\
	m_pInstance.reset();																					\
}
#pragma endregion

#pragma region PHYSICS
#define PX_RELEASE(x)	if(x)	{ x->release(); x = NULL;	}
#pragma endregion

}
#endif // !__ENGINE_DEFINE_H__