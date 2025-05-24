// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "ClothFragmentGenerator.generated.h"

/**
 * 布料碎片生成器 - 简化版
 * 使用基本方法模拟布料碎片
 */
UCLASS(BlueprintType)
class CHAOSCLOTHBROKENEXT_API UClothFragmentGenerator : public UObject
{
	GENERATED_BODY()

public:
	UClothFragmentGenerator();

	/**
	 * 从骨骼网格体的布料中生成碎片
	 * @param SkeletalMeshComponent 目标骨骼网格体组件
	 * @param ImpactLocation 碰撞位置
	 * @param ImpactRadius 影响半径
	 * @param MaterialID 材质ID
	 * @param FragmentCount 生成的碎片数量
	 * @param MinSize 最小碎片尺寸
	 * @param MaxSize 最大碎片尺寸
	 * @return 是否成功生成碎片
	 */
	UFUNCTION(BlueprintCallable, Category = "Cloth Breaking")
	bool GenerateFragmentsFromCloth(USkeletalMeshComponent* SkeletalMeshComponent,
		const FVector& ImpactLocation, float ImpactRadius, int32 MaterialID,
		int32 FragmentCount, float MinSize, float MaxSize);

protected:
	/**
	 * 创建简单的碎片Actor
	 * @param WorldLocation 世界位置
	 * @param Size 碎片大小
	 * @param Material 材质
	 * @return 创建的Actor
	 */
	AActor* CreateSimpleFragment(const FVector& WorldLocation, float Size, UMaterialInterface* Material);

	/**
	 * 为碎片设置物理属性
	 * @param FragmentComponent 碎片组件
	 * @return 是否成功设置
	 */
	bool SetupFragmentPhysics(UPrimitiveComponent* FragmentComponent);

	/**
	 * 清理旧的碎片
	 */
	void CleanupOldFragments();

	/**
	 * 销毁碎片
	 * @param Fragment 要销毁的碎片Actor
	 */
	UFUNCTION()
	void DestroyFragment(AActor* Fragment);

private:
	/** 已生成的碎片列表 */
	UPROPERTY()
	TArray<AActor*> GeneratedFragments;
};
