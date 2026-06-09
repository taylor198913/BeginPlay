# BeginPlay - Unreal Engine C++ Inventory Demo

这是一个基于 **Unreal Engine 5.4** 的 C++ 学习项目，当前重点实现了一个可扩展的背包系统 Demo。

修改 1

项目包含角色移动、射线交互、拾取物、物品数据资产、背包数据组件、UMG 背包界面，以及重构后的 PlayerController UI 管理架构。

## 项目目标

本项目用于练习 Unreal Engine C++ Gameplay 开发，重点关注：

- Actor / Component 设计
- C++ 与蓝图协作
- UMG 背包 UI
- DataAsset 物品配置
- 玩家交互射线
- 背包数据与 UI 解耦
- PlayerController 管理 UI 输入模式

## 当前功能

### 角色基础功能

- 第三人称角色移动
- 摄像机和 SpringArm
- 鼠标视角控制
- `E` 键射线交互

### 拾取系统

- `APickupActor` 表示场景中的可拾取物
- 每个拾取物可配置：
  - `ItemData`
  - `PickupCount`
- 玩家交互命中拾取物后，会把物品添加到背包
- 添加成功后销毁场景拾取物

### 物品数据

物品静态信息使用 `UItemData` DataAsset 管理，包括：

- 物品名称
- 图标
- 最大堆叠数量
- 描述文本

### 背包系统

`UInventoryComponent` 负责背包数据：

- 槽位数组 `Slots`
- 添加物品 `AddItem`
- 移除物品 `RemoveItem`
- 查找物品 `FindItem`
- 获取槽位快照 `GetSlots`
- 扩展槽位数量 `Diliattion`
- 数据变化广播 `OnInventoryChanged`

### 背包 UI

- `UW_Inventoryslots`：背包主 UI C++ 基类
- `UW_Slot`：单个格子 UI C++ 基类
- 蓝图负责实际控件布局、图标和数量显示
- 打开背包时刷新所有槽位
- 背包数据变化后自动刷新 UI

## 架构设计

当前背包系统采用职责分层设计：

```text
AMyCharacter
├── 角色移动
├── 摄像机
├── 射线交互
├── 拾取 PickupActor
└── 持有 InventoryComponent

UInventoryComponent
├── Slots 数据
├── AddItem
├── RemoveItem
├── FindItem
├── GetSlots
├── Diliattion
└── OnInventoryChanged

AMyPlayerController
├── B 键打开/关闭背包
├── 创建 WBP_Inventoryslots
├── AddToViewport / RemoveFromParent
├── 绑定 InventoryComponent.OnInventoryChanged
├── 刷新背包 UI
├── 切换输入模式
├── 显示/隐藏鼠标
└── 禁用/恢复角色移动

UW_Inventoryslots
├── 接收 Slots 快照
├── 清空 WrapBox
├── 循环创建 WBP_Slot
└── 关闭前清理格子

UW_Slot
├── 接收 FInventorySlot
├── 显示 ItemData 信息
└── 显示数量
```

### 打开背包调用链

```text
玩家按 B
↓
AMyPlayerController::ToggleInventory
↓
AMyPlayerController::ResolveInventoryComponent
↓
从当前 Pawn 查找 UInventoryComponent
↓
AMyPlayerController::OpenInventory
↓
CreateWidget / AddToViewport
↓
AMyPlayerController::RefreshInventoryWidget
↓
UW_Inventoryslots::RefreshInventory
↓
WBP_Inventoryslots 蓝图循环 Slots
↓
创建 WBP_Slot
↓
UW_Slot::RefreshSlot
```

### 拾取刷新调用链

```text
玩家按 E
↓
AMyCharacter::Interact
↓
射线检测 APickupActor
↓
读取 ItemData / PickupCount
↓
UInventoryComponent::AddItem
↓
修改 Slots
↓
OnInventoryChanged.Broadcast
↓
AMyPlayerController::HandleInventoryChanged
↓
RefreshInventoryWidget
↓
背包 UI 自动刷新
```

### 关闭背包调用链

```text
玩家再次按 B
↓
AMyPlayerController::ToggleInventory
↓
AMyPlayerController::CloseInventory
↓
UW_Inventoryslots::ClearInventorySlots
↓
RemoveFromParent
↓
UnbindInventoryComponent
↓
RestoreGameInputMode
```

## 操作方式

| 操作 | 按键 |
| --- | --- |
| 前进 / 后退 | `W` / `S` |
| 左右移动 | `A` / `D` |
| 视角 | 鼠标 |
| 交互 / 拾取 | `E` |
| 打开 / 关闭背包 | `B` |

## 主要源码文件

```text
Source/BeginPlay/
├── MyCharacter.h / .cpp
│   └── 玩家角色、移动、摄像机、交互、拾取
│
├── MyPlayerController.h / .cpp
│   └── 背包 UI 打开关闭、输入模式、鼠标、UI 刷新
│
├── InventoryComponent.h / .cpp
│   └── 背包数据组件、添加/移除/查找物品、数据变化事件
│
├── InventoryTypes.h
│   └── FInventorySlot 槽位结构
│
├── ItemData.h / .cpp
│   └── 物品 DataAsset 配置
│
├── PickupActor.h / .cpp
│   └── 场景可拾取物 Actor
│
├── W_Inventoryslots.h / .cpp
│   └── 背包主 UI C++ 基类
│
└── W_Slot.h / .cpp
    └── 单个背包格子 UI C++ 基类
```

## 运行环境

- Unreal Engine：5.4
- 语言：C++ / Blueprint
- 平台：Windows
- IDE：Visual Studio / Rider / Cursor 均可

模块依赖：

```csharp
Core
CoreUObject
Engine
InputCore
EnhancedInput
UMG
```

## 如何运行

1. 克隆项目：

```bash
git clone <your-repository-url>
```

2. 使用 Unreal Engine 5.4 打开：

```text
BeginPlay.uproject
```

3. 如果需要，右键 `.uproject`：

```text
Generate Visual Studio project files
```

4. 编译：

```text
Development Editor / Win64
```

5. 打开 UE 后运行当前地图，测试角色移动、拾取和背包 UI。

## UE 蓝图配置说明

为了让新架构正常工作，需要在 UE 中确认以下配置：

### PlayerController

创建或使用 `AMyPlayerController` 的蓝图子类，例如：

```text
BP_MyPlayerController
```

并设置：

```text
InventoryWidgetClass = WBP_Inventoryslots
```

### GameMode

在 GameMode 或 Project Settings 中设置：

```text
Player Controller Class = BP_MyPlayerController
```

### Character

`AMyCharacter` 会在 C++ 构造函数中自动创建：

```text
InventoryComponent
```

因此角色蓝图不需要再手动挂载 `BP_InventoryComponent`。如果旧的 `BP_MyCharacter` 里已经手动添加过背包组件，建议删除那个蓝图组件，避免角色身上出现两个背包组件。

### Widget

`WBP_Inventoryslots` 应继承自：

```text
UW_Inventoryslots
```

`WBP_Slot` 应继承自：

```text
UW_Slot
```

## 常见问题

### 编译出现 `C3859` / `C1076` / `paging file is too small`

这是 Windows 虚拟内存不足，不是代码语法错误。

建议：

- 关闭 Unreal Editor 后再编译
- 增大 Windows 页面文件
- 降低 UE 编译并行数量

示例：

```powershell
& "D:\unreal版本\UE_5.4\Engine\Build\BatchFiles\Build.bat" BeginPlayEditor Win64 Development -Project="d:\UnrealCpp\BeginPlay\BeginPlay.uproject" -WaitMutex -FromMsBuild -MaxParallelActions=1
```

### `FImageUtils::CompressImageArray` Warning

这是 UnrealMCP 插件中的废弃 API 警告，不影响当前项目运行。

```text
Warning C4996 : FImageUtils::CompressImageArray
```

如果编译最终成功，可以暂时忽略。

## 后续计划

可以继续扩展以下功能：

- 格子点击显示物品详情
- 使用物品 `UseItem`
- 丢弃物品 `DropItem`
- 拖拽交换格子 `SwapSlots`
- 快捷栏 Hotbar
- 存档 / 读档
- 物品分类和排序
- 拾取提示 UI

## 项目状态

当前项目处于 Gameplay 系统学习和功能原型阶段，重点展示 Unreal C++ 与 UMG 蓝图协作，以及背包系统的基础架构设计。
