#include "..\Header\Object.h"

USING(ENGINE)
static uint32 UniqueIDRegist = 0u;

Object::Object() :
	UniqueID { UniqueIDRegist++}
	, m_nTag(0)
{

}

Object::Object(const Object& rOther)
	: UniqueID{ UniqueIDRegist++ }
	, m_nTag(rOther.m_nTag)
{
}

void Object::Free()
{

}

void Object::Editor()
{
	EditName = "Edit " + GetName() + std::to_string(UniqueID);
	ImGui::Checkbox(EditName.c_str(), &bEdit);
}

std::string Object::GetName()
{
	return "Object";
}
