// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClothBreakableSettings.h"

UClothBreakableSettings::UClothBreakableSettings()
{
	// 通用设置默认值
	BreakForceThreshold = 1000.0f;

	// 子弹相关默认值
	RadiusMultiplier = 2.0f;

	// 碎片相关默认值
	MinFragmentCount = 3;
	MaxFragmentCount = 7;
	MinFragmentSize = 5.0f;
	MaxFragmentSize = 20.0f;
	FragmentLifetime = 5.0f;

	// 物理相关默认值
	bEnableFragmentPhysics = true;
	FragmentMass = 1.0f;
}
