// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Modules/ModuleManager.h"

/**
 * ChaosClothBrokenEXT模块
 * 提供布料断裂功能的扩展
 */
class FChaosClothBrokenEXTModule : public IModuleInterface
{
public:
	/** 获取模块单例 */
	static inline FChaosClothBrokenEXTModule& Get()
	{
		return FModuleManager::LoadModuleChecked<FChaosClothBrokenEXTModule>("ChaosClothBrokenEXT");
	}

	/** 检查模块是否已加载 */
	static inline bool IsAvailable()
	{
		return FModuleManager::Get().IsModuleLoaded("ChaosClothBrokenEXT");
	}

	/** IModuleInterface实现 */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};
