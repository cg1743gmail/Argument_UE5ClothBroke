# ChaosClothBrokenEXT 完整使用手册

## 目录
1. [插件概述](#插件概述)
2. [安装与配置](#安装与配置)
3. [基础使用流程](#基础使用流程)
4. [高级功能](#高级功能)
5. [蓝图使用指南](#蓝图使用指南)
6. [C++使用指南](#c使用指南)
7. [性能优化](#性能优化)
8. [故障排除](#故障排除)

## 插件概述

ChaosClothBrokenEXT 是一个专为 UE5.5 设计的高级布料断裂插件，提供以下核心功能：

### 主要特性
- **实时布料断裂**：基于物理碰撞的动态布料撕裂
- **可配置碎片生成**：自定义碎片数量、大小和物理属性
- **多材质支持**：可单独控制不同材质的断裂行为
- **性能优化**：内置LOD和距离优化机制
- **调试工具**：完整的可视化调试系统

### 技术架构
- **ClothBreakableComponent**：核心断裂控制组件
- **ClothFragmentGenerator**：碎片生成系统
- **BulletImpactHandler**：碰撞处理系统
- **ClothBreakableFunctionLibrary**：蓝图函数库

## 安装与配置

### 1. 插件安装

#### 方法1：项目插件安装
1. 将 `ChaosClothBrokenEXT` 文件夹复制到项目的 `Plugins` 目录
2. 如果 `Plugins` 目录不存在，请创建它
3. 重启 Unreal Editor

#### 方法2：引擎插件安装
1. 将插件复制到 UE5 安装目录的 `Engine/Plugins` 下
2. 重启编辑器

### 2. 插件启用

1. 打开项目后，进入 `编辑 > 插件`
2. 在搜索框中输入 `ChaosClothBrokenEXT`
3. 勾选插件旁的复选框
4. 点击 `立即重启` 重启编辑器

### 3. 依赖插件检查

确保以下插件已启用：
- ✅ **ChaosCloth** - 布料物理系统
- ✅ **GeometryScripting** - 几何脚本支持  
- ✅ **GeometryProcessing** - 几何处理功能

## 基础使用流程

### 第一步：准备布料Actor

#### 1.1 创建布料Actor蓝图
```
1. 内容浏览器 > 右键 > 蓝图类 > Actor
2. 命名为 "BP_ClothActor"
3. 双击打开蓝图编辑器
```

#### 1.2 添加骨骼网格体组件
```
组件面板 > 添加组件 > Skeletal Mesh Component
设置属性：
- Skeletal Mesh: 选择包含布料的骨骼网格体
- Collision Enabled: Query and Physics
- Collision Object Type: WorldDynamic
```

#### 1.3 添加布料断裂组件
```
组件面板 > 添加组件 > ClothBreakableComponent
设置属性：
- Target Skeletal Mesh: 选择上面的骨骼网格体组件
```

### 第二步：配置断裂参数

在 ClothBreakableComponent 的详情面板中设置：

#### 2.1 基础断裂设置
```
Break Force Threshold: 2000.0     // 需要多大力才能撕裂布料
Radius Multiplier: 2.0            // 撕裂区域大小倍率
Enable Breaking: ✓                // 启用断裂功能
```

#### 2.2 碎片生成设置
```
Min Fragment Count: 3             // 最少生成几个碎片
Max Fragment Count: 8             // 最多生成几个碎片
Min Fragment Size: 3.0            // 碎片最小尺寸
Max Fragment Size: 12.0           // 碎片最大尺寸
Fragment Lifetime: 5.0            // 碎片存在时间（秒）
```

### 第三步：创建子弹Actor

#### 3.1 创建子弹蓝图
```
内容浏览器 > 右键 > 蓝图类 > Actor
命名为 "BP_Bullet"
```

#### 3.2 添加必要组件
```
1. Scene Component (根组件)
2. Static Mesh Component (子弹外观)
   - Static Mesh: 选择子弹网格
   - Collision Enabled: No Collision
   
3. Sphere Component (碰撞检测)
   - Sphere Radius: 1.0
   - Collision Enabled: Query Only
   - Collision Object Type: WorldDynamic
   - Generate Hit Events: ✓
   
4. Projectile Movement Component (运动控制)
   - Initial Speed: 2000.0
   - Max Speed: 3000.0
   - Gravity Scale: 0.5
```

### 第四步：实现碰撞处理

#### 4.1 添加碰撞事件
在子弹蓝图的事件图表中：

```
1. 选择 Sphere Component
2. 在详情面板的事件部分，点击 "On Component Hit" 旁的 "+"
3. 这会在事件图表中创建 "Event On Component Hit" 节点
```

#### 4.2 连接处理逻辑
```
Event On Component Hit
├── Hit Result (输出)
├── Other Actor (输出)  
├── Other Component (输出)
└── 连接到以下节点序列：

Other Component → Cast to SkeletalMeshComponent
                ↓ (成功执行引脚)
                Handle Bullet Hit (函数库调用)
                ├── Target: Cast 结果
                ├── Hit Result: Event 的 Hit Result
                └── Radius Multiplier: 2.0

最后连接 → Destroy Actor (销毁子弹)
```

### 第五步：测试设置

#### 5.1 创建测试场景
```
1. 将布料Actor拖入场景
2. 调整位置和大小
3. 确保布料可见且有足够空间
```

#### 5.2 创建简单发射器
```
在关卡蓝图中：
Event Begin Play → Input Action (鼠标左键)
                 ↓
                 Spawn Actor from Class
                 ├── Class: BP_Bullet
                 ├── Spawn Transform: 摄像机位置
                 └── Spawn Collision Handling: Always Spawn
```

## 高级功能

### 1. 动态参数调整

#### 运行时修改断裂参数
```cpp
// 蓝图节点：Set Cloth Break Parameters
Target: ClothBreakableComponent
Break Force Threshold: 1500.0
Min Fragment Count: 5
Max Fragment Count: 12
Min Fragment Size: 2.0
Max Fragment Size: 15.0
```

#### 材质特定控制
```cpp
// 设置特定材质可断裂
Set Material Breakable
├── Target: SkeletalMeshComponent
├── Material Index: 0
└── Breakable: true

// 设置所有材质断裂状态
Set All Materials Breakable
├── Target: SkeletalMeshComponent  
└── Breakable: true
```

### 2. 事件监听系统

#### 绑定断裂事件
```cpp
// 在布料Actor的蓝图中
Event Begin Play → Bind Event to On Cloth Break
                 ├── Event: On Cloth Break (自定义事件)
                 └── Object: ClothBreakableComponent

// 创建自定义事件：On Cloth Break
输入参数：
- Skeletal Mesh Component (object reference)
- Break Location (vector)
- Break Radius (float)
- Impact Force (float)  
- Material ID (integer)

事件处理：
On Cloth Break → Play Sound at Location
               ├── Sound: 断裂音效
               └── Location: Break Location
               ↓
               Spawn Emitter at Location
               ├── Emitter Template: 粒子效果
               └── Location: Break Location
```

### 3. 调试和可视化

#### 启用调试显示
```cpp
// 蓝图节点：Enable Debug Visualization
Target: ClothBreakableComponent
Enable: true
Draw Duration: 5.0
```

#### 手动触发断裂（测试）
```cpp
// 蓝图节点：Force Break Cloth At Location
Target: SkeletalMeshComponent
Location: 指定位置
Radius: 50.0
Force: 2000.0
Material ID: 0
```

## 蓝图使用指南

### 常用蓝图节点

#### 1. 组件管理节点
```
- Add Cloth Breakable To Skeletal Mesh
- Get Cloth Breakable Component
- Is Cloth Breakable Component Valid
```

#### 2. 参数设置节点
```
- Set Cloth Break Parameters
- Set Bullet Break Parameters  
- Set Material Breakable
- Set All Materials Breakable
```

#### 3. 断裂控制节点
```
- Handle Bullet Hit
- Force Break Cloth At Location
- Can Break At Location
```

#### 4. 调试节点
```
- Enable Debug Visualization
- Disable Debug Visualization
- Draw Debug Break Info
```

### 蓝图最佳实践

#### 1. 组件初始化模板
```
Event Begin Play
├── Get Component by Class (ClothBreakableComponent)
├── Is Valid (检查组件存在)
└── [True] → Set Cloth Break Parameters
            ├── Break Force Threshold: 2000.0
            ├── Min Fragment Count: 3
            ├── Max Fragment Count: 8
            ├── Min Fragment Size: 3.0
            └── Max Fragment Size: 12.0
```

#### 2. 碰撞处理模板
```
Event On Component Hit
├── Cast to SkeletalMeshComponent (Other Component)
└── [Success] → Handle Bullet Hit
              ├── Target: Cast Result
              ├── Hit Result: Hit Result
              └── Radius Multiplier: 2.0
              ↓
              Destroy Actor (Self)
```

## C++使用指南

### 1. 头文件包含
```cpp
#include "ClothBreakableComponent.h"
#include "ClothBreakableFunctionLibrary.h"
#include "BulletImpactHandler.h"
```

### 2. 组件添加和配置
```cpp
// 在Actor构造函数中
UCLASS()
class YOURPROJECT_API AClothActor : public AActor
{
    GENERATED_BODY()

public:
    AClothActor();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USkeletalMeshComponent* SkeletalMeshComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")  
    UClothBreakableComponent* ClothBreakableComponent;
};

// 构造函数实现
AClothActor::AClothActor()
{
    PrimaryActorTick.bCanEverTick = false;

    // 创建骨骼网格体组件
    SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
    RootComponent = SkeletalMeshComponent;

    // 创建布料断裂组件
    ClothBreakableComponent = CreateDefaultSubobject<UClothBreakableComponent>(TEXT("ClothBreakable"));
    
    // 配置组件
    ClothBreakableComponent->SetTargetSkeletalMesh(SkeletalMeshComponent);
}
```

### 3. 运行时参数设置
```cpp
void AClothActor::BeginPlay()
{
    Super::BeginPlay();
    
    // 设置断裂参数
    UClothBreakableFunctionLibrary::SetClothBreakParameters(
        SkeletalMeshComponent,
        2000.0f,    // Break Force Threshold
        3,          // Min Fragment Count
        8,          // Max Fragment Count  
        3.0f,       // Min Fragment Size
        12.0f       // Max Fragment Size
    );
    
    // 设置材质断裂状态
    UClothBreakableFunctionLibrary::SetAllMaterialsBreakable(
        SkeletalMeshComponent, true);
        
    // 绑定断裂事件
    if (ClothBreakableComponent)
    {
        ClothBreakableComponent->OnClothBreak.AddDynamic(
            this, &AClothActor::OnClothBreakEvent);
    }
}
```

### 4. 事件处理
```cpp
// 头文件中声明
UFUNCTION()
void OnClothBreakEvent(USkeletalMeshComponent* SkeletalMeshComponent,
    FVector BreakLocation, float BreakRadius, float ImpactForce, int32 MaterialID);

// 实现
void AClothActor::OnClothBreakEvent(USkeletalMeshComponent* SkeletalMeshComponent,
    FVector BreakLocation, float BreakRadius, float ImpactForce, int32 MaterialID)
{
    // 播放音效
    if (BreakSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, BreakSound, BreakLocation);
    }
    
    // 生成粒子效果
    if (BreakParticleSystem)
    {
        UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), 
            BreakParticleSystem, BreakLocation);
    }
    
    // 记录日志
    UE_LOG(LogTemp, Log, TEXT("Cloth broken at %s with force %f on material %d"),
        *BreakLocation.ToString(), ImpactForce, MaterialID);
}
```

## 性能优化

### 1. 碎片数量控制
```cpp
// 根据平台调整碎片数量
int32 MaxFragments = 8;  // 默认值

#if PLATFORM_MOBILE
    MaxFragments = 4;     // 移动平台减少碎片
#elif PLATFORM_CONSOLE  
    MaxFragments = 6;     // 主机平台中等数量
#else
    MaxFragments = 10;    // PC平台更多碎片
#endif

UClothBreakableFunctionLibrary::SetClothBreakParameters(
    SkeletalMeshComponent, 2000.0f, 2, MaxFragments, 3.0f, 12.0f);
```

### 2. 距离LOD优化
```cpp
void AClothActor::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // 获取玩家距离
    APawn* Player = GetWorld()->GetFirstPlayerController()->GetPawn();
    if (Player)
    {
        float Distance = FVector::Dist(GetActorLocation(), Player->GetActorLocation());
        
        // 根据距离调整断裂参数
        if (Distance > 1000.0f)
        {
            // 远距离：禁用断裂
            ClothBreakableComponent->SetEnableBreaking(false);
        }
        else if (Distance > 500.0f)
        {
            // 中距离：减少碎片数量
            ClothBreakableComponent->SetEnableBreaking(true);
            UClothBreakableFunctionLibrary::SetClothBreakParameters(
                SkeletalMeshComponent, 2000.0f, 1, 3, 5.0f, 10.0f);
        }
        else
        {
            // 近距离：完整效果
            ClothBreakableComponent->SetEnableBreaking(true);
            UClothBreakableFunctionLibrary::SetClothBreakParameters(
                SkeletalMeshComponent, 2000.0f, 3, 8, 3.0f, 12.0f);
        }
    }
}
```

### 3. 内存管理
```cpp
// 定期清理无效碎片
void AClothActor::CleanupFragments()
{
    if (ClothBreakableComponent)
    {
        // 插件内部会自动清理，但可以手动触发
        ClothBreakableComponent->CleanupOldFragments();
    }
}
```

## 故障排除

### 常见问题及解决方案

#### 1. 布料不断裂
**症状**：子弹击中布料但没有断裂效果

**检查清单**：
- [ ] ClothBreakableComponent 已添加并正确配置
- [ ] Target Skeletal Mesh 已设置
- [ ] Enable Breaking 已勾选
- [ ] Break Force Threshold 不要设置过高（建议 < 3000）
- [ ] 子弹碰撞事件正确调用 HandleBulletHit
- [ ] 材质已设置为可断裂

**解决步骤**：
```cpp
// 1. 启用调试可视化
UClothBreakableFunctionLibrary::EnableDebugVisualization(
    SkeletalMeshComponent, true, 5.0f);

// 2. 手动测试断裂
UClothBreakableFunctionLibrary::ForceBreakClothAtLocation(
    SkeletalMeshComponent, FVector(0,0,100), 50.0f, 2000.0f, 0);

// 3. 检查组件有效性
if (!ClothBreakableComponent || !ClothBreakableComponent->IsValidLowLevel())
{
    UE_LOG(LogTemp, Error, TEXT("ClothBreakableComponent is invalid!"));
}
```

#### 2. 碎片生成异常
**症状**：断裂发生但碎片显示不正常

**可能原因**：
- 碎片大小设置不合理
- 材质丢失
- 物理设置错误

**解决方案**：
```cpp
// 检查碎片参数
float MinSize = 1.0f, MaxSize = 20.0f;  // 确保合理范围
int32 MinCount = 1, MaxCount = 15;      // 确保合理数量

UClothBreakableFunctionLibrary::SetClothBreakParameters(
    SkeletalMeshComponent, 2000.0f, MinCount, MaxCount, MinSize, MaxSize);
```

#### 3. 性能问题
**症状**：断裂时帧率下降明显

**优化措施**：
```cpp
// 1. 减少碎片数量
MaxFragmentCount = 5;  // 从8减少到5

// 2. 缩短碎片生命周期  
FragmentLifetime = 3.0f;  // 从5秒减少到3秒

// 3. 启用距离优化
if (DistanceToPlayer > 500.0f)
{
    ClothBreakableComponent->SetEnableBreaking(false);
}
```

### 调试工具使用

#### 1. 控制台命令
```
// 在编辑器控制台中输入：
ClothBreak.EnableDebug 1          // 启用调试显示
ClothBreak.ShowFragmentCount 1    // 显示碎片计数
ClothBreak.MaxFragments 10        // 设置最大碎片数
```

#### 2. 日志输出
```cpp
// 在项目设置中启用日志类别
LogTemp, Log, All

// 查看断裂相关日志
UE_LOG(LogTemp, Log, TEXT("Cloth break triggered at %s"), 
    *BreakLocation.ToString());
```

通过本手册，您应该能够完全掌握 ChaosClothBrokenEXT 插件的使用方法。如有其他问题，请参考插件源代码或联系技术支持。
