# ChaosClothBrokenEXT 详细配置指南

本文档详细说明了如何配置 ChaosClothBrokenEXT 插件的各个组件，包括每个参数的具体设置方法和推荐值。

## 目录

1. [前置准备](#前置准备)
2. [布料Actor配置](#布料actor配置)
3. [子弹Actor配置](#子弹actor配置)
4. [ClothBreakableComponent配置](#clothbreakablecomponent配置)
5. [材质和视觉效果配置](#材质和视觉效果配置)
6. [调试和测试配置](#调试和测试配置)
7. [性能优化配置](#性能优化配置)

## 前置准备

### 1. 启用插件
1. 打开项目设置：`编辑 > 插件`
2. 搜索 `ChaosClothBrokenEXT`
3. 勾选启用插件
4. 重启编辑器

### 2. 确保依赖插件已启用
确保以下插件已启用：
- **ChaosCloth** - 布料物理系统
- **GeometryScripting** - 几何脚本支持
- **GeometryProcessing** - 几何处理功能

## 布料Actor配置

### 1. 创建布料Actor

#### 步骤1：添加骨骼网格体组件
1. 创建新的Actor蓝图
2. 添加 `Skeletal Mesh Component`
3. 设置骨骼网格体资产（必须包含布料资产）

#### 步骤2：配置骨骼网格体组件
**组件名称**: `SkeletalMeshComponent`

| 参数 | 设置值 | 说明 |
|------|--------|------|
| **Skeletal Mesh** | 选择包含布料的骨骼网格 | 必须有布料资产 |
| **Animation Mode** | `Use Animation Blueprint` 或 `Use Animation Asset` | 根据需要选择 |
| **Collision Enabled** | `Query and Physics` | 启用碰撞检测 |
| **Collision Object Type** | `WorldDynamic` | 设置碰撞类型 |
| **Collision Responses** | 根据需要设置 | 确保能与子弹碰撞 |

#### 步骤3：配置布料物理
在骨骼网格体的布料资产中：

| 参数 | 推荐值 | 说明 |
|------|--------|------|
| **Mass Scale** | 1.0 | 布料质量倍率 |
| **Damping** | 0.1-0.3 | 阻尼系数 |
| **Friction** | 0.2-0.5 | 摩擦系数 |
| **Collision Thickness** | 1.0-5.0 | 碰撞厚度 |

### 2. 添加ClothBreakableComponent

#### 步骤1：添加组件
1. 在布料Actor蓝图中，点击 `添加组件`
2. 搜索 `ClothBreakableComponent`
3. 添加到Actor

#### 步骤2：配置目标网格体
**组件名称**: `ClothBreakableComponent`

| 参数 | 设置值 | 说明 |
|------|--------|------|
| **Target Skeletal Mesh** | 选择骨骼网格体组件 | 指向要断裂的骨骼网格体 |

## 子弹Actor配置

### 1. 创建子弹Actor

#### 步骤1：基础组件设置
1. 创建新的Actor蓝图
2. 添加以下组件：
   - `Scene Component` (根组件)
   - `Static Mesh Component` (子弹外观)
   - `Sphere Component` (碰撞检测)
   - `Projectile Movement Component` (运动控制)

#### 步骤2：配置静态网格体组件
**组件名称**: `StaticMeshComponent`

| 参数 | 设置值 | 说明 |
|------|--------|------|
| **Static Mesh** | 子弹网格资产 | 子弹的视觉外观 |
| **Collision Enabled** | `No Collision` | 不参与碰撞 |
| **Cast Shadow** | `false` | 性能优化 |

#### 步骤3：配置球体碰撞组件
**组件名称**: `SphereComponent`

| 参数 | 设置值 | 说明 |
|------|--------|------|
| **Sphere Radius** | 0.5-2.0 | 子弹碰撞半径 |
| **Collision Enabled** | `Query Only` | 仅查询碰撞 |
| **Collision Object Type** | `WorldDynamic` | 碰撞对象类型 |
| **Collision Response** | 设置对布料的响应为 `Block` | 确保能碰撞布料 |
| **Generate Hit Events** | `true` | 生成碰撞事件 |

#### 步骤4：配置抛射物运动组件
**组件名称**: `ProjectileMovementComponent`

| 参数 | 推荐值 | 说明 |
|------|--------|------|
| **Initial Speed** | 1000-3000 | 初始速度 |
| **Max Speed** | 3000-5000 | 最大速度 |
| **Gravity Scale** | 0.0-1.0 | 重力影响 |
| **Bounce** | 0.0 | 弹跳系数 |
| **Should Bounce** | `false` | 是否弹跳 |

### 2. 配置碰撞事件处理

#### 步骤1：绑定碰撞事件
在子弹Actor的蓝图中：

1. 选择 `SphereComponent`
2. 在事件图表中添加 `On Component Hit` 事件
3. 连接到碰撞处理逻辑

#### 步骤2：实现碰撞处理逻辑
在 `On Component Hit` 事件中添加以下节点：

```
Event On Component Hit
├── Hit Result (输出)
├── Other Actor (输出)
└── Other Component (输出)
    └── Cast to SkeletalMeshComponent
        └── [成功] Handle Bullet Hit (函数库调用)
            ├── Skeletal Mesh Component: Other Component
            ├── Hit Result: Hit Result
            └── Radius Multiplier: 2.0 (可调节)
```

## ClothBreakableComponent配置

### 1. 基本断裂参数

在ClothBreakableComponent的详情面板中配置：

| 参数分类 | 参数名称 | 推荐值 | 说明 |
|----------|----------|--------|------|
| **断裂设置** | **Break Force Threshold** | 1000.0-3000.0 | 断裂力阈值 |
| | **Radius Multiplier** | 1.5-3.0 | 断裂半径倍率 |
| | **Enable Breaking** | `true` | 启用断裂功能 |
| **碎片设置** | **Min Fragment Count** | 3-5 | 最小碎片数量 |
| | **Max Fragment Count** | 7-12 | 最大碎片数量 |
| | **Min Fragment Size** | 2.0-5.0 | 最小碎片尺寸 |
| | **Max Fragment Size** | 8.0-15.0 | 最大碎片尺寸 |
| | **Fragment Lifetime** | 3.0-8.0 | 碎片生命周期(秒) |

### 2. 材质断裂配置

#### 步骤1：设置可断裂材质
1. 在ClothBreakableComponent中找到 `Material Breaking Settings`
2. 添加材质ID到可断裂列表

| 参数 | 设置方法 | 说明 |
|------|----------|------|
| **Breakable Material IDs** | 添加材质索引 | 指定哪些材质可以断裂 |
| **Material 0** | `true` | 第一个材质可断裂 |
| **Material 1** | `false` | 第二个材质不可断裂 |

#### 步骤2：使用蓝图函数设置
也可以通过蓝图函数动态设置：

```
Begin Play
└── Set All Materials Breakable
    ├── Target: ClothBreakableComponent
    └── Breakable: true
```

### 3. 物理参数配置

| 参数分类 | 参数名称 | 推荐值 | 说明 |
|----------|----------|--------|------|
| **物理设置** | **Fragment Mass** | 5.0-20.0 | 碎片质量 |
| | **Fragment Impulse Scale** | 50.0-200.0 | 碎片冲量倍率 |
| | **Enable Fragment Physics** | `true` | 启用碎片物理 |
| | **Fragment Collision** | `PhysicsActor` | 碎片碰撞配置 |

## 材质和视觉效果配置

### 1. 布料材质设置

#### 基础材质参数
| 参数 | 推荐设置 | 说明 |
|------|----------|------|
| **Blend Mode** | `Opaque` 或 `Masked` | 根据需要选择 |
| **Shading Model** | `Default Lit` | 标准光照 |
| **Two Sided** | `true` | 双面渲染 |

#### 断裂效果材质
为碎片创建专用材质：
1. 复制原始布料材质
2. 添加破损效果纹理
3. 调整颜色和粗糙度

### 2. 粒子效果配置

#### 断裂时的粒子效果
1. 创建粒子系统资产
2. 在ClothBreakableComponent中绑定事件：

```
On Cloth Break (事件)
├── Break Location (输出)
├── Break Radius (输出)
└── Spawn Emitter at Location
    ├── Emitter Template: 断裂粒子系统
    ├── Location: Break Location
    └── Auto Destroy: true
```

## 调试和测试配置

### 1. 启用调试可视化

#### 方法1：通过组件设置
在ClothBreakableComponent中：
| 参数 | 设置值 | 说明 |
|------|--------|------|
| **Enable Debug Visualization** | `true` | 启用调试显示 |
| **Debug Draw Duration** | 5.0 | 调试显示持续时间 |

#### 方法2：通过蓝图函数
```
Begin Play
└── Enable Debug Visualization
    ├── Target: ClothBreakableComponent
    ├── Enable: true
    └── Draw Duration: 3.0
```

### 2. 测试用子弹发射器

#### 创建简单的发射器
1. 创建新的Actor蓝图
2. 添加以下组件和逻辑：

```
Input Action (鼠标左键)
└── Spawn Actor from Class
    ├── Class: 子弹Actor类
    ├── Spawn Transform: 摄像机位置和旋转
    └── Spawn Collision Handling: Always Spawn
```

## 性能优化配置

### 1. 碎片数量限制

| 参数 | 推荐值 | 说明 |
|------|--------|------|
| **Max Fragments Per Break** | 10 | 单次断裂最大碎片数 |
| **Max Total Fragments** | 50 | 场景中最大碎片总数 |
| **Fragment Cleanup Interval** | 1.0 | 碎片清理间隔(秒) |

### 2. LOD和距离优化

| 参数 | 推荐值 | 说明 |
|------|--------|------|
| **Max Break Distance** | 1000.0 | 最大断裂距离 |
| **Fragment LOD Distance** | 500.0 | 碎片LOD切换距离 |
| **Disable Breaking Distance** | 2000.0 | 禁用断裂的距离 |

### 3. 内存优化

| 参数 | 推荐值 | 说明 |
|------|--------|------|
| **Fragment Pool Size** | 20 | 碎片对象池大小 |
| **Reuse Fragments** | `true` | 重用碎片对象 |
| **Auto Cleanup** | `true` | 自动清理无效碎片 |

## 常见配置问题解决

### 1. 布料不断裂
**检查项目**：
- [ ] ClothBreakableComponent已添加并配置
- [ ] Target Skeletal Mesh已正确设置
- [ ] Break Force Threshold不要设置过高
- [ ] 材质ID已设置为可断裂

### 2. 碎片生成异常
**检查项目**：
- [ ] Fragment Count范围合理(1-20)
- [ ] Fragment Size范围合理(1.0-50.0)
- [ ] Enable Fragment Physics已启用

### 3. 性能问题
**优化建议**：
- [ ] 减少Max Fragment Count
- [ ] 缩短Fragment Lifetime
- [ ] 启用Fragment Pool
- [ ] 设置合理的Max Break Distance

## 配置模板

### 轻量级配置（移动平台）
```
Break Force Threshold: 1500.0
Max Fragment Count: 5
Fragment Lifetime: 3.0
Max Break Distance: 500.0
Enable Fragment Physics: false
```

### 标准配置（PC平台）
```
Break Force Threshold: 2000.0
Max Fragment Count: 8
Fragment Lifetime: 5.0
Max Break Distance: 1000.0
Enable Fragment Physics: true
```

### 高质量配置（高端PC）
```
Break Force Threshold: 2500.0
Max Fragment Count: 12
Fragment Lifetime: 8.0
Max Break Distance: 1500.0
Enable Fragment Physics: true
Enable Debug Visualization: true
```

通过以上详细配置，您应该能够成功设置和使用ChaosClothBrokenEXT插件的所有功能。
