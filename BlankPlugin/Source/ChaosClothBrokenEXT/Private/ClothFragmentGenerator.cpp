// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClothFragmentGenerator.h"
#include "GeometryScript/MeshPrimitiveFunctions.h"
#include "GeometryScript/MeshBasicEditFunctions.h"
#include "GeometryScript/MeshBooleanFunctions.h"
#include "GeometryScript/MeshSpatialFunctions.h"
#include "GeometryScript/MeshMaterialFunctions.h"
#include "DynamicMesh/MeshTransforms.h"
#include "Components/SkeletalMeshComponent.h"
#include "ClothingSimulationInteractor.h"
#include "ClothingAsset.h"
#include "PhysicsEngine/PhysicsAsset.h"
#include "Engine/StaticMeshActor.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
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

    // 提取布料网格
    UDynamicMesh* ClothMesh = NewObject<UDynamicMesh>(this);
    if (!ExtractDynamicMeshFromCloth(SkeletalMeshComponent, MaterialID, ClothMesh))
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to extract cloth mesh"));
        return false;
    }

    // 切割网格生成碎片
    TArray<UDynamicMesh*> Fragments;
    if (!CutMeshAtLocation(ClothMesh, ImpactLocation, ImpactRadius, FragmentCount, MinSize, MaxSize, Fragments))
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to cut mesh"));
        return false;
    }

    // 为每个碎片创建静态网格Actor
    FTransform WorldTransform = SkeletalMeshComponent->GetComponentTransform();
    UMaterialInterface* ClothMaterial = nullptr;

    // 尝试获取布料材质
    if (SkeletalMeshComponent->GetNumMaterials() > 0)
    {
        ClothMaterial = SkeletalMeshComponent->GetMaterial(0);
    }

    for (UDynamicMesh* Fragment : Fragments)
    {
        if (Fragment && Fragment->GetTriangleCount() > 0)
        {
            // 创建静态网格Actor
            AStaticMeshActor* FragmentActor = CreateFragmentActor(Fragment, WorldTransform, ClothMaterial);
            if (FragmentActor)
            {
                // 设置物理属性
                SetupFragmentPhysics(FragmentActor->GetStaticMeshComponent(), WorldTransform);

                // 添加到生成的碎片列表
                GeneratedFragments.Add(FragmentActor);
            }
        }
    }

    UE_LOG(LogTemp, Log, TEXT("Successfully generated %d fragments"), GeneratedFragments.Num());
    return GeneratedFragments.Num() > 0;
}

bool UClothFragmentGenerator::ExtractDynamicMeshFromCloth(USkeletalMeshComponent* SkeletalMeshComponent,
    int32 MaterialID, UDynamicMesh* OutDynamicMesh)
{
    if (!SkeletalMeshComponent || !OutDynamicMesh)
    {
        return false;
    }

    // 初始化动态网格
    OutDynamicMesh->Reset();

    // 获取骨骼网格体的当前姿势网格
    USkeletalMesh* SkelMesh = SkeletalMeshComponent->SkeletalMesh;
    if (!SkelMesh)
    {
        return false;
    }

    // 使用GeometryScript从骨骼网格体提取网格
    UGeometryScriptLibrary_MeshPrimitiveFunctions* MeshPrimLib =
        UGeometryScriptLibrary_MeshPrimitiveFunctions::GetInstance();

    if (!MeshPrimLib)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to get GeometryScriptLibrary_MeshPrimitiveFunctions instance"));
        return false;
    }

    // 创建一个临时的动态网格来存储完整的骨骼网格
    UDynamicMesh* TempFullMesh = NewObject<UDynamicMesh>(this);

    // 从骨骼网格体提取网格
    FGeometryScriptCopyMeshFromSkeletalMeshOptions Options;
    Options.bWantNormals = true;
    Options.bWantTangents = true;
    Options.bWantUVs = true;
    Options.bWantColors = true;
    Options.bWantMaterials = true;

    // 使用当前姿势
    Options.SpecificLOD = 0;
    Options.bUsePosedPosition = true;

    FTransform LocalToWorld = SkeletalMeshComponent->GetComponentTransform();
    MeshPrimLib->CopyMeshFromSkeletalMesh(TempFullMesh, SkelMesh, Options, LocalToWorld);

    // 如果指定了材质ID，则提取该材质ID的部分
    if (MaterialID != INDEX_NONE)
    {
        // 使用GeometryScript提取指定材质ID的部分
        UGeometryScriptLibrary_MeshMaterialFunctions* MeshMatLib =
            UGeometryScriptLibrary_MeshMaterialFunctions::GetInstance();

        if (MeshMatLib)
        {
            FGeometryScriptMeshMaterialFilterOptions FilterOptions;
            FilterOptions.MaterialID = MaterialID;

            MeshMatLib->CopyMeshByMaterialID(TempFullMesh, OutDynamicMesh, FilterOptions);
        }
        else
        {
            // 如果无法获取材质函数库，则使用完整网格
            OutDynamicMesh->Copy(TempFullMesh);
        }
    }
    else
    {
        // 如果没有指定材质ID，则使用完整网格
        OutDynamicMesh->Copy(TempFullMesh);
    }

    // 检查是否成功提取了网格
    if (OutDynamicMesh->GetTriangleCount() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to extract mesh from skeletal mesh"));
        return false;
    }

    return true;
}

bool UClothFragmentGenerator::CutMeshAtLocation(UDynamicMesh* DynamicMesh,
    const FVector& WorldLocation, float Radius, int32 FragmentCount,
    float MinSize, float MaxSize,
    TArray<UDynamicMesh*>& OutFragments)
{
    if (!DynamicMesh || DynamicMesh->GetTriangleCount() == 0)
    {
        return false;
    }

    // 获取GeometryScript库实例
    UGeometryScriptLibrary_MeshPrimitiveFunctions* MeshPrimLib =
        UGeometryScriptLibrary_MeshPrimitiveFunctions::GetInstance();

    UGeometryScriptLibrary_MeshBooleanFunctions* MeshBoolLib =
        UGeometryScriptLibrary_MeshBooleanFunctions::GetInstance();

    UGeometryScriptLibrary_MeshSpatialFunctions* MeshSpatialLib =
        UGeometryScriptLibrary_MeshSpatialFunctions::GetInstance();

    if (!MeshPrimLib || !MeshBoolLib || !MeshSpatialLib)
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to get GeometryScript library instances"));
        return false;
    }

    // 创建一个球体作为切割工具
    UDynamicMesh* CutterSphere = NewObject<UDynamicMesh>(this);
    FGeometryScriptPrimitiveOptions SphereOptions;
    SphereOptions.PolygroupMode = EGeometryScriptPrimitivePolygroupMode::PerFace;
    MeshPrimLib->AppendSphere(CutterSphere, SphereOptions, Radius, 16, 16);

    // 将球体移动到碰撞位置
    FTransform CutterTransform = FTransform(WorldLocation);
    MeshSpatialLib->TransformMesh(CutterSphere, CutterTransform);

    // 使用布尔操作切割网格
    UDynamicMesh* CutResult = NewObject<UDynamicMesh>(this);
    FGeometryScriptMeshBooleanOptions BooleanOptions;
    BooleanOptions.Operation = EGeometryScriptBooleanOperation::Difference;
    MeshBoolLib->ApplyMeshBoolean(DynamicMesh, CutterSphere, CutResult, BooleanOptions);

    // 获取切割区域内的网格
    UDynamicMesh* CutArea = NewObject<UDynamicMesh>(this);
    BooleanOptions.Operation = EGeometryScriptBooleanOperation::Intersect;
    MeshBoolLib->ApplyMeshBoolean(DynamicMesh, CutterSphere, CutArea, BooleanOptions);

    // 将切割结果添加到输出
    OutFragments.Add(CutResult);

    // 如果切割区域有效，则生成碎片
    if (CutArea && CutArea->GetTriangleCount() > 0)
    {
        // 确定实际生成的碎片数量
        int32 ActualFragmentCount = FMath::Min(FragmentCount, 10); // 限制最大碎片数量

        // 生成随机碎片
        for (int32 i = 0; i < ActualFragmentCount; ++i)
        {
            // 创建一个随机大小的球体作为碎片
            float FragmentSize = FMath::RandRange(MinSize, MaxSize);
            UDynamicMesh* FragmentMesh = NewObject<UDynamicMesh>(this);

            // 在切割区域内随机位置生成碎片
            FVector RandomOffset = FMath::VRand() * (Radius * 0.7f);
            FVector FragmentLocation = WorldLocation + RandomOffset;

            // 创建碎片网格
            MeshPrimLib->AppendSphere(FragmentMesh, SphereOptions, FragmentSize, 8, 8);

            // 将碎片移动到随机位置
            FTransform FragmentTransform = FTransform(FragmentLocation);
            MeshSpatialLib->TransformMesh(FragmentMesh, FragmentTransform);

            // 使用布尔操作获取碎片与切割区域的交集
            UDynamicMesh* Fragment = NewObject<UDynamicMesh>(this);
            BooleanOptions.Operation = EGeometryScriptBooleanOperation::Intersect;
            MeshBoolLib->ApplyMeshBoolean(CutArea, FragmentMesh, Fragment, BooleanOptions);

            // 如果生成的碎片有效，则添加到输出
            if (Fragment && Fragment->GetTriangleCount() > 0)
            {
                OutFragments.Add(Fragment);
            }
        }
    }

    return OutFragments.Num() > 0;
}

AStaticMeshActor* UClothFragmentGenerator::CreateFragmentActor(UDynamicMesh* FragmentMesh,
    const FTransform& WorldTransform, UMaterialInterface* Material)
{
    if (!FragmentMesh || !GetWorld())
    {
        return nullptr;
    }

    // 创建静态网格
    UStaticMesh* StaticMesh = NewObject<UStaticMesh>(this);

    // 使用GeometryScript将动态网格转换为静态网格
    UGeometryScriptLibrary_MeshAssetFunctions* MeshAssetLib =
        UGeometryScriptLibrary_MeshAssetFunctions::GetInstance();

    if (MeshAssetLib)
    {
        FGeometryScriptMeshWriteOptions WriteOptions;
        MeshAssetLib->CopyMeshToStaticMesh(FragmentMesh, StaticMesh, WriteOptions);
    }

    // 创建静态网格Actor
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    AStaticMeshActor* FragmentActor = GetWorld()->SpawnActor<AStaticMeshActor>(
        AStaticMeshActor::StaticClass(), WorldTransform, SpawnParams);

    if (FragmentActor)
    {
        // 设置静态网格
        UStaticMeshComponent* MeshComponent = FragmentActor->GetStaticMeshComponent();
        if (MeshComponent)
        {
            MeshComponent->SetStaticMesh(StaticMesh);

            // 设置材质
            if (Material)
            {
                // 创建动态材质实例
                UMaterialInstanceDynamic* DynMaterial = UMaterialInstanceDynamic::Create(Material, this);
                if (DynMaterial)
                {
                    MeshComponent->SetMaterial(0, DynMaterial);
                }
            }

            // 设置碰撞
            MeshComponent->SetCollisionProfileName(TEXT("PhysicsActor"));
            MeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
            MeshComponent->SetGenerateOverlapEvents(true);
        }

        // 设置Actor名称
        FragmentActor->SetActorLabel(FString::Printf(TEXT("ClothFragment_%d"), GeneratedFragments.Num()));

        // 设置自动销毁定时器
        float LifeTime = 5.0f; // 默认生命周期
        FTimerHandle TimerHandle;
        FTimerDelegate TimerDelegate;
        TimerDelegate.BindUFunction(this, FName("DestroyFragment"), FragmentActor);
        GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, LifeTime, false);
    }

    return FragmentActor;
}

bool UClothFragmentGenerator::SetupFragmentPhysics(UStaticMeshComponent* FragmentComponent,
    const FTransform& WorldTransform)
{
    if (!FragmentComponent)
    {
        return false;
    }

    // 启用物理模拟
    FragmentComponent->SetSimulatePhysics(true);

    // 设置物理材质
    UPhysicalMaterial* PhysMaterial = LoadObject<UPhysicalMaterial>(nullptr, TEXT("/Engine/EngineMaterials/DefaultPhysicalMaterial.DefaultPhysicalMaterial"));
    if (PhysMaterial)
    {
        FragmentComponent->SetPhysMaterialOverride(PhysMaterial);
    }

    // 设置质量和惯性
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
        GeneratedFragments.Remove(Cast<AStaticMeshActor>(Fragment));

        // 销毁Actor
        Fragment->Destroy();
    }
}
