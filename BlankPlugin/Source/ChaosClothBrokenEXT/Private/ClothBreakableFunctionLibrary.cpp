// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClothBreakableFunctionLibrary.h"
#include "ClothBreakableComponent.h"
#include "ClothBreakableSettings.h"
#include "BulletImpactHandler.h"
#include "ClothingAsset.h"
#include "ClothingSimulationInteractor.h"

UClothBreakableComponent* UClothBreakableFunctionLibrary::AddClothBreakableToSkeletalMesh(USkeletalMeshComponent* SkeletalMeshComponent)
{
    if (!SkeletalMeshComponent || !SkeletalMeshComponent->GetOwner())
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid skeletal mesh component"));
        return nullptr;
    }

    // 检查是否已经有布料断裂组件
    UClothBreakableComponent* ExistingComponent = SkeletalMeshComponent->GetOwner()->FindComponentByClass<UClothBreakableComponent>();
    if (ExistingComponent)
    {
        // 如果已经存在，则更新目标骨骼网格体
        ExistingComponent->TargetSkeletalMesh = SkeletalMeshComponent;
        return ExistingComponent;
    }

    // 创建新的布料断裂组件
    UClothBreakableComponent* NewComponent = NewObject<UClothBreakableComponent>(SkeletalMeshComponent->GetOwner(), UClothBreakableComponent::StaticClass());
    if (NewComponent)
    {
        NewComponent->TargetSkeletalMesh = SkeletalMeshComponent;
        NewComponent->RegisterComponent();

        UE_LOG(LogTemp, Log, TEXT("Added cloth breakable component to %s"), *SkeletalMeshComponent->GetOwner()->GetName());
    }

    return NewComponent;
}

bool UClothBreakableFunctionLibrary::SetBreakableMaterialID(USkeletalMeshComponent* SkeletalMeshComponent, int32 MaterialID, bool bBreakable)
{
    if (!SkeletalMeshComponent || !SkeletalMeshComponent->GetOwner())
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid skeletal mesh component"));
        return false;
    }

    // 获取布料断裂组件
    UClothBreakableComponent* BreakableComponent = SkeletalMeshComponent->GetOwner()->FindComponentByClass<UClothBreakableComponent>();
    if (!BreakableComponent)
    {
        // 如果不存在，则创建一个
        BreakableComponent = AddClothBreakableToSkeletalMesh(SkeletalMeshComponent);
        if (!BreakableComponent)
        {
            return false;
        }
    }

    // 设置材质ID是否可断裂
    BreakableComponent->SetBreakableMaterialID(MaterialID, bBreakable);
    return true;
}

bool UClothBreakableFunctionLibrary::SetAllMaterialsBreakable(USkeletalMeshComponent* SkeletalMeshComponent)
{
    if (!SkeletalMeshComponent || !SkeletalMeshComponent->GetOwner())
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid skeletal mesh component"));
        return false;
    }

    // 获取布料断裂组件
    UClothBreakableComponent* BreakableComponent = SkeletalMeshComponent->GetOwner()->FindComponentByClass<UClothBreakableComponent>();
    if (!BreakableComponent || !BreakableComponent->BreakableSettings)
    {
        // 如果不存在，则创建一个
        BreakableComponent = AddClothBreakableToSkeletalMesh(SkeletalMeshComponent);
        if (!BreakableComponent || !BreakableComponent->BreakableSettings)
        {
            return false;
        }
    }

    // 清空现有的可断裂材质ID列表
    BreakableComponent->BreakableSettings->BreakableMaterialIDs.Empty();

    // 获取骨骼网格体的材质数量
    int32 MaterialCount = SkeletalMeshComponent->GetNumMaterials();

    // 将所有材质ID添加到可断裂列表
    for (int32 i = 0; i < MaterialCount; ++i)
    {
        BreakableComponent->BreakableSettings->BreakableMaterialIDs.Add(i);
    }

    UE_LOG(LogTemp, Log, TEXT("Set all %d materials as breakable"), MaterialCount);

    return true;
}

bool UClothBreakableFunctionLibrary::ForceBreakClothAtLocation(USkeletalMeshComponent* SkeletalMeshComponent, FVector WorldLocation, float Radius)
{
    if (!SkeletalMeshComponent || !SkeletalMeshComponent->GetOwner())
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid skeletal mesh component"));
        return false;
    }

    // 获取布料断裂组件
    UClothBreakableComponent* BreakableComponent = SkeletalMeshComponent->GetOwner()->FindComponentByClass<UClothBreakableComponent>();
    if (!BreakableComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("No cloth breakable component found"));
        return false;
    }

    // 触发断裂
    BreakableComponent->ForceBreakClothAtLocation(WorldLocation, Radius);
    return true;
}

bool UClothBreakableFunctionLibrary::SetClothBreakParameters(USkeletalMeshComponent* SkeletalMeshComponent,
    float BreakForceThreshold, int32 MinFragmentCount, int32 MaxFragmentCount,
    float MinFragmentSize, float MaxFragmentSize)
{
    if (!SkeletalMeshComponent || !SkeletalMeshComponent->GetOwner())
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid skeletal mesh component"));
        return false;
    }

    // 获取布料断裂组件
    UClothBreakableComponent* BreakableComponent = SkeletalMeshComponent->GetOwner()->FindComponentByClass<UClothBreakableComponent>();
    if (!BreakableComponent || !BreakableComponent->BreakableSettings)
    {
        // 如果不存在，则创建一个
        BreakableComponent = AddClothBreakableToSkeletalMesh(SkeletalMeshComponent);
        if (!BreakableComponent || !BreakableComponent->BreakableSettings)
        {
            return false;
        }
    }

    // 设置参数
    BreakableComponent->BreakableSettings->BreakForceThreshold = BreakForceThreshold;
    BreakableComponent->BreakableSettings->MinFragmentCount = FMath::Clamp(MinFragmentCount, 1, 10);
    BreakableComponent->BreakableSettings->MaxFragmentCount = FMath::Clamp(MaxFragmentCount, MinFragmentCount, 20);
    BreakableComponent->BreakableSettings->MinFragmentSize = MinFragmentSize;
    BreakableComponent->BreakableSettings->MaxFragmentSize = MaxFragmentSize;

    UE_LOG(LogTemp, Log, TEXT("Set cloth break parameters: Force=%f, Count=[%d, %d], Size=[%f, %f]"),
        BreakForceThreshold, MinFragmentCount, MaxFragmentCount, MinFragmentSize, MaxFragmentSize);

    return true;
}

bool UClothBreakableFunctionLibrary::SetBulletBreakParameters(USkeletalMeshComponent* SkeletalMeshComponent,
    float RadiusMultiplier)
{
    if (!SkeletalMeshComponent || !SkeletalMeshComponent->GetOwner())
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid skeletal mesh component"));
        return false;
    }

    // 获取布料断裂组件
    UClothBreakableComponent* BreakableComponent = SkeletalMeshComponent->GetOwner()->FindComponentByClass<UClothBreakableComponent>();
    if (!BreakableComponent || !BreakableComponent->BreakableSettings)
    {
        // 如果不存在，则创建一个
        BreakableComponent = AddClothBreakableToSkeletalMesh(SkeletalMeshComponent);
        if (!BreakableComponent || !BreakableComponent->BreakableSettings)
        {
            return false;
        }
    }

    // 设置子弹参数
    BreakableComponent->BreakableSettings->RadiusMultiplier = FMath::Max(RadiusMultiplier, 0.1f);

    UE_LOG(LogTemp, Log, TEXT("Set bullet break parameters: RadiusMultiplier=%f"),
        RadiusMultiplier);

    return true;
}

bool UClothBreakableFunctionLibrary::SimulateBulletImpact(USkeletalMeshComponent* SkeletalMeshComponent,
    FVector ImpactLocation, float BulletSize, float ImpactForce)
{
    if (!SkeletalMeshComponent || !SkeletalMeshComponent->GetOwner())
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid skeletal mesh component"));
        return false;
    }

    // 获取布料断裂组件
    UClothBreakableComponent* BreakableComponent = SkeletalMeshComponent->GetOwner()->FindComponentByClass<UClothBreakableComponent>();
    if (!BreakableComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("No cloth breakable component found"));
        return false;
    }

    // 模拟子弹碰撞
    return BreakableComponent->SimulateBulletImpact(ImpactLocation, BulletSize, ImpactForce);
}

bool UClothBreakableFunctionLibrary::HandleBulletHit(USkeletalMeshComponent* SkeletalMeshComponent,
    const FHitResult& HitResult)
{
    if (!SkeletalMeshComponent || !SkeletalMeshComponent->GetOwner())
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid skeletal mesh component"));
        return false;
    }

    // 获取布料断裂组件
    UClothBreakableComponent* BreakableComponent = SkeletalMeshComponent->GetOwner()->FindComponentByClass<UClothBreakableComponent>();
    if (!BreakableComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("No cloth breakable component found"));
        return false;
    }

    // 处理子弹碰撞
    return BreakableComponent->HandleBulletHit(HitResult);
}

bool UClothBreakableFunctionLibrary::SetFragmentPhysicsParameters(USkeletalMeshComponent* SkeletalMeshComponent,
    bool bEnablePhysics, float FragmentMass, float FragmentLifetime)
{
    if (!SkeletalMeshComponent || !SkeletalMeshComponent->GetOwner())
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid skeletal mesh component"));
        return false;
    }

    // 获取布料断裂组件
    UClothBreakableComponent* BreakableComponent = SkeletalMeshComponent->GetOwner()->FindComponentByClass<UClothBreakableComponent>();
    if (!BreakableComponent || !BreakableComponent->BreakableSettings)
    {
        // 如果不存在，则创建一个
        BreakableComponent = AddClothBreakableToSkeletalMesh(SkeletalMeshComponent);
        if (!BreakableComponent || !BreakableComponent->BreakableSettings)
        {
            return false;
        }
    }

    // 设置物理参数
    BreakableComponent->BreakableSettings->bEnableFragmentPhysics = bEnablePhysics;
    BreakableComponent->BreakableSettings->FragmentMass = FMath::Max(FragmentMass, 0.1f);
    BreakableComponent->BreakableSettings->FragmentLifetime = FMath::Max(FragmentLifetime, 0.1f);

    UE_LOG(LogTemp, Log, TEXT("Set fragment physics parameters: Enable=%d, Mass=%f, Lifetime=%f"),
        bEnablePhysics, FragmentMass, FragmentLifetime);

    return true;
}

bool UClothBreakableFunctionLibrary::EnableDebugVisualization(USkeletalMeshComponent* SkeletalMeshComponent,
    bool bEnable, float DrawDuration)
{
    if (!SkeletalMeshComponent || !SkeletalMeshComponent->GetOwner())
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid skeletal mesh component"));
        return false;
    }

    // 获取布料断裂组件
    UClothBreakableComponent* BreakableComponent = SkeletalMeshComponent->GetOwner()->FindComponentByClass<UClothBreakableComponent>();
    if (!BreakableComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("No cloth breakable component found"));
        return false;
    }

    // 获取子弹碰撞处理器
    UBulletImpactHandler* BulletHandler = nullptr;

    // 使用反射获取私有成员
    UProperty* BulletHandlerProperty = UClothBreakableComponent::StaticClass()->FindPropertyByName(TEXT("BulletImpactHandler"));
    if (BulletHandlerProperty)
    {
        UObjectProperty* ObjectProperty = Cast<UObjectProperty>(BulletHandlerProperty);
        if (ObjectProperty)
        {
            void* PropertyAddress = BulletHandlerProperty->ContainerPtrToValuePtr<void>(BreakableComponent);
            BulletHandler = Cast<UBulletImpactHandler>(ObjectProperty->GetObjectPropertyValue(PropertyAddress));
        }
    }

    // 如果无法通过反射获取，则尝试创建一个新的
    if (!BulletHandler)
    {
        BulletHandler = NewObject<UBulletImpactHandler>(BreakableComponent);
    }

    if (BulletHandler)
    {
        // 设置调试可视化
        BulletHandler->SetDebugVisualization(bEnable, DrawDuration);

        UE_LOG(LogTemp, Log, TEXT("Set debug visualization: Enable=%d, Duration=%f"),
            bEnable, DrawDuration);

        return true;
    }

    return false;
}

bool UClothBreakableFunctionLibrary::GetSkeletalMeshMaterialIDs(USkeletalMeshComponent* SkeletalMeshComponent,
    TArray<int32>& OutMaterialIDs)
{
    if (!SkeletalMeshComponent || !SkeletalMeshComponent->SkeletalMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid skeletal mesh component"));
        return false;
    }

    // 清空输出数组
    OutMaterialIDs.Empty();

    // 获取材质数量
    int32 MaterialCount = SkeletalMeshComponent->GetNumMaterials();

    // 添加所有材质ID
    for (int32 i = 0; i < MaterialCount; ++i)
    {
        OutMaterialIDs.Add(i);
    }

    return OutMaterialIDs.Num() > 0;
}

bool UClothBreakableFunctionLibrary::HasCloth(USkeletalMeshComponent* SkeletalMeshComponent)
{
    if (!SkeletalMeshComponent || !SkeletalMeshComponent->SkeletalMesh)
    {
        return false;
    }

    // 检查是否有布料模拟
    if (SkeletalMeshComponent->ClothingSimulationInteractor)
    {
        return true;
    }

    // 检查骨骼网格体是否有布料资产
    USkeletalMesh* SkelMesh = SkeletalMeshComponent->SkeletalMesh;
    if (SkelMesh)
    {
        // 在UE5中，可以通过以下方式检查布料资产
        TArray<UClothingAssetBase*> ClothingAssets = SkelMesh->GetMeshClothingAssets();
        return ClothingAssets.Num() > 0;
    }

    return false;
}
