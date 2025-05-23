// Copyright Epic Games, Inc. All Rights Reserved.

#include "ClothBreakableComponent.h"
#include "ClothingSimulationInteractor.h"
#include "Components/SkeletalMeshComponent.h"
#include "BulletImpactHandler.h"
#include "ClothFragmentGenerator.h"

UClothBreakableComponent::UClothBreakableComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bIsInitialized = false;

	// 创建默认设置对象
	BreakableSettings = CreateDefaultSubobject<UClothBreakableSettings>(TEXT("BreakableSettings"));
}

void UClothBreakableComponent::BeginPlay()
{
	Super::BeginPlay();

	// 在BeginPlay中初始化布料断裂系统
	InitializeBreakableCloth();

	// 注册碰撞事件
	RegisterHitEvents();
}

void UClothBreakableComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 清理资源
	if (TargetSkeletalMesh)
	{
		// 移除碰撞事件监听
		TargetSkeletalMesh->OnComponentHit.RemoveDynamic(this, &UClothBreakableComponent::OnComponentHit);
	}

	Super::EndPlay(EndPlayReason);
}

void UClothBreakableComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// 如果尚未初始化且目标骨骼网格体有效，则尝试初始化
	if (!bIsInitialized && TargetSkeletalMesh && TargetSkeletalMesh->IsValidLowLevel())
	{
		InitializeBreakableCloth();
		RegisterHitEvents();
	}
}

void UClothBreakableComponent::InitializeBreakableCloth()
{
	if (!TargetSkeletalMesh || !BreakableSettings)
	{
		return;
	}

	// 创建子弹碰撞处理器
	if (!BulletImpactHandler)
	{
		BulletImpactHandler = NewObject<UBulletImpactHandler>(this);
	}

	// 创建碎片生成器
	if (!FragmentGenerator)
	{
		FragmentGenerator = NewObject<UClothFragmentGenerator>(this);
	}

	// 检查布料模拟是否激活（可根据实际需求调整）
	bIsInitialized = true;
	UE_LOG(LogTemp, Log, TEXT("ClothBreakableComponent initialized successfully"));
}

void UClothBreakableComponent::RegisterHitEvents()
{
	if (!TargetSkeletalMesh)
	{
		return;
	}

	// 注册碰撞事件
	TargetSkeletalMesh->OnComponentHit.AddDynamic(this, &UClothBreakableComponent::OnComponentHit);

	// 确保启用碰撞
	TargetSkeletalMesh->SetNotifyRigidBodyCollision(true);

	UE_LOG(LogTemp, Log, TEXT("Registered hit events for cloth breakable component"));
}

void UClothBreakableComponent::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// 检查是否是子弹碰撞
	if (!OtherActor || !OtherComp)
	{
		return;
	}

	// 简单的子弹检测逻辑，可以根据实际需求调整
	// 例如，可以检查子弹的标签、类型等
	bool bIsBullet = OtherActor->ActorHasTag(FName("Bullet")) ||
		OtherActor->GetClass()->GetName().Contains(TEXT("Bullet")) ||
		OtherActor->GetName().Contains(TEXT("Bullet"));

	if (bIsBullet)
	{
		// 处理子弹碰撞
		HandleBulletHit(Hit);
	}
}

bool UClothBreakableComponent::HandleBulletHit(const FHitResult& HitResult)
{
	if (!bIsInitialized || !TargetSkeletalMesh || !BulletImpactHandler || !BreakableSettings)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot handle bullet hit: component not properly initialized"));
		return false;
	}

	// 处理子弹碰撞
	FVector ImpactLocation;
	float BreakRadius;
	float ImpactForce;

	if (!BulletImpactHandler->ProcessBulletImpact(HitResult, BreakableSettings->RadiusMultiplier,
		ImpactLocation, BreakRadius, ImpactForce))
	{
		return false;
	}

	// 检查碰撞力是否超过阈值
	if (ImpactForce < BreakableSettings->BreakForceThreshold)
	{
		UE_LOG(LogTemp, Log, TEXT("Bullet impact force (%f) below threshold (%f)"),
			ImpactForce, BreakableSettings->BreakForceThreshold);
		return false;
	}

	// 检查位置是否在可断裂区域内
	int32 MaterialID = INDEX_NONE;
	if (!IsLocationInBreakableRegion(ImpactLocation, MaterialID))
	{
		UE_LOG(LogTemp, Log, TEXT("Bullet impact location not in breakable region"));
		return false;
	}

	// 生成碎片
	GenerateFragmentsAtLocation(ImpactLocation, BreakRadius, ImpactForce);

	// 触发事件
	OnClothBreak.Broadcast(TargetSkeletalMesh, ImpactLocation, BreakRadius, ImpactForce, MaterialID);

	UE_LOG(LogTemp, Log, TEXT("Bullet hit processed: Location=%s, Radius=%f, Force=%f"),
		*ImpactLocation.ToString(), BreakRadius, ImpactForce);

	return true;
}

bool UClothBreakableComponent::SimulateBulletImpact(FVector ImpactLocation, float BulletSize, float ImpactForce)
{
	if (!bIsInitialized || !TargetSkeletalMesh || !BreakableSettings)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot simulate bullet impact: component not properly initialized"));
		return false;
	}

	// 计算断裂半径
	float BreakRadius = BulletSize * BreakableSettings->RadiusMultiplier;

	// 检查碰撞力是否超过阈值
	if (ImpactForce < BreakableSettings->BreakForceThreshold)
	{
		UE_LOG(LogTemp, Log, TEXT("Simulated impact force (%f) below threshold (%f)"),
			ImpactForce, BreakableSettings->BreakForceThreshold);
		return false;
	}

	// 检查位置是否在可断裂区域内
	int32 MaterialID = INDEX_NONE;
	if (!IsLocationInBreakableRegion(ImpactLocation, MaterialID))
	{
		UE_LOG(LogTemp, Log, TEXT("Simulated impact location not in breakable region"));
		return false;
	}

	// 生成碎片
	GenerateFragmentsAtLocation(ImpactLocation, BreakRadius, ImpactForce);

	// 触发事件
	OnClothBreak.Broadcast(TargetSkeletalMesh, ImpactLocation, BreakRadius, ImpactForce, MaterialID);

	UE_LOG(LogTemp, Log, TEXT("Simulated bullet impact: Location=%s, Radius=%f, Force=%f"),
		*ImpactLocation.ToString(), BreakRadius, ImpactForce);

	return true;
}

void UClothBreakableComponent::GenerateFragmentsAtLocation(const FVector& Location, float Radius, float ImpactForce)
{
	if (!FragmentGenerator || !BreakableSettings)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot generate fragments: fragment generator not initialized"));
		return;
	}

	// 确定碎片数量
	int32 FragmentCount = FMath::RandRange(BreakableSettings->MinFragmentCount, BreakableSettings->MaxFragmentCount);

	// 生成碎片
	bool bSuccess = FragmentGenerator->GenerateFragmentsFromCloth(TargetSkeletalMesh,
		Location, Radius, INDEX_NONE, FragmentCount,
		BreakableSettings->MinFragmentSize, BreakableSettings->MaxFragmentSize);

	if (bSuccess)
	{
		UE_LOG(LogTemp, Log, TEXT("Generated %d fragments at location: %s with radius: %f"),
			FragmentCount, *Location.ToString(), Radius);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to generate fragments"));
	}
}

bool UClothBreakableComponent::IsLocationInBreakableRegion(const FVector& Location, int32& OutMaterialID)
{
	if (!TargetSkeletalMesh || !BreakableSettings)
	{
		return false;
	}

	// 简化实现，实际上需要根据材质ID检查位置是否在可断裂区域内
	// 这里仅作为占位符

	// 在实际实现中，需要:
	// 1. 将世界空间位置转换为布料空间
	// 2. 找到最近的布料顶点
	// 3. 获取该顶点的材质ID
	// 4. 检查该材质ID是否在可断裂列表中

	// 临时返回第一个可断裂材质ID（如果有）
	if (BreakableSettings->BreakableMaterialIDs.Num() > 0)
	{
		OutMaterialID = BreakableSettings->BreakableMaterialIDs[0];
		return true;
	}

	// 如果没有指定可断裂材质ID，则所有区域都可断裂
	if (BreakableSettings->BreakableMaterialIDs.Num() == 0)
	{
		OutMaterialID = 0;
		return true;
	}

	return false;
}

void UClothBreakableComponent::ForceBreakClothAtLocation(FVector WorldLocation, float Radius)
{
	if (!bIsInitialized || !TargetSkeletalMesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot break cloth: component not initialized or target mesh invalid"));
		return;
	}

	int32 MaterialID = INDEX_NONE;
	if (IsLocationInBreakableRegion(WorldLocation, MaterialID))
	{
		// 使用默认力度
		float DefaultForce = BreakableSettings ? BreakableSettings->BreakForceThreshold * 1.5f : 1500.0f;

		// 生成碎片
		GenerateFragmentsAtLocation(WorldLocation, Radius, DefaultForce);

		// 触发事件
		OnClothBreak.Broadcast(TargetSkeletalMesh, WorldLocation, Radius, DefaultForce, MaterialID);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Location is not in a breakable region"));
	}
}

void UClothBreakableComponent::SetBreakableMaterialID(int32 MaterialID, bool bBreakable)
{
	if (!BreakableSettings)
	{
		return;
	}

	if (bBreakable)
	{
		// 添加到可断裂材质ID列表（如果不存在）
		if (!BreakableSettings->BreakableMaterialIDs.Contains(MaterialID))
		{
			BreakableSettings->BreakableMaterialIDs.Add(MaterialID);
			UE_LOG(LogTemp, Log, TEXT("Added material ID %d to breakable list"), MaterialID);
		}
	}
	else
	{
		// 从可断裂材质ID列表中移除
		BreakableSettings->BreakableMaterialIDs.Remove(MaterialID);
		UE_LOG(LogTemp, Log, TEXT("Removed material ID %d from breakable list"), MaterialID);
	}
}
