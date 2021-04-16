#ifndef _NODE_H_
#define _NODE_H_
#include "EngineDefine.h"
#include "EngineTypedef.h"
#include "FMath.hpp"
#include "AnimationBlendInfo.h"
#include <unordered_map>
#include <vector>
#include <bitset>

#include <memory>
#include <map>
#include "AnimationTrack.h"


BEGIN(ENGINE)
class ENGINE_DLL Node
{
public:
	Node* Parent{};
	std::vector<Node*>    Childrens{};
	Matrix OriginTransform{ FMath::Identity() };
	Matrix Final{ FMath::Identity() };
	Matrix Transform{ FMath::Identity() };
	Matrix ToRoot{ FMath::Identity() };
	Matrix Offset{ FMath::Identity() };
	std::string Name{};
	int32 Index{ -1 };
	std::bitset<3> RootMotionFlag{};

	


	inline bool IsBone() { return Index >=0; };

	std::unordered_map<std::string,AnimationTrack> _AnimationTrack{};

	static std::tuple<Vector3, Quaternion, Vector3>
		CurrentAnimationTransform(
			const AnimationTrack& AnimTrack,
			const double CurrentAnimationTime);

	static Vector3
		CurrentAnimationScale(
			const AnimationTrack& AnimTrack,
			const double CurrentAnimationTime);
	static  Quaternion
		CurrentAnimationQuaternion(
			const AnimationTrack& AnimTrack,
			const double CurrentAnimationTime);
	
	static Vector3
		CurrentAnimationPosition(
			const AnimationTrack& AnimTrack,
			const double CurrentAnimationTime);


	void NodeUpdate(
		const Matrix& ParentToRoot,
		const double CurrentAnimationTime,
		const std::string& AnimationName,
		const std::optional<AnimationBlendInfo>& IsAnimationBlend ,
		const Quaternion& QuatOffset = Quaternion{0,0,0,1})&;

	void Editor(std::string& RefRootMotionScaleName,
				std::string& RefRootMotionRotationName,
				std::string& RefRootMotionTransitionName)&;
};
END

#endif // !_NODE_H_




