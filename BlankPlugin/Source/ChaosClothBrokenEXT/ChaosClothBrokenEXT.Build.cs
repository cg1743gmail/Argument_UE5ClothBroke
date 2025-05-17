// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class ChaosClothBrokenEXT : ModuleRules
{
	public ChaosClothBrokenEXT(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicIncludePaths.AddRange(
			new string[] {
				// ... add public include paths required here ...
			}
			);


		PrivateIncludePaths.AddRange(
			new string[] {
				// ... add other private include paths required here ...
			}
			);


		PublicDependencyModuleNames.AddRange(
			new string[]
			{
				"Core",
				"ClothingSystemRuntimeInterface", // 布料系统运行时接口
				"ClothingSystemRuntimeCommon",    // 布料系统运行时通用功能
				"GeometryCore",                   // 几何核心库
				"GeometryScriptingCore",          // 几何脚本核心
				"DynamicMesh",                    // 动态网格
				// ... add other public dependencies that you statically link with here ...
			}
			);


		PrivateDependencyModuleNames.AddRange(
			new string[]
			{
				"CoreUObject",
				"Engine",
				"Slate",
				"SlateCore",
				"RenderCore",                     // 渲染核心
				"RHI",                            // 渲染硬件接口
				"PhysicsCore",                    // 物理核心
				"Chaos",                          // Chaos物理系统
				"GeometryFramework",              // 几何框架
				"MeshConversion",                 // 网格转换
				"MeshDescription",                // 网格描述
				"StaticMeshDescription",          // 静态网格描述
				// ... add private dependencies that you statically link with here ...
			}
			);


		DynamicallyLoadedModuleNames.AddRange(
			new string[]
			{
				// ... add any modules that your module loads dynamically here ...
			}
			);
	}
}
