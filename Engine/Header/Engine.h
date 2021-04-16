#ifndef __ENGINE_H__
#define __ENGINE_H__

#pragma region SYSTEM_INTERFACE
#include "Core.h"
#include "Graphic.h"
#include "Input.h"
#include "Timer.h"
#include "SceneManager.h"
#include "Resources.h"
#pragma endregion

#pragma region SCENE
#include "Scene.h"
#pragma endregion

#pragma region GAMEOBJECT
#include "GameObject.h"
#pragma endregion

#pragma region MyRegion
#include "Component.h"
#include "Transform.h"
#include "MeshRenderer.h"
#pragma endregion


#pragma region RESOURCE
#include "Texture.h"
#include "StaticMesh.h"
#include "SkeletonMesh.h"
#pragma endregion

#include "EngineStdafx.h"

#endif // !__ENGINE_H__

