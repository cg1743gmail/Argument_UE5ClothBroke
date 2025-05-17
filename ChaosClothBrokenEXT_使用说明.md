# ChaosClothBrokenEXT 插件使用说明

## 概述

ChaosClothBrokenEXT 是一个 UE5.5 插件，用于实现基于子弹碰撞的布料断裂功能。该插件允许布料在被子弹击中时产生逼真的撕裂和断裂效果，并生成可配置数量和大小的碎片。

## 主要功能

- **子弹碰撞断裂**：布料在被子弹击中时自动断裂
- **可配置断裂参数**：可调节断裂力阈值、断裂半径倍率等
- **可控碎片生成**：可配置碎片数量、大小范围等
- **物理模拟**：碎片支持物理模拟，可自然下落或弹开
- **调试可视化**：提供断裂区域和碰撞力的可视化功能

## 安装方法

1. 将 `ChaosClothBrokenEXT` 文件夹复制到您的项目的 `Plugins` 目录中
2. 重新启动 Unreal Editor
3. 在编辑器中启用插件：`编辑 > 插件 > 物理 > ChaosClothBrokenEXT`

## 基本使用流程

### 1. 添加布料断裂组件

有两种方式可以添加布料断裂组件：

#### 方式一：通过蓝图编辑器

1. 选择包含布料的骨骼网格体 Actor
2. 点击 `添加组件` 按钮
3. 搜索并选择 `ClothBreakableComponent`
4. 在组件详情面板中，将 `Target Skeletal Mesh` 设置为目标骨骼网格体组件

#### 方式二：通过蓝图函数

```
// 获取骨骼网格体组件
USkeletalMeshComponent* SkeletalMeshComp = GetComponentByClass<USkeletalMeshComponent>();

// 添加布料断裂组件
UClothBreakableComponent* BreakableComp = UClothBreakableFunctionLibrary::AddClothBreakableToSkeletalMesh(SkeletalMeshComp);
```

### 2. 配置断裂区域

默认情况下，布料的所有部分都可以断裂。如果需要指定特定区域可断裂，可以通过以下方式配置：

#### 设置特定材质 ID 可断裂

```
// 设置材质 ID 2 可断裂
UClothBreakableFunctionLibrary::SetBreakableMaterialID(SkeletalMeshComp, 2, true);

// 设置材质 ID 3 不可断裂
UClothBreakableFunctionLibrary::SetBreakableMaterialID(SkeletalMeshComp, 3, false);
```

#### 设置所有材质可断裂

```
// 将所有材质设置为可断裂
UClothBreakableFunctionLibrary::SetAllMaterialsBreakable(SkeletalMeshComp);
```

### 3. 配置断裂参数

#### 基本断裂参数

```
// 设置布料断裂基本参数
UClothBreakableFunctionLibrary::SetClothBreakParameters(
    SkeletalMeshComp,    // 目标骨骼网格体组件
    1000.0f,             // 断裂力阈值
    3,                   // 最小碎片数量
    7,                   // 最大碎片数量
    5.0f,                // 最小碎片尺寸
    20.0f                // 最大碎片尺寸
);
```

#### 子弹断裂参数

```
// 设置子弹断裂参数
UClothBreakableFunctionLibrary::SetBulletBreakParameters(
    SkeletalMeshComp,    // 目标骨骼网格体组件
    2.0f                 // 子弹大小到断裂半径的倍率
);
```

#### 碎片物理参数

```
// 设置碎片物理参数
UClothBreakableFunctionLibrary::SetFragmentPhysicsParameters(
    SkeletalMeshComp,    // 目标骨骼网格体组件
    true,                // 是否启用物理
    10.0f,               // 碎片质量
    5.0f                 // 碎片生命周期（秒）
);
```

### 4. 处理子弹碰撞

要使布料对子弹碰撞做出反应，需要在子弹碰撞事件中调用 `HandleBulletHit` 函数：

```
// 在子弹碰撞事件中
void AMyActor::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, 
    UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    // 检查被击中的是否是布料
    USkeletalMeshComponent* SkeletalMeshComp = Cast<USkeletalMeshComponent>(OtherComp);
    if (SkeletalMeshComp)
    {
        // 处理子弹碰撞
        UClothBreakableFunctionLibrary::HandleBulletHit(SkeletalMeshComp, Hit);
    }
}
```

### 5. 手动触发断裂（可选）

如果需要在没有子弹碰撞的情况下触发布料断裂，可以使用以下函数：

```
// 在指定位置手动触发断裂
UClothBreakableFunctionLibrary::ForceBreakClothAtLocation(
    SkeletalMeshComp,        // 目标骨骼网格体组件
    FVector(0, 0, 100),      // 世界空间中的位置
    15.0f                    // 影响半径
);
```

### 6. 模拟子弹碰撞（可选）

如果需要模拟子弹碰撞而不是使用实际的物理碰撞，可以使用以下函数：

```
// 模拟子弹碰撞
UClothBreakableFunctionLibrary::SimulateBulletImpact(
    SkeletalMeshComp,        // 目标骨骼网格体组件
    FVector(0, 0, 100),      // 碰撞位置
    5.0f,                    // 子弹大小
    2000.0f                  // 碰撞力
);
```

## 调试功能

### 启用调试可视化

```
// 启用调试可视化
UClothBreakableFunctionLibrary::EnableDebugVisualization(
    SkeletalMeshComp,        // 目标骨骼网格体组件
    true,                    // 是否启用
    3.0f                     // 绘制持续时间（秒）
);
```

启用调试可视化后，可以看到：
- 断裂区域的球形范围（红色表示实际碰撞，绿色表示模拟碰撞）
- 碰撞力大小的文本显示
- 碎片生成位置的标记

### 检查骨骼网格体是否有布料

```
// 检查骨骼网格体是否有布料
bool bHasCloth = UClothBreakableFunctionLibrary::HasCloth(SkeletalMeshComp);
if (!bHasCloth)
{
    UE_LOG(LogTemp, Warning, TEXT("Skeletal mesh does not have cloth!"));
}
```

### 获取材质 ID 列表

```
// 获取骨骼网格体的材质 ID 列表
TArray<int32> MaterialIDs;
UClothBreakableFunctionLibrary::GetSkeletalMeshMaterialIDs(SkeletalMeshComp, MaterialIDs);

// 输出材质 ID
for (int32 ID : MaterialIDs)
{
    UE_LOG(LogTemp, Log, TEXT("Material ID: %d"), ID);
}
```

## 参数调优建议

### 断裂力阈值

- **较低值**（500-1000）：轻微碰撞也会导致断裂，适合脆弱布料
- **中等值**（1000-3000）：需要一定力度的碰撞才会断裂，适合一般布料
- **较高值**（3000+）：需要强力碰撞才会断裂，适合坚韧布料

### 断裂半径倍率

- **较低值**（0.5-1.0）：断裂区域小，精确断裂
- **中等值**（1.0-3.0）：断裂区域适中，平衡效果
- **较高值**（3.0+）：断裂区域大，夸张效果

### 碎片数量

- **较少**（1-3）：性能好，但视觉效果简单
- **适中**（4-7）：平衡性能和视觉效果
- **较多**（8+）：视觉效果好，但可能影响性能

### 碎片大小

- **较小**（1.0-5.0）：细碎效果，适合轻薄布料
- **适中**（5.0-15.0）：平衡效果，适合一般布料
- **较大**（15.0+）：大块断裂，适合厚重布料

## 性能考虑

1. **碎片数量限制**：碎片数量直接影响性能，建议根据目标平台调整最大碎片数量
2. **碎片生命周期**：较短的生命周期可以减少场景中同时存在的碎片数量
3. **物理模拟**：如果性能是关键考虑因素，可以禁用碎片物理模拟
4. **调试可视化**：在发布版本中禁用调试可视化

## 常见问题

### 布料不断裂

- 检查断裂力阈值是否设置过高
- 确认目标骨骼网格体有布料资产
- 验证材质 ID 是否正确设置为可断裂

### 碎片生成异常

- 检查碎片大小范围是否合理
- 确认断裂半径倍率设置适当
- 验证碎片数量限制是否过低

### 性能问题

- 减少最大碎片数量
- 缩短碎片生命周期
- 考虑禁用碎片物理模拟

## 高级用法

### 监听断裂事件

可以在蓝图中绑定到 `ClothBreakableComponent` 的 `OnClothBreak` 事件，以在布料断裂时执行自定义逻辑：

```
// 获取布料断裂组件
UClothBreakableComponent* BreakableComp = GetComponentByClass<UClothBreakableComponent>();

// 绑定到断裂事件
if (BreakableComp)
{
    BreakableComp->OnClothBreak.AddDynamic(this, &AMyActor::OnClothBreakEvent);
}

// 断裂事件回调
void AMyActor::OnClothBreakEvent(USkeletalMeshComponent* SkeletalMeshComponent, 
    FVector BreakLocation, float BreakRadius, float ImpactForce, int32 MaterialID)
{
    // 在断裂位置播放粒子效果
    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BreakParticleSystem, BreakLocation);
    
    // 播放断裂音效
    UGameplayStatics::PlaySoundAtLocation(this, BreakSound, BreakLocation);
}
```

### 自定义碎片行为

如果需要更高级的碎片控制，可以修改 `ClothFragmentGenerator` 类的实现，或者在断裂事件中创建自定义碎片。
