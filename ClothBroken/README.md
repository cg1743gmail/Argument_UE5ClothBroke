# ClothBroken 插件

## 概述

ClothBroken 是一个 UE5.5 插件，专为实现基于子弹碰撞的布料断裂功能而设计。该插件允许布料在被子弹击中时产生逼真的撕裂和断裂效果，并生成可配置数量和大小的碎片。

![ClothBroken 效果展示](Resources/Preview.png)

## 主要功能

- **子弹碰撞断裂**：布料在被子弹击中时自动断裂
- **可配置断裂参数**：可调节断裂力阈值、断裂半径倍率等
- **可控碎片生成**：可配置碎片数量、大小范围等
- **物理模拟**：碎片支持物理模拟，可自然下落或弹开
- **调试可视化**：提供断裂区域和碰撞力的可视化功能

## 兼容性

- **引擎版本**：UE 5.5+
- **平台**：Windows, Mac, Linux, iOS, Android
- **API**：Blueprint 和 C++

## 安装方法

1. 关闭 Unreal Editor
2. 将 `ClothBroken` 文件夹复制到您的项目的 `Plugins` 目录中
3. 重新启动 Unreal Editor
4. 在编辑器中启用插件：`编辑 > 插件 > 物理 > ClothBroken`

## 快速入门

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

### 2. 配置断裂参数

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

// 设置子弹断裂参数
UClothBreakableFunctionLibrary::SetBulletBreakParameters(
    SkeletalMeshComp,    // 目标骨骼网格体组件
    2.0f                 // 子弹大小到断裂半径的倍率
);
```

### 3. 处理子弹碰撞

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

## 高级功能

### 监听断裂事件

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

### 调试可视化

```
// 启用调试可视化
UClothBreakableFunctionLibrary::EnableDebugVisualization(
    SkeletalMeshComp,        // 目标骨骼网格体组件
    true,                    // 是否启用
    3.0f                     // 绘制持续时间（秒）
);
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

## 性能考虑

1. **碎片数量限制**：碎片数量直接影响性能，建议根据目标平台调整最大碎片数量
2. **碎片生命周期**：较短的生命周期可以减少场景中同时存在的碎片数量
3. **物理模拟**：如果性能是关键考虑因素，可以禁用碎片物理模拟
4. **调试可视化**：在发布版本中禁用调试可视化

## API 参考

### 主要类

- **UClothBreakableComponent**：管理布料断裂行为的组件
- **UClothBreakableSettings**：存储断裂参数的设置类
- **UClothFragmentGenerator**：生成布料碎片的工具类
- **UBulletImpactHandler**：处理子弹碰撞事件的工具类
- **UClothBreakableFunctionLibrary**：提供蓝图函数的工具类

### 主要函数

详细的API文档请参考 [API参考文档](Docs/API_Reference.md)。

## 示例项目

插件包含一个示例场景，展示了如何使用ClothBroken插件实现子弹击中布料的断裂效果。

1. 打开示例场景：`Content/ClothBroken/Examples/ClothBreakingExample`
2. 按下Play按钮
3. 使用WASD键移动，鼠标瞄准，左键射击布料目标

## 常见问题

### 布料不断裂

- 检查断裂力阈值是否设置过高
- 确认目标骨骼网格体有布料资产
- 验证材质ID是否正确设置为可断裂

### 碎片生成异常

- 检查碎片大小范围是否合理
- 确认断裂半径倍率设置适当
- 验证碎片数量限制是否过低

### 性能问题

- 减少最大碎片数量
- 缩短碎片生命周期
- 考虑禁用碎片物理模拟

## 技术支持

如有任何问题或建议，请联系：

- 邮箱：support@example.com
- 论坛：[UE论坛ClothBroken主题](https://forums.unrealengine.com)
- GitHub：[ClothBroken项目页面](https://github.com/example/ClothBroken)

## 许可证

本插件采用MIT许可证。详情请参阅 [LICENSE](LICENSE) 文件。

## 致谢

- 感谢Epic Games提供的Unreal Engine和Chaos物理系统
- 感谢所有测试和提供反馈的开发者
