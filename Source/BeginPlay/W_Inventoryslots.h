// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryTypes.h"
#include "W_Inventoryslots.generated.h"

/**
 * 背包主 UI 的 C++ 基类，负责承接背包 Slots 快照并交给蓝图绘制。
 *
 * 新架构职责：
 * - 不保存背包原始数据，只接收 InventoryComponent 传来的 Slots 快照。
 * - 不直接修改 InventoryComponent::Slots。
 * - 派生蓝图 WBP_Inventoryslots 负责 WrapBox 清空、循环创建 WBP_Slot、显示详情面板。
 * - 打开/关闭生命周期由 AMyPlayerController 管理。
 *
 * 主要调用链：
 * 1. AMyPlayerController::OpenInventory 创建并显示 WBP_Inventoryslots。
 * 2. PlayerController 调用 RefreshInventory(Slots)。
 * 3. WBP_Inventoryslots 蓝图清空旧格子，循环 Slots 创建 WBP_Slot。
 * 4. 每个 WBP_Slot 调用 RefreshSlot(SlotData) 显示单格内容。
 * 5. CloseInventory 前由 PlayerController 调用 ClearInventorySlots。
 */
UCLASS(BlueprintType, Blueprintable)
class BEGINPLAY_API UW_Inventoryslots : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * 打开或刷新背包 UI。
	 *
	 * Slots 是背包组件当前槽位数据的只读快照。
	 * 派生蓝图应在这里清空旧格子，然后循环 Slots 创建 WBP_Slot。
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Inventory")
	void RefreshInventory(const TArray<FInventorySlot>& Slots);

	/**
	 * 关闭背包 UI 前清理格子。
	 *
	 * 派生蓝图应在这里调用 WrapBox 的 ClearChildren，避免下次打开时重复子控件。
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Inventory")
	void ClearInventorySlots();
};
