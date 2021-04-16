#ifndef __MATERIAL_H__
#define __MATERIAL_H__
#include "Object.h"
BEGIN(ENGINE)
class Texture;
class Material : public Object
{
public:
	std::string Name;
	// 텍스쳐 타입 -> 텍스쳐 인덱스 . 
	std::unordered_map</*aiTextureType*/UINT, std::vector<std::shared_ptr<Texture>>> Textures;
private:
	explicit Material();
	virtual	~Material() = default;
	// Object을(를) 통해 상속됨
	virtual void Free() override;
public:
	static Material* Create();
};
END
#endif // !__MATERIAL_H__