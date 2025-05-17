# UE5插件重命名指南

本文档详细介绍了如何将UE5插件从一个名称重命名为另一个名称（例如从BlankPlugin重命名为ClothBroken）的完整流程。

## 目录

1. [概述](#概述)
2. [准备工作](#准备工作)
3. [文件夹结构修改](#文件夹结构修改)
4. [文件重命名](#文件重命名)
5. [文件内容修改](#文件内容修改)
6. [重新编译](#重新编译)
7. [自动化脚本](#自动化脚本)
8. [常见问题](#常见问题)

## 概述

重命名UE5插件涉及多个方面的修改，包括：

- 文件夹结构
- 文件名
- 文件内容（类名、命名空间、宏定义等）
- 编译设置

本指南以将插件从"BlankPlugin"重命名为"ClothBroken"为例，详细说明每个步骤。

## 准备工作

在开始重命名之前，请确保：

1. **备份整个项目**：重命名过程可能会导致错误，确保有备份可以恢复
2. **关闭Unreal Editor**：避免文件锁定问题
3. **关闭Visual Studio或其他IDE**：避免文件锁定问题
4. **记录原始插件的依赖关系**：确保重命名后不会丢失依赖

## 文件夹结构修改

### 1. 主文件夹重命名

将插件的主文件夹重命名：

```
BlankPlugin/ → ClothBroken/
```

### 2. 源代码文件夹重命名

将Source下的模块文件夹重命名：

```
ClothBroken/Source/BlankPlugin/ → ClothBroken/Source/ClothBroken/
```

如果有多个模块，需要对每个模块进行相应修改。

## 文件重命名

### 1. 插件描述文件

```
BlankPlugin.uplugin → ClothBroken.uplugin
```

### 2. 构建文件

```
Source/BlankPlugin/BlankPlugin.Build.cs → Source/ClothBroken/ClothBroken.Build.cs
```

### 3. 模块实现文件

```
Source/BlankPlugin/Public/BlankPlugin.h → Source/ClothBroken/Public/ClothBroken.h
Source/BlankPlugin/Private/BlankPlugin.cpp → Source/ClothBroken/Private/ClothBroken.cpp
```

### 4. 其他相关文件

如果插件包含其他带有原插件名称的文件，也需要相应重命名。

## 文件内容修改

### 1. 插件描述文件 (.uplugin)

编辑 `ClothBroken.uplugin` 文件，修改以下内容：

```json
{
  "FileVersion": 3,
  "Version": 1,
  "VersionName": "1.0",
  "FriendlyName": "ClothBroken",  // 修改
  "Description": "...",
  "Category": "...",
  "CreatedBy": "...",
  "CreatedByURL": "...",
  "DocsURL": "...",
  "MarketplaceURL": "...",
  "SupportURL": "...",
  "CanContainContent": true,
  "IsBetaVersion": false,
  "IsExperimentalVersion": false,
  "Installed": false,
  "Modules": [
    {
      "Name": "ClothBroken",  // 修改
      "Type": "Runtime",
      "LoadingPhase": "Default"
    }
  ]
}
```

### 2. 构建文件 (.Build.cs)

编辑 `ClothBroken.Build.cs` 文件，修改类名：

```csharp
// 修改前
public class BlankPlugin : ModuleRules
{
    public BlankPlugin(ReadOnlyTargetRules Target) : base(Target)
    {
        // ...
    }
}

// 修改后
public class ClothBroken : ModuleRules
{
    public ClothBroken(ReadOnlyTargetRules Target) : base(Target)
    {
        // ...
    }
}
```

### 3. 模块头文件 (.h)

编辑 `ClothBroken.h` 文件，修改类名和宏定义：

```cpp
// 修改前
class FBlankPluginModule : public IModuleInterface
{
public:
    static inline FBlankPluginModule& Get()
    {
        return FModuleManager::LoadModuleChecked<FBlankPluginModule>("BlankPlugin");
    }

    static inline bool IsAvailable()
    {
        return FModuleManager::Get().IsModuleLoaded("BlankPlugin");
    }
    
    // ...
};

// 修改后
class FClothBrokenModule : public IModuleInterface
{
public:
    static inline FClothBrokenModule& Get()
    {
        return FModuleManager::LoadModuleChecked<FClothBrokenModule>("ClothBroken");
    }

    static inline bool IsAvailable()
    {
        return FModuleManager::Get().IsModuleLoaded("ClothBroken");
    }
    
    // ...
};
```

### 4. 模块实现文件 (.cpp)

编辑 `ClothBroken.cpp` 文件，修改类名和宏定义：

```cpp
// 修改前
#include "BlankPlugin.h"

#define LOCTEXT_NAMESPACE "FBlankPluginModule"

void FBlankPluginModule::StartupModule()
{
    // ...
}

void FBlankPluginModule::ShutdownModule()
{
    // ...
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FBlankPluginModule, BlankPlugin)

// 修改后
#include "ClothBroken.h"

#define LOCTEXT_NAMESPACE "FClothBrokenModule"

void FClothBrokenModule::StartupModule()
{
    // ...
}

void FClothBrokenModule::ShutdownModule()
{
    // ...
}

#undef LOCTEXT_NAMESPACE
    
IMPLEMENT_MODULE(FClothBrokenModule, ClothBroken)
```

### 5. API宏定义

在所有头文件中，修改API宏定义：

```cpp
// 修改前
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyClass.generated.h"

UCLASS()
class BLANKPLUGIN_API UMyClass : public UObject
{
    GENERATED_BODY()
    
    // ...
};

// 修改后
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MyClass.generated.h"

UCLASS()
class CLOTHBROKEN_API UMyClass : public UObject
{
    GENERATED_BODY()
    
    // ...
};
```

### 6. 包含路径

在所有源文件中，更新包含路径：

```cpp
// 修改前
#include "BlankPlugin/Public/MyClass.h"

// 修改后
#include "ClothBroken/Public/MyClass.h"
```

### 7. 其他引用

搜索并替换所有源文件中的其他引用：

- `BlankPlugin` → `ClothBroken`
- `BLANKPLUGIN` → `CLOTHBROKEN`
- `FBlankPluginModule` → `FClothBrokenModule`

## 重新编译

### 1. 清理中间文件

删除以下目录中的所有文件：
- `ClothBroken/Intermediate/`
- `ClothBroken/Binaries/`

### 2. 重新生成项目文件

右键点击 `.uproject` 文件，选择 "Generate Visual Studio project files"。

### 3. 重新编译插件

打开解决方案并重新编译插件。

## 自动化脚本

如果您需要经常重命名插件，可以使用以下PowerShell脚本自动化这个过程：

```powershell
param (
    [string]$OldName = "BlankPlugin",
    [string]$NewName = "ClothBroken"
)

# 1. 重命名文件夹
Rename-Item -Path $OldName -NewName $NewName
Set-Location $NewName

# 2. 重命名插件文件
Rename-Item -Path "$OldName.uplugin" -NewName "$NewName.uplugin"

# 3. 更新插件文件内容
$upluginContent = Get-Content -Path "$NewName.uplugin" -Raw
$upluginContent = $upluginContent -replace $OldName, $NewName
Set-Content -Path "$NewName.uplugin" -Value $upluginContent

# 4. 重命名Source文件夹
Rename-Item -Path "Source\$OldName" -NewName "Source\$NewName"

# 5. 重命名Build.cs文件
Rename-Item -Path "Source\$NewName\$OldName.Build.cs" -NewName "Source\$NewName\$NewName.Build.cs"

# 6. 更新Build.cs文件内容
$buildContent = Get-Content -Path "Source\$NewName\$NewName.Build.cs" -Raw
$buildContent = $buildContent -replace $OldName, $NewName
Set-Content -Path "Source\$NewName\$NewName.Build.cs" -Value $buildContent

# 7. 重命名模块文件
Rename-Item -Path "Source\$NewName\Public\$OldName.h" -NewName "Source\$NewName\Public\$NewName.h"
Rename-Item -Path "Source\$NewName\Private\$OldName.cpp" -NewName "Source\$NewName\Private\$NewName.cpp"

# 8. 更新模块文件内容
$headerContent = Get-Content -Path "Source\$NewName\Public\$NewName.h" -Raw
$headerContent = $headerContent -replace $OldName, $NewName
$headerContent = $headerContent -replace $OldName.ToUpper(), $NewName.ToUpper()
Set-Content -Path "Source\$NewName\Public\$NewName.h" -Value $headerContent

$cppContent = Get-Content -Path "Source\$NewName\Private\$NewName.cpp" -Raw
$cppContent = $cppContent -replace $OldName, $NewName
$cppContent = $cppContent -replace $OldName.ToUpper(), $NewName.ToUpper()
Set-Content -Path "Source\$NewName\Private\$NewName.cpp" -Value $cppContent

# 9. 更新所有源文件中的引用
$sourceFiles = Get-ChildItem -Path "Source\$NewName" -Recurse -Include "*.h", "*.cpp"
foreach ($file in $sourceFiles) {
    $content = Get-Content -Path $file.FullName -Raw
    $content = $content -replace $OldName, $NewName
    $content = $content -replace ($OldName + "_API"), ($NewName + "_API")
    $content = $content -replace $OldName.ToUpper(), $NewName.ToUpper()
    Set-Content -Path $file.FullName -Value $content
}

# 10. 清理中间文件
if (Test-Path "Intermediate") {
    Remove-Item -Path "Intermediate" -Recurse -Force
}
if (Test-Path "Binaries") {
    Remove-Item -Path "Binaries" -Recurse -Force
}

Write-Host "插件重命名完成：$OldName -> $NewName"
```

使用方法：

```
.\RenamePlugin.ps1 -OldName "BlankPlugin" -NewName "ClothBroken"
```

## 常见问题

### 1. 编译错误

**问题**：重命名后出现编译错误。

**解决方案**：
- 检查是否有遗漏的引用
- 确保所有API宏定义都已更新
- 删除Intermediate和Binaries文件夹，重新生成项目文件

### 2. 插件无法加载

**问题**：插件在编辑器中无法加载。

**解决方案**：
- 检查.uplugin文件中的模块名称是否正确
- 确保Build.cs文件中的类名与模块名称匹配
- 检查IMPLEMENT_MODULE宏中的参数是否正确

### 3. 类找不到

**问题**：使用插件的项目报错，找不到某些类。

**解决方案**：
- 更新项目中对插件类的所有引用
- 检查包含路径是否正确
- 重新编译依赖于此插件的项目

### 4. 版本控制问题

**问题**：重命名后Git或SVN等版本控制系统无法正确跟踪文件。

**解决方案**：
- 使用版本控制系统的重命名命令（如`git mv`）而不是直接重命名文件
- 如果已经重命名，可能需要手动告诉版本控制系统文件已被重命名

## 特殊情况

### 保留部分原名称

在某些情况下，您可能希望将插件重命名为新名称，但保留内部模块的原始名称。例如，将插件重命名为ClothBroken，但保留ChaosClothBrokenEXT作为内部模块名称。

在这种情况下，您需要：

1. 重命名插件主文件夹和.uplugin文件
2. 在.uplugin文件中，将模块名称保留为原始名称
3. 不重命名Source下的模块文件夹
4. 不修改模块内部的类名和宏定义

这种方法可以减少需要修改的文件数量，但可能导致命名不一致的问题。
