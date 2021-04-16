#include "imgui.h"
#include "Node.h"
USING(ENGINE)

void Node::Editor(std::string& RefRootMotionScaleName,
					std::string& RefRootMotionRotationName,
					std::string& RefRootMotionTransitionName)&
{
	if (ImGui::TreeNode(Name.c_str()))
	{
		if (ImGui::TreeNode("RootMotionSet"))
		{
			if (ImGui::Button("Scale"))
			{
				RefRootMotionScaleName = Name;
			}
			else if (ImGui::Button("Rotation"))
			{
				RefRootMotionRotationName = Name;
			}
			else if (ImGui::Button("Transition"))
			{
				RefRootMotionTransitionName = Name;
			}

			ImGui::TreePop();
		}
		for (auto& Children : Childrens)
		{
			Children->Editor
			(RefRootMotionScaleName,
				RefRootMotionRotationName , RefRootMotionTransitionName);
		}
		ImGui::TreePop();
	}
}

std::tuple<Vector3, Quaternion, Vector3>
Node::CurrentAnimationTransform(
	const AnimationTrack& AnimTrack,
	const double CurrentAnimationTime)
{
	Vector3 Scale{ 1,1,1 };
	Quaternion Quat{0,0,0,1};
	Vector3 Pos{ 0,0,0 };

	{
		auto Right = AnimTrack.ScaleTimeLine.upper_bound(CurrentAnimationTime);

		if (Right == AnimTrack.ScaleTimeLine.begin())
		{
			Scale = Right->second;
		}
		else if (Right == AnimTrack.ScaleTimeLine.end())
		{
			std::advance(Right, -1);
			Scale = Right->second;
		}
		else
		{
			auto Left = Right;
			std::advance(Left, -1);
			const double Interval = Right->first - Left->first;
			const float t = (CurrentAnimationTime - Left->first) / Interval;
			D3DXVec3Lerp(&Scale, &Left->second, &Right->second, t);
		}
	}

	{
		auto Right = AnimTrack.QuatTimeLine.upper_bound(CurrentAnimationTime);

		if (Right == AnimTrack.QuatTimeLine.begin())
		{
			Quat = Right->second;
		}
		else if (Right == AnimTrack.QuatTimeLine.end())
		{
			std::advance(Right, -1);
			Quat = Right->second;
		}
		else
		{
			auto Left = Right;
			std::advance(Left, -1);
			const double Interval = Right->first - Left->first;
			const float t = (CurrentAnimationTime - Left->first) / Interval;
			D3DXQuaternionSlerp
			(&Quat, &Left->second, &Right->second, t);
		}
	}

	{
		auto Right = AnimTrack.PosTimeLine.upper_bound(CurrentAnimationTime);

		if (Right == AnimTrack.PosTimeLine.begin())
		{
			Pos = Right->second;
		}
		else if (Right == AnimTrack.PosTimeLine.end())
		{
			std::advance(Right, -1);
			Pos = Right->second;
		}
		else
		{
			auto Left = Right;
			std::advance(Left, -1);
			const double Interval = Right->first - Left->first;
			const float t = (CurrentAnimationTime - Left->first) / Interval;
			D3DXVec3Lerp
			(&Pos, &Left->second, &Right->second, t);
		}
	}

	return { Scale,Quat,Pos };
};

Quaternion Node::CurrentAnimationQuaternion(const AnimationTrack& AnimTrack,
	const double CurrentAnimationTime)
{
	
	Quaternion Quat{};

	{
		auto Right = AnimTrack.QuatTimeLine.upper_bound(CurrentAnimationTime);

		if (Right == AnimTrack.QuatTimeLine.begin())
		{
			Quat = Right->second;
		}
		else if (Right == AnimTrack.QuatTimeLine.end())
		{
			std::advance(Right, -1);
			Quat = Right->second;
		}
		else
		{
			auto Left = Right;
			std::advance(Left, -1);
			const double Interval = Right->first - Left->first;
			const float t = (CurrentAnimationTime - Left->first) / Interval;
			D3DXQuaternionSlerp
			(&Quat, &Left->second, &Right->second, t);
		}
	}

	return Quat; 
}

Vector3 Node::CurrentAnimationScale(const AnimationTrack& AnimTrack, const double CurrentAnimationTime)
{
	Vector3 Scale{ 1,1,1 };

	{
		auto Right = AnimTrack.ScaleTimeLine.upper_bound(CurrentAnimationTime);

		if (Right == AnimTrack.ScaleTimeLine.begin())
		{
			Scale = Right->second;
		}
		else if (Right == AnimTrack.ScaleTimeLine.end())
		{
			std::advance(Right, -1);
			Scale = Right->second;
		}
		else
		{
			auto Left = Right;
			std::advance(Left, -1);
			const double Interval = Right->first - Left->first;
			const float t = (CurrentAnimationTime - Left->first) / Interval;
			D3DXVec3Lerp(&Scale, &Left->second, &Right->second, t);
		}
	}

	return Scale; 
}

Vector3 Node::CurrentAnimationPosition(const AnimationTrack& AnimTrack, const double CurrentAnimationTime)
{
	Vector3 Pos{ 0,0,0 };

	{
		auto Right = AnimTrack.PosTimeLine.upper_bound(CurrentAnimationTime);

		if (Right == AnimTrack.PosTimeLine.begin())
		{
			Pos = Right->second;
		}
		else if (Right == AnimTrack.PosTimeLine.end())
		{
			std::advance(Right, -1);
			Pos = Right->second;
		}
		else
		{
			auto Left = Right;
			std::advance(Left, -1);
			const double Interval = Right->first - Left->first;
			const float t = (CurrentAnimationTime - Left->first) / Interval;
			D3DXVec3Lerp
			(&Pos, &Left->second, &Right->second, t);
		}
	}

	return Pos;
}

void Node::NodeUpdate(const Matrix& ParentToRoot,
				     const double CurrentAnimationTime,
				     const std::string& AnimationName, 
	const std::optional<AnimationBlendInfo>& IsAnimationBlend ,
	const Quaternion& QuatOffset)&
{
	// 여기서 이전 프레임과 다음 프레임을 보간 한다.
	auto iter = _AnimationTrack.find(AnimationName);
	const bool bCurAnim = iter != std::end(_AnimationTrack);

	if (bCurAnim)
	{
		auto [Scale,Quat,Pos] = 
			CurrentAnimationTransform(iter->second, CurrentAnimationTime);

		if (IsAnimationBlend.has_value())
		{
			auto PrevIter =
				_AnimationTrack.find(IsAnimationBlend->PrevAnimationName);
			const bool bPrevAnim = PrevIter != std::end(_AnimationTrack);
			if (bPrevAnim)
			{
				auto [PrevScale, PrevQuat, PrevPos] =
					CurrentAnimationTransform(PrevIter->second,
						IsAnimationBlend->AnimationTime);

				const double BlendWeight =
					1.0 - IsAnimationBlend->PrevAnimationWeight;

				D3DXVec3Lerp(&Scale, &PrevScale, &Scale, BlendWeight);
				D3DXQuaternionSlerp(&Quat, &PrevQuat, &Quat, BlendWeight);
				D3DXVec3Lerp(&Pos, &PrevPos, &Pos, BlendWeight);
			}
		}

		// 포지션
		if (RootMotionFlag.test(2)/*"root_$AssimpFbx$_Transition"*/)
		{
			Pos = { 0,0,0 };
		}
		 // 로테이션 .. 
		if (RootMotionFlag.test(1))
		{
			// 오프셋 쿼터니언 켤레 사원수로 . 
			D3DXQuaternionConjugate(&Quat, &QuatOffset);
			// Quat = UnitQuat !! 
		}
		 // 스케일링
		if (RootMotionFlag.test(0)/*"root_$AssimpFbx$_Scaling"*/)
		{
			Scale = { 1,1,1 };
		}

		Transform = FMath::Scale(Scale) *
					FMath::Rotation(Quat) *
					FMath::Translation(Pos);
	}
	else
	{
		Transform = OriginTransform;
	}

	ToRoot = Transform * ParentToRoot;
	Final = Offset * ToRoot;

	for (auto& ChildrenTarget : Childrens)
	{
		ChildrenTarget->NodeUpdate(ToRoot,
			CurrentAnimationTime, 
			AnimationName,
			IsAnimationBlend ,
			QuatOffset);
	}
}


//
//
//void Engine::Bone::BoneEdit()
//{
//	if (ImGui::TreeNode(Name.c_str()))
//	{
//		bEditObserver = true;
//		if (ImGui::CollapsingHeader("Edit"))
//		{
//			bEditSelect = true;
//			ImGui::Button("Attach");
//			ImGui::Button("Detach");
//		}
//		else
//		{
//			bEditSelect = false;
//		}
//
//		for (const auto& _Children : Childrens)
//		{
//			_Children->BoneEdit();
//		}
//		ImGui::TreePop();
//	}
//	else
//	{
//		bEditObserver = false;
//	}
//};
//
//void Engine::Bone::DebugRender(
//	const Matrix& World,
//	IDirect3DDevice9* Device,
//	ID3DXMesh* const DebugMesh)&
//{
//	IDirect3DTexture9* CurColorTex{ nullptr };
//	auto& ResourceSys = ResourceSystem::Instance;
//
//	if (bEditSelect)
//	{
//		CurColorTex = ResourceSys->Get<IDirect3DTexture9>(L"Texture_Red");
//	}
//	else if (bEditObserver)
//	{
//		CurColorTex = ResourceSys->Get<IDirect3DTexture9>(L"Texture_Blue");
//	}
//	else
//	{
//		CurColorTex = ResourceSys->Get<IDirect3DTexture9>(L"Texture_Green");
//	}
//
//	const Matrix ToRootWorld = ToRoot * World;
//	const Matrix Final = FMath::Scale({ 5.f,5.f,5.f }) * ToRootWorld;
//	Device->SetTransform(D3DTS_WORLD, &Final);
//	Device->SetTexture(0, CurColorTex);
//	DebugMesh->DrawSubset(0);
//
//	if (CollisionGeometric)
//	{
//		Device->SetTransform(D3DTS_WORLD, &ToRootWorld);
//		CollisionGeometric->Render(Device, true);
//	}
//};
