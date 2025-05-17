// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

/**
 * ClothBroken模块
 * 提供布料断裂功能的扩展
 */
class FClothBrokenModule : public IModuleInterface
{
public:
    /** 获取模块单例 */
    static inline FClothBrokenModule& Get()
    {
        return FModuleManager::LoadModuleChecked<FClothBrokenModule>("ClothBroken");
    }

    /** 检查模块是否已加载 */
    static inline bool IsAvailable()
    {
        return FModuleManager::Get().IsModuleLoaded("ClothBroken");
    }

    /** IModuleInterface实现 */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
