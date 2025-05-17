// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClothBroken.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "FClothBrokenModule"

void FClothBrokenModule::StartupModule()
{
    // 模块加载时的初始化代码
    UE_LOG(LogTemp, Log, TEXT("ClothBroken module has been loaded"));
}

void FClothBrokenModule::ShutdownModule()
{
    // 模块卸载时的清理代码
    UE_LOG(LogTemp, Log, TEXT("ClothBroken module has been unloaded"));
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FClothBrokenModule, ClothBroken)
