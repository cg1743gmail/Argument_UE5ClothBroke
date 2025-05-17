// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "ClothBreakableSettings.generated.h"

/**
 * 存储布料断裂设置
 * 专注于子弹碰撞断裂功能
 */
UCLASS(BlueprintType)
class CHAOSCLOTHBROKENEXT_API UClothBreakableSettings : public UObject
{
	GENERATED_BODY()

public:
	UClothBreakableSettings();

	/** 可断裂区域的材质ID列表 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Breaking|General")
	TArray<int32> BreakableMaterialIDs;

	/** 触发断裂的力阈值 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Breaking|General", meta = (ClampMin = "0.0"))
	float BreakForceThreshold;

	/** 子弹大小到断裂半径的倍率 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Breaking|Bullet", meta = (ClampMin = "0.1"))
	float RadiusMultiplier;

	/** 断裂时生成的最小碎片数量 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Breaking|Fragments", meta = (ClampMin = "1", ClampMax = "10"))
	int32 MinFragmentCount;

	/** 断裂时生成的最大碎片数量 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Breaking|Fragments", meta = (ClampMin = "1", ClampMax = "20"))
	int32 MaxFragmentCount;

	/** 碎片最小尺寸 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Breaking|Fragments", meta = (ClampMin = "0.1"))
	float MinFragmentSize;

	/** 碎片最大尺寸 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Breaking|Fragments", meta = (ClampMin = "0.1"))
	float MaxFragmentSize;

	/** 碎片生命周期 (秒) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Breaking|Fragments", meta = (ClampMin = "0.1"))
	float FragmentLifetime;

	/** 是否启用碎片物理模拟 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Breaking|Physics")
	bool bEnableFragmentPhysics;

	/** 碎片物理质量 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cloth Breaking|Physics", meta = (EditCondition = "bEnableFragmentPhysics", ClampMin = "0.1"))
	float FragmentMass;
};
