// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClothFragmentGenerator.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StaticMesh.h"
#include "TimerManager.h"

UClothFragmentGenerator::UClothFragmentGenerator()
{
    // 初始化成员变量
    GeneratedFragments.Empty();
}

bool UClothFragmentGenerator::GenerateFragmentsFromCloth(USkeletalMeshComponent* SkeletalMeshComponent,
    const FVector& ImpactLocation, float ImpactRadius, int32 MaterialID,
    int32 FragmentCount, float MinSize, float MaxSize)
{
    if (!SkeletalMeshComponent || !SkeletalMeshComponent->SkeletalMesh)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid skeletal mesh component"));
        return false;
    }

    // 清理之前生成的碎片
    CleanupOldFragments();

    // 获取材质
    UMaterialInterface* ClothMaterial = nullptr;
    if (SkeletalMeshComponent->GetNumMaterials() > 0)
    {
        // 如果指定了材质ID且有效，则使用该材质
        if (MaterialID >= 0 && MaterialID < SkeletalMeshComponent->GetNumMaterials())
        {
            ClothMaterial = SkeletalMeshComponent->GetMaterial(MaterialID);
        }
        else
        {
            // 否则使用第一个材质
            ClothMaterial = SkeletalMeshComponent->GetMaterial(0);
        }
    }

    // 生成随机碎片
    int32 ActualFragmentCount = FMath::Clamp(FragmentCount, 1, 20);
    for (int32 i = 0; i < ActualFragmentCount; ++i)
    {
        // 随机大小
        float FragmentSize = FMath::RandRange(MinSize, MaxSize);

        // 随机位置 (在碰撞半径内)
        FVector RandomOffset = FMath::VRand() * FMath::RandRange(0.0f, ImpactRadius * 0.8f);
        FVector FragmentLocation = ImpactLocation + RandomOffset;

        // 创建碎片
        AActor* Fragment = CreateSimpleFragment(FragmentLocation, FragmentSize, ClothMaterial);
        if (Fragment)
        {
            GeneratedFragments.Add(Fragment);
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Generated %d simple fragments"), GeneratedFragments.Num());
    return GeneratedFragments.Num() > 0;
}

AActor* UClothFragmentGenerator::CreateSimpleFragment(const FVector& WorldLocation, float Size, UMaterialInterface* Material)
{
    UWorld* World = GetWorld();
    if (!World)
    {
        return nullptr;
    }

    // 创建一个空的Actor
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AActor* FragmentActor = World->SpawnActor<AActor>(AActor::StaticClass(), FTransform(WorldLocation), SpawnParams);
    if (!FragmentActor)
    {
        return nullptr;
    }

    // 添加一个球体组件作为碎片
    USphereComponent* SphereComp = NewObject<USphereComponent>(FragmentActor, TEXT("SphereComp"));
    if (SphereComp)
    {
        SphereComp->SetSphereRadius(Size);
        SphereComp->SetCollisionProfileName(TEXT("PhysicsActor"));
        SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        SphereComp->SetGenerateOverlapEvents(true);

        // 设置材质
        if (Material)
        {
            UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(Material, this);
            if (DynMaterial)
            {
                SphereComp->SetMaterial(0, DynMaterial);
            }
        }

        // 注册组件
        SphereComp->RegisterComponent();

        // 设置为根组件
        FragmentActor->SetRootComponent(SphereComp);

        // 设置物理属性
        SetupFragmentPhysics(SphereComp);
    }

    // 设置Actor名称
    FragmentActor->SetActorLabel(FString::Printf(TEXT("ClothFragment_%d"), GeneratedFragments.Num()));

    // 设置自动销毁定时器
    float LifeTime = 5.0f; // 默认生命周期
    FTimerHandle TimerHandle;
    FTimerDelegate TimerDelegate;
    TimerDelegate.BindUFunction(this, FName("DestroyFragment"), FragmentActor);
    World->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, LifeTime, false);

    return FragmentActor;
}

bool UClothFragmentGenerator::SetupFragmentPhysics(UPrimitiveComponent* FragmentComponent)
{
    if (!FragmentComponent)
    {
        return false;
    }

    // 启用物理模拟
    FragmentComponent->SetSimulatePhysics(true);

    // 设置质量
    float Mass = 10.0f; // 默认质量
    FragmentComponent->SetMassOverrideInKg(NAME_None, Mass);

    // 添加初始冲量
    FVector RandomImpulse = FMath::VRand() * 100.0f;
    FragmentComponent->AddImpulse(RandomImpulse, NAME_None, true);

    return true;
}

void UClothFragmentGenerator::CleanupOldFragments()
{
    // 移除无效的碎片
    for (int32 i = GeneratedFragments.Num() - 1; i >= 0; --i)
    {
        if (!GeneratedFragments[i] || !GeneratedFragments[i]->IsValidLowLevel())
        {
            GeneratedFragments.RemoveAt(i);
        }
    }

    // 如果碎片数量超过限制，则销毁最旧的碎片
    const int32 MaxFragments = 50; // 最大碎片数量限制
    if (GeneratedFragments.Num() > MaxFragments)
    {
        int32 NumToRemove = GeneratedFragments.Num() - MaxFragments;
        for (int32 i = 0; i < NumToRemove; ++i)
        {
            if (GeneratedFragments[i] && GeneratedFragments[i]->IsValidLowLevel())
            {
                GeneratedFragments[i]->Destroy();
            }
        }
        GeneratedFragments.RemoveAt(0, NumToRemove);
    }
}

void UClothFragmentGenerator::DestroyFragment(AActor* Fragment)
{
    if (Fragment && Fragment->IsValidLowLevel())
    {
        // 从列表中移除
        GeneratedFragments.Remove(Fragment);

        // 销毁Actor
        Fragment->Destroy();
    }
}
