// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryTypes.generated.h"

class UItemData;

// 结构体 / 类
// 大致作用是一致的，都能够描述一个对象：属性/方法

// 但是类可以声明私有化的属性/方法


/**
 * FInventorySlot — 背包中的一个槽位。
 *
 * 代码功能：
 * - 用于保存背包运行时的一格数据。
 * - ItemData 指向物品的静态配置资源，例如名称、图标、最大堆叠数。
 * - Count 表示当前槽位中该物品的数量。
 *
 * 设计说明：
 * - 槽位本身只保存运行时状态，不保存物品描述文本、图标等静态信息。
 * - 使用 USTRUCT(BlueprintType) 可以让 UI 蓝图直接读取槽位数据。
 */
USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()

	/** 当前槽位中的物品数据；为空表示该槽位没有有效物品。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	TObjectPtr<UItemData> ItemData = nullptr;

	/** 当前槽位中的物品数量；小于等于 0 时视为无效数量。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 Count = 0;

	/** 判断当前槽位是否为空，供背包查找和清理逻辑使用。 */
	bool IsEmpty() const
	{
		return ItemData == nullptr || Count <= 0;
	}
		
	
	// 判断这个槽位是否可用
	// 槽位可用依据可能有很多
	// 1. 槽位是否是空的
	// 2. 其他情况 -> 
	bool CanUse() const
	{
		return this->IsEmpty();
	}
};
