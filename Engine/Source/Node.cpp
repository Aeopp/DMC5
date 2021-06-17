#include "imgui.h"
#include "Node.h"
USING(ENGINE)

void Node::Editor(std::string& RefRootMotionScaleName,
					std::string& RefRootMotionRotationName,
					std::string& RefRootMotionTransitionName)&
{
	if (ImGui::TreeNode(Name.c_str()))
	{
		for (uint32 Row = 0u; Row < 4u; ++Row)
		{
			for (uint32 Col = 0u; Col < 4u; ++Col)
			{
				ImGui::Text("%9.9f", Transform(Row, Col));
			}
			ImGui::Separator();
		}
		ImGui::Separator();

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
	const std::optional<AnimationBlendInfo>& IsAnimationBlend,
	const Quaternion& QuatOffset)&
{
	// 여기서 이전 프레임과 다음 프레임을 보간 한다.
	auto iter = _AnimationTrack.find(AnimationName);
	const bool bCurAnim = iter != std::end(_AnimationTrack);

	if (bCurAnim)
	{
		auto [Scale, Quat, Pos] =
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

		if (RootMotionFlag.test(RootMotionFlag::Transition))
		{
			Pos = { 0,0,0 };
		}
		if (RootMotionFlag.test(RootMotionFlag::Rotation))
		{
			// 오프셋 쿼터니언 켤레 사원수로 변환. 
			D3DXQuaternionConjugate(&Quat, &QuatOffset);
		}
		if (RootMotionFlag.test(RootMotionFlag::Scale))
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

	if (ClothTransform.has_value())
		Transform = ClothTransform.value();

	ToRoot = Transform * ParentToRoot;

	Final = Offset * ToRoot;

	for (auto& ChildrenTarget : Childrens)
	{
		ChildrenTarget->NodeUpdate(ToRoot,
			CurrentAnimationTime,
			AnimationName,
			IsAnimationBlend,
			QuatOffset);
	}
};
