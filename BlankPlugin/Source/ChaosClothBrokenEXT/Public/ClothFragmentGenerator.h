// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DynamicMesh/DynamicMesh.h"
#include "GeometryScript/GeometryScriptTypes.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/StaticMeshActor.h"
#include "ClothFragmentGenerator.generated.h"

/**
 * 布料碎片生成器
 * 使用Geometry Script生成布料碎片
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
	 * 从布料网格提取动态网格
	 * @param SkeletalMeshComponent 目标骨骼网格体组件
	 * @param MaterialID 材质ID
	 * @param OutDynamicMesh 输出的动态网格
	 * @return 是否成功提取
	 */
	bool ExtractDynamicMeshFromCloth(USkeletalMeshComponent* SkeletalMeshComponent,
		int32 MaterialID, UDynamicMesh* OutDynamicMesh);

	/**
	 * 在指定位置切割网格
	 * @param DynamicMesh 要切割的动态网格
	 * @param WorldLocation 世界空间中的切割位置
	 * @param Radius 切割半径
	 * @param FragmentCount 生成的碎片数量
	 * @param MinSize 最小碎片尺寸
	 * @param MaxSize 最大碎片尺寸
	 * @param OutFragments 输出的碎片网格列表
	 * @return 是否成功切割
	 */
	bool CutMeshAtLocation(UDynamicMesh* DynamicMesh,
		const FVector& WorldLocation, float Radius, int32 FragmentCount,
		float MinSize, float MaxSize,
		TArray<UDynamicMesh*>& OutFragments);

	/**
	 * 创建碎片Actor
	 * @param FragmentMesh 碎片网格
	 * @param WorldTransform 世界变换
	 * @param Material 材质
	 * @return 创建的静态网格Actor
	 */
	AStaticMeshActor* CreateFragmentActor(UDynamicMesh* FragmentMesh,
		const FTransform& WorldTransform, UMaterialInterface* Material);

	/**
	 * 为碎片设置物理属性
	 * @param FragmentComponent 碎片组件
	 * @param WorldTransform 世界变换
	 * @return 是否成功设置
	 */
	bool SetupFragmentPhysics(UStaticMeshComponent* FragmentComponent,
		const FTransform& WorldTransform);

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
	TArray<AStaticMeshActor*> GeneratedFragments;
};
