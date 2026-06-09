// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryTypes.h"
#include "W_Slot.generated.h"

/**
 * 单个背包格子的 C++ 基类，负责把一格数据交给蓝图显示。
 *
 * 新架构职责：
 * - 不保存或修改背包原始 Slots。
 * - 只根据传入的 FInventorySlot 快照显示图标、名称、数量等信息。
 * - 后续如果要做点击、悬浮、拖拽，应由 WBP_Slot 发事件给父级 WBP_Inventoryslots，
 *   再由父级或 PlayerController 调用 InventoryComponent 的公开函数修改数据。
 *
 * 主要调用链：
 * 1. WBP_Inventoryslots::RefreshInventory 循环 Slots。
 * 2. 每个槽位创建一个 WBP_Slot。
 * 3. 调用 WBP_Slot::RefreshSlot(SlotData)。
 * 4. WBP_Slot 从 SlotData.ItemData 读取 ItemName / Icon / Description，从 SlotData.Count 读取数量。
 */
UCLASS(BlueprintType, Blueprintable)
class BEGINPLAY_API UW_Slot : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * 刷新单个格子的显示。
	 * SlotData 是只读输入，蓝图只读取它，不修改背包原始数据。
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Inventory Slot")
	void RefreshSlot(const FInventorySlot& SlotData);
};
