// Copyright Epic Games, Inc. All Rights Reserved.

#include "BulletImpactHandler.h"
#include "Components/PrimitiveComponent.h"
#include "PhysicsEngine/BodyInstance.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

UBulletImpactHandler::UBulletImpactHandler()
{
    // 初始化默认值
    bEnableDebugVisualization = false;
    DebugDrawDuration = 3.0f;
}

bool UBulletImpactHandler::ProcessBulletImpact(const FHitResult& HitResult, float RadiusMultiplier,
    FVector& OutImpactLocation, float& OutBreakRadius, float& OutImpactForce)
{
    // 检查碰撞是否有效
    if (!HitResult.Component.IsValid() || !HitResult.GetComponent())
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid hit result"));
        return false;
    }

    // 获取被击中的组件和子弹组件
    UPrimitiveComponent* HitComponent = HitResult.Component.Get();
    AActor* BulletActor = HitResult.GetActor();

    if (!BulletActor)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid bullet actor"));
        return false;
    }

    UPrimitiveComponent* BulletComponent = Cast<UPrimitiveComponent>(BulletActor->GetRootComponent());
    if (!BulletComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid bullet component"));
        return false;
    }

    // 计算碰撞位置
    OutImpactLocation = HitResult.ImpactPoint;

    // 计算断裂半径
    OutBreakRadius = CalculateBreakRadius(HitComponent, BulletComponent, RadiusMultiplier);

    // 计算碰撞力
    OutImpactForce = CalculateImpactForce(HitResult);

    // 调试可视化
    if (bEnableDebugVisualization && HitComponent->GetWorld())
    {
        DrawDebugSphere(
            HitComponent->GetWorld(),
            OutImpactLocation,
            OutBreakRadius,
            16,
            FColor::Red,
            false,
            DebugDrawDuration,
            0,
            1.0f
        );

        DrawDebugString(
            HitComponent->GetWorld(),
            OutImpactLocation + FVector(0, 0, 10.0f),
            FString::Printf(TEXT("Force: %.1f"), OutImpactForce),
            nullptr,
            FColor::White,
            DebugDrawDuration
        );
    }

    UE_LOG(LogTemp, Log, TEXT("Bullet impact processed: Location=%s, Radius=%f, Force=%f"),
        *OutImpactLocation.ToString(), OutBreakRadius, OutImpactForce);

    return true;
}

float UBulletImpactHandler::CalculateBreakRadius(UPrimitiveComponent* HitComponent,
    UPrimitiveComponent* BulletComponent, float RadiusMultiplier)
{
    if (!BulletComponent)
    {
        return DefaultRadius;
    }

    // 获取子弹大小
    float BulletSize = 0.0f;

    // 方法1: 使用包围盒
    FVector BulletExtent = BulletComponent->Bounds.BoxExtent;
    BulletSize = BulletExtent.GetMax();

    // 方法2: 如果是球体碰撞，尝试获取球体半径
    USphereComponent* SphereComp = Cast<USphereComponent>(BulletComponent);
    if (SphereComp)
    {
        BulletSize = SphereComp->GetScaledSphereRadius();
    }

    // 方法3: 如果是胶囊体碰撞，尝试获取胶囊体半径
    UCapsuleComponent* CapsuleComp = Cast<UCapsuleComponent>(BulletComponent);
    if (CapsuleComp)
    {
        BulletSize = CapsuleComp->GetScaledCapsuleRadius();
    }

    // 如果无法获取大小，使用默认值
    if (BulletSize <= 0.0f)
    {
        BulletSize = 5.0f;
    }

    // 应用半径倍率
    float BreakRadius = BulletSize * RadiusMultiplier;

    // 确保半径在合理范围内
    BreakRadius = FMath::Clamp(BreakRadius, 1.0f, 100.0f);

    return BreakRadius;
}

float UBulletImpactHandler::CalculateImpactForce(const FHitResult& HitResult)
{
    // 获取子弹Actor
    AActor* BulletActor = HitResult.GetActor();
    if (!BulletActor)
    {
        return 1000.0f; // 默认碰撞力
    }

    // 获取子弹根组件
    UPrimitiveComponent* BulletComponent = Cast<UPrimitiveComponent>(BulletActor->GetRootComponent());
    if (!BulletComponent)
    {
        return 1000.0f;
    }

    // 方法1: 使用物理速度
    FVector BulletVelocity = BulletComponent->GetPhysicsLinearVelocity();
    float BulletSpeed = BulletVelocity.Size();

    // 方法2: 尝试从ProjectileMovementComponent获取速度
    UProjectileMovementComponent* ProjectileMovement = BulletActor->FindComponentByClass<UProjectileMovementComponent>();
    if (ProjectileMovement)
    {
        BulletSpeed = ProjectileMovement->Velocity.Size();
    }

    // 如果速度太小，使用默认值
    if (BulletSpeed < 100.0f)
    {
        BulletSpeed = 1000.0f;
    }

    // 获取子弹质量
    float BulletMass = 1.0f;
    if (BulletComponent->IsSimulatingPhysics())
    {
        BulletMass = BulletComponent->GetMass();
    }

    // 如果质量太小，使用默认值
    if (BulletMass < 0.1f)
    {
        BulletMass = 1.0f;
    }

    // 计算碰撞力 (简化的动量公式)
    float ImpactForce = BulletMass * BulletSpeed * 0.1f;

    // 确保力在合理范围内
    ImpactForce = FMath::Clamp(ImpactForce, 100.0f, 10000.0f);

    return ImpactForce;
}

bool UBulletImpactHandler::SimulateBulletImpact(UPrimitiveComponent* TargetComponent,
    const FVector& ImpactLocation, float BulletSize, float ImpactForce,
    float RadiusMultiplier, float& OutBreakRadius)
{
    if (!TargetComponent)
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid target component"));
        return false;
    }

    // 计算断裂半径
    OutBreakRadius = BulletSize * RadiusMultiplier;

    // 确保半径在合理范围内
    OutBreakRadius = FMath::Clamp(OutBreakRadius, 1.0f, 100.0f);

    // 调试可视化
    if (bEnableDebugVisualization && TargetComponent->GetWorld())
    {
        DrawDebugSphere(
            TargetComponent->GetWorld(),
            ImpactLocation,
            OutBreakRadius,
            16,
            FColor::Green,
            false,
            DebugDrawDuration,
            0,
            1.0f
        );

        DrawDebugString(
            TargetComponent->GetWorld(),
            ImpactLocation + FVector(0, 0, 10.0f),
            FString::Printf(TEXT("Simulated Force: %.1f"), ImpactForce),
            nullptr,
            FColor::White,
            DebugDrawDuration
        );
    }

    UE_LOG(LogTemp, Log, TEXT("Simulated bullet impact: Location=%s, Radius=%f, Force=%f"),
        *ImpactLocation.ToString(), OutBreakRadius, ImpactForce);

    return true;
}

void UBulletImpactHandler::SetDebugVisualization(bool bEnable, float DrawDuration)
{
    bEnableDebugVisualization = bEnable;
    DebugDrawDuration = DrawDuration;
}
