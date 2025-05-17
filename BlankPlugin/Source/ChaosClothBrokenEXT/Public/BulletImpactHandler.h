// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/EngineTypes.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "BulletImpactHandler.generated.h"

/**
 * 处理子弹碰撞事件，计算断裂区域
 * 专门用于子弹击中布料时的断裂效果
 */
UCLASS(BlueprintType)
class CHAOSCLOTHBROKENEXT_API UBulletImpactHandler : public UObject
{
	GENERATED_BODY()

public:
	UBulletImpactHandler();

	/**
	 * 处理子弹碰撞事件
	 * @param HitResult 碰撞结果
	 * @param RadiusMultiplier 子弹大小到断裂半径的倍率
	 * @param OutImpactLocation 输出的碰撞位置
	 * @param OutBreakRadius 输出的断裂半径
	 * @param OutImpactForce 输出的碰撞力
	 * @return 是否成功处理碰撞
	 */
	UFUNCTION(BlueprintCallable, Category = "Cloth Breaking")
	bool ProcessBulletImpact(const FHitResult& HitResult, float RadiusMultiplier,
		FVector& OutImpactLocation, float& OutBreakRadius, float& OutImpactForce);

	/**
	 * 计算断裂区域半径
	 * @param HitComponent 被击中的组件
	 * @param BulletComponent 子弹组件
	 * @param RadiusMultiplier 半径倍率
	 * @return 断裂区域半径
	 */
	UFUNCTION(BlueprintCallable, Category = "Cloth Breaking")
	static float CalculateBreakRadius(UPrimitiveComponent* HitComponent,
		UPrimitiveComponent* BulletComponent, float RadiusMultiplier);

	/**
	 * 计算碰撞力
	 * @param HitResult 碰撞结果
	 * @return 碰撞力大小
	 */
	UFUNCTION(BlueprintCallable, Category = "Cloth Breaking")
	static float CalculateImpactForce(const FHitResult& HitResult);

	/**
	 * 模拟子弹碰撞
	 * @param TargetComponent 目标组件
	 * @param ImpactLocation 碰撞位置
	 * @param BulletSize 子弹大小
	 * @param ImpactForce 碰撞力
	 * @param RadiusMultiplier 半径倍率
	 * @param OutBreakRadius 输出的断裂半径
	 * @return 是否成功模拟碰撞
	 */
	UFUNCTION(BlueprintCallable, Category = "Cloth Breaking")
	static bool SimulateBulletImpact(UPrimitiveComponent* TargetComponent,
		const FVector& ImpactLocation, float BulletSize, float ImpactForce,
		float RadiusMultiplier, float& OutBreakRadius);

	/**
	 * 设置调试可视化
	 * @param bEnable 是否启用调试可视化
	 * @param DrawDuration 调试绘制持续时间
	 */
	UFUNCTION(BlueprintCallable, Category = "Cloth Breaking|Debug")
	void SetDebugVisualization(bool bEnable, float DrawDuration = 3.0f);

private:
	// 默认断裂半径
	static constexpr float DefaultRadius = 5.0f;

	// 是否启用调试可视化
	UPROPERTY()
	bool bEnableDebugVisualization;

	// 调试绘制持续时间
	UPROPERTY()
	float DebugDrawDuration;
};
