// Copyright Epic Games, Inc. All Rights Reserved.

#include "ChaosClothBrokenEXT.h"
#include "ClothBreakableComponent.h"
#include "ClothFragmentGenerator.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FChaosClothBrokenEXTModule"

void FChaosClothBrokenEXTModule::StartupModule()
{
	// 模块加载时的初始化代码
	UE_LOG(LogTemp, Log, TEXT("ChaosClothBrokenEXT module has been loaded"));
}

void FChaosClothBrokenEXTModule::ShutdownModule()
{
	// 模块卸载时的清理代码
	UE_LOG(LogTemp, Log, TEXT("ChaosClothBrokenEXT module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FChaosClothBrokenEXTModule, ChaosClothBrokenEXT)