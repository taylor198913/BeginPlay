// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryTypes.h"
#include "InventoryComponent.generated.h"

class UItemData;

/** 背包数据发生变化时广播；UI 或其他系统可以监听，但组件本身不依赖 UI 类型。 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

/**
 * 背包组件 C++ 基类，作为背包系统的数据层。
 *
 * 新架构职责：
 * - 保存 Slots 运行时数据。
 * - 处理 AddItem、RemoveItem、FindItem、扩容等背包规则。
 * - 数据变化后广播 OnInventoryChanged。
 * - 不创建 UI，不 AddToViewport，不切换输入模式，不持有 WBP_Inventoryslots。
 *
 * 主要调用链：
 * 1. AMyCharacter::Interact 命中 APickupActor。
 * 2. Character 读取 ItemData / PickupCount。
 * 3. Character 调用 InventoryComponent->AddItem。
 * 4. AddItem 修改 Slots 后调用 OnInventoryChanged.Broadcast。
 * 5. AMyPlayerController 监听该事件并刷新 WBP_Inventoryslots。
 *
 * Blueprintable：允许创建 BP_InventoryComponent。
 * BlueprintSpawnableComponent：允许在 BP_MyCharacter 中 Add Component。
 */
UCLASS(BlueprintType, Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BEGINPLAY_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	/** 添加物品；成功修改 Slots 后广播 OnInventoryChanged。 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(UItemData* ItemData, int32 Count);

	/** 按物品类型移除指定数量；成功修改 Slots 后广播 OnInventoryChanged。 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(UItemData* ItemData, int32 Count);

	/** 查找第一个包含指定物品的非空槽位，未找到时返回空槽位。 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	FInventorySlot FindItem(UItemData* ItemData) const;

	/** 返回 Slots 快照，供 UI 刷新显示；外部不应直接修改原始 Slots。 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	TArray<FInventorySlot> GetSlots() const;

	/** 扩展当前可用槽位数量；成功后广播 OnInventoryChanged。 */
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void Diliattion();

	/** 数据层对外通知入口；PlayerController/UI 可监听它来刷新显示。 */
	UPROPERTY(BlueprintAssignable, Category = "Inventory")
	FOnInventoryChanged OnInventoryChanged;

	/** 背包允许扩展到的最大槽位数。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = "0"))
	int32 MaxSlots = 12;

	/** 当前实际可用槽位数量。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (ClampMin = "1"))
	int32 CurrentSlotCount = 6;

protected:
	/** 背包运行时槽位数据，只由 InventoryComponent 自己维护。 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FInventorySlot> Slots;

private:
	int32 GetTotalItemCount(UItemData* ItemData) const;
	int32 GetRemainingCapacity(UItemData* ItemData) const;
};
