// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ClothBreakableComponent.h"
#include "Engine/EngineTypes.h"
#include "ClothBreakableFunctionLibrary.generated.h"

/**
 * 布料断裂功能的蓝图函数库
 * 专注于子弹碰撞断裂功能
 */
UCLASS()
class CHAOSCLOTHBROKENEXT_API UClothBreakableFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * 为骨骼网格体添加布料断裂组件
	 * @param SkeletalMeshComponent 目标骨骼网格体组件
	 * @return 新创建的布料断裂组件
	 */
	UFUNCTION(BlueprintCallable, Category = "Cloth Breaking|Setup")
	static UClothBreakableComponent* AddClothBreakableToSkeletalMesh(USkeletalMeshComponent* SkeletalMeshComponent);

	/**
	 * 设置材质ID是否可断裂
	 * @param SkeletalMeshComponent 目标骨骼网格体组件
	 * @param MaterialID 材质ID
	 * @param bBreakable 是否可断裂
	 * @return 是否成功设置
	 */
	UFUNCTION(BlueprintCallable, Category = "Cloth Breaking|Setup")
	static bool SetBreakableMaterialID(USkeletalMeshComponent* SkeletalMeshComponent, int32 MaterialID, bool bBreakable);

	/**
	 * 设置所有材质可断裂
	 * @param SkeletalMeshComponent 目标骨骼网格体组件
	 * @return 是否成功设置
	 */
	UFUNCTION(BlueprintCallable, Category = "Cloth Breaking|Setup")
	static bool SetAllMaterialsBreakable(USkeletalMeshComponent* SkeletalMeshComponent);

	/**
	 * 手动触发布料在指定位置断裂
	 * @param SkeletalMeshComponent 目标骨骼网格体组件
	 * @param WorldLocation 世界空间中的位置
	 * @param Radius 影响半径
	 * @return 是否成功触发断裂
	 */
	UFUNCTION(BlueprintCallable, Category = "Cloth Breaking|Manual")
	static bool ForceBreakClothAtLocation(USkeletalMeshComponent* SkeletalMeshComponent, FVector WorldLocation, float Radius);

	/**
	 * 设置布料断裂基本参数
	 * @param SkeletalMeshComponent 目标骨骼网格体组件
	 * @param BreakForceThreshold 断裂力阈值
	 * @param MinFragmentCount 最小碎片数量
	 * @param MaxFragmentCount 最大碎片数量
	 * @param MinFragmentSize 最小碎片尺寸
	 * @param MaxFragmentSize 最大碎片尺寸
	 * @return 是否成功设置
	 */
	UFUNCTION(BlueprintCallable, Category = "Cloth Breaking|Setup")
	static bool SetClothBreakParameters(USkeletalMeshComponent* SkeletalMeshComponent,
		float BreakForceThreshold, int32 MinFragmentCount, int32 MaxFragmentCount,
		float MinFragmentSize, float MaxFragmentSize);

	/**
	 * 设置子弹断裂参数
	 * @param SkeletalMeshComponent 目标骨骼网格体组件
	 * @param RadiusMultiplier 子弹大小到断裂半径的倍率
	 * @return 是否成功设置
	 */
	UFUNCTION(BlueprintCallable, Category = "Cloth Breaking|Bullet")
	static bool SetBulletBreakParameters(USkeletalMeshComponent* SkeletalMeshComponent,
		float RadiusMultiplier);

	/**
	 * 模拟子弹碰撞
	 * @param SkeletalMeshComponent 目标骨骼网格体组件
	 * @param ImpactLocation 碰撞位置
	 * @param BulletSize 子弹大小
	 * @param ImpactForce 碰撞力
	 * @return 是否成功模拟碰撞
	 */
	UFUNCTION(BlueprintCallable, Category = "Cloth Breaking|Bullet")
	static bool SimulateBulletImpact(USkeletalMeshComponent* SkeletalMeshComponent,
		FVector ImpactLocation, float BulletSize, float ImpactForce);

	/**
	 * 处理子弹碰撞事件
	 * @param SkeletalMeshComponent 目标骨骼网格体组件
	 * @param HitResult 碰撞结果
	 * @return 是否成功处理碰撞
	 */
	UFUNCTION(BlueprintCallable, Category = "Cloth Breaking|Bullet")
	static bool HandleBulletHit(USkeletalMeshComponent* SkeletalMeshComponent,
		const FHitResult& HitResult);

	/**
	 * 设置碎片物理参数
	 * @param SkeletalMeshComponent 目标骨骼网格体组件
	 * @param bEnablePhysics 是否启用物理
	 * @param FragmentMass 碎片质量
	 * @param FragmentLifetime 碎片生命周期
	 * @return 是否成功设置
	 */
	UFUNCTION(BlueprintCallable, Category = "Cloth Breaking|Physics")
	static bool SetFragmentPhysicsParameters(USkeletalMeshComponent* SkeletalMeshComponent,
		bool bEnablePhysics, float FragmentMass, float FragmentLifetime);

	/**
	 * 启用调试可视化
	 * @param SkeletalMeshComponent 目标骨骼网格体组件
	 * @param bEnable 是否启用调试可视化
	 * @param DrawDuration 调试绘制持续时间
	 * @return 是否成功设置
	 */
	UFUNCTION(BlueprintCallable, Category = "Cloth Breaking|Debug")
	static bool EnableDebugVisualization(USkeletalMeshComponent* SkeletalMeshComponent,
		bool bEnable, float DrawDuration = 3.0f);

	/**
	 * 获取骨骼网格体的材质ID列表
	 * @param SkeletalMeshComponent 目标骨骼网格体组件
	 * @param OutMaterialIDs 输出的材质ID列表
	 * @return 是否成功获取
	 */
	UFUNCTION(BlueprintCallable, Category = "Cloth Breaking|Utility")
	static bool GetSkeletalMeshMaterialIDs(USkeletalMeshComponent* SkeletalMeshComponent,
		TArray<int32>& OutMaterialIDs);

	/**
	 * 检查骨骼网格体是否有布料
	 * @param SkeletalMeshComponent 目标骨骼网格体组件
	 * @return 是否有布料
	 */
	UFUNCTION(BlueprintCallable, Category = "Cloth Breaking|Utility")
	static bool HasCloth(USkeletalMeshComponent* SkeletalMeshComponent);
};
