#include "RenderInterface.h"
#include "Renderer.h"
#include "Resources.h"
#include "Shader.h"

USING(ENGINE)
void RenderInterface::Initialize(const RenderProperty& _RenderProp)&
{
	this->_RenderProperty = _RenderProp;
};
