// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ClothBreakableSettings.h"
#include "ClothingSimulationInterface.h"
#include "BulletImpactHandler.h"
#include "ClothFragmentGenerator.h"
#include "ClothBreakableComponent.generated.h"

// 布料断裂事件委托
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnClothBreakEvent, USkeletalMeshComponent*, SkeletalMeshComponent,
	FVector, BreakLocation, float, BreakRadius, float, ImpactForce, int32, MaterialID);

/**
 * 管理布料断裂行为的组件
 * 专注于子弹碰撞断裂功能
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CHAOSCLOTHBROKENEXT_API UClothBreakableComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UClothBreakableComponent();

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** 布料断裂设置 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Breaking")
	UClothBreakableSettings* BreakableSettings;

	/** 目标骨骼网格体组件 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Breaking")
	USkeletalMeshComponent* TargetSkeletalMesh;

	/** 布料断裂事件 */
	UPROPERTY(BlueprintAssignable, Category = "Cloth Breaking")
	FOnClothBreakEvent OnClothBreak;

	/** 手动触发布料在指定位置断裂 */
	UFUNCTION(BlueprintCallable, Category = "Cloth Breaking")
	void ForceBreakClothAtLocation(FVector WorldLocation, float Radius);

	/** 设置材质ID是否可断裂 */
	UFUNCTION(BlueprintCallable, Category = "Cloth Breaking")
	void SetBreakableMaterialID(int32 MaterialID, bool bBreakable);

	/**
	 * 处理子弹碰撞事件
	 * @param HitResult 碰撞结果
	 * @return 是否成功处理碰撞
	 */
	UFUNCTION(BlueprintCallable, Category = "Cloth Breaking")
	bool HandleBulletHit(const FHitResult& HitResult);

	/**
	 * 模拟子弹碰撞
	 * @param ImpactLocation 碰撞位置
	 * @param BulletSize 子弹大小
	 * @param ImpactForce 碰撞力
	 * @return 是否成功模拟碰撞
	 */
	UFUNCTION(BlueprintCallable, Category = "Cloth Breaking")
	bool SimulateBulletImpact(FVector ImpactLocation, float BulletSize, float ImpactForce);

protected:
	/** 初始化可断裂布料 */
	void InitializeBreakableCloth();

	/** 在指定位置生成碎片 */
	void GenerateFragmentsAtLocation(const FVector& Location, float Radius, float ImpactForce);

	/** 检查位置是否在可断裂区域内 */
	bool IsLocationInBreakableRegion(const FVector& Location, int32& OutMaterialID);

	/** 注册碰撞事件 */
	void RegisterHitEvents();

	/** 碰撞事件回调 */
	UFUNCTION()
	void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
	// 是否已初始化
	bool bIsInitialized;

	// 碰撞事件处理器句柄
	FDelegateHandle ClothCollisionDelegateHandle;

	// 子弹碰撞处理器
	UPROPERTY()
	UBulletImpactHandler* BulletImpactHandler;

	// 碎片生成器
	UPROPERTY()
	UClothFragmentGenerator* FragmentGenerator;
};
