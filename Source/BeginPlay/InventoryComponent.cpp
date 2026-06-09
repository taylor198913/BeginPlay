// Fill out your copyright notice in the Description page of Project Settings.

#include "InventoryComponent.h"
#include "ItemData.h"

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	Slots.SetNum(CurrentSlotCount);
}

bool UInventoryComponent::AddItem(UItemData* ItemData, int32 Count)
{
	if (!ItemData || Count <= 0 || ItemData->MaxStack <= 0)
	{
		return false;
	}

	if (GetRemainingCapacity(ItemData) < Count)
	{
		return false;
	}

	int32 RemainingCount = Count;

	for (FInventorySlot& Slot : Slots)
	{
		if (Slot.ItemData != ItemData || Slot.Count >= ItemData->MaxStack)
		{
			continue;
		}

		const int32 StackSpace = ItemData->MaxStack - Slot.Count;
		const int32 AmountToAdd = FMath::Min(StackSpace, RemainingCount);
		Slot.Count += AmountToAdd;
		RemainingCount -= AmountToAdd;

		if (RemainingCount <= 0)
		{
			OnInventoryChanged.Broadcast();
			return true;
		}
	}

	for (FInventorySlot& Slot : Slots)
	{
		if (!Slot.IsEmpty())
		{
			continue;
		}

		Slot.ItemData = ItemData;
		Slot.Count = FMath::Min(RemainingCount, ItemData->MaxStack);
		RemainingCount -= Slot.Count;

		if (RemainingCount <= 0)
		{
			OnInventoryChanged.Broadcast();
			return true;
		}
	}

	return false;
}

bool UInventoryComponent::RemoveItem(UItemData* ItemData, int32 Count)
{
	if (!ItemData || Count <= 0)
	{
		return false;
	}

	if (GetTotalItemCount(ItemData) < Count)
	{
		return false;
	}

	int32 RemainingCount = Count;

	for (int32 SlotIndex = Slots.Num() - 1; SlotIndex >= 0 && RemainingCount > 0; --SlotIndex)
	{
		FInventorySlot& Slot = Slots[SlotIndex];
		if (Slot.ItemData != ItemData)
		{
			continue;
		}

		if (Slot.Count <= RemainingCount)
		{
			RemainingCount -= Slot.Count;
			Slot.ItemData = nullptr;
			Slot.Count = 0;
		}
		else
		{
			Slot.Count -= RemainingCount;
			RemainingCount = 0;
		}
	}

	OnInventoryChanged.Broadcast();
	return true;
}

FInventorySlot UInventoryComponent::FindItem(UItemData* ItemData) const
{
	if (!ItemData)
	{
		return FInventorySlot();
	}

	for (const FInventorySlot& Slot : Slots)
	{
		if (Slot.ItemData == ItemData && !Slot.IsEmpty())
		{
			return Slot;
		}
	}

	return FInventorySlot();
}

TArray<FInventorySlot> UInventoryComponent::GetSlots() const
{
	return Slots;
}

void UInventoryComponent::Diliattion()
{
	CurrentSlotCount = FMath::Clamp(CurrentSlotCount + 2, 1, MaxSlots);
	Slots.SetNum(CurrentSlotCount);
	OnInventoryChanged.Broadcast();
}

int32 UInventoryComponent::GetTotalItemCount(UItemData* ItemData) const
{
	if (!ItemData)
	{
		return 0;
	}

	int32 TotalCount = 0;
	for (const FInventorySlot& Slot : Slots)
	{
		if (Slot.ItemData == ItemData)
		{
			TotalCount += Slot.Count;
		}
	}

	return TotalCount;
}

int32 UInventoryComponent::GetRemainingCapacity(UItemData* ItemData) const
{
	if (!ItemData || ItemData->MaxStack <= 0)
	{
		return 0;
	}

	int32 RemainingCapacity = 0;

	for (const FInventorySlot& Slot : Slots)
	{
		if (Slot.ItemData == ItemData && Slot.Count < ItemData->MaxStack)
		{
			RemainingCapacity += ItemData->MaxStack - Slot.Count;
		}
		else if (Slot.IsEmpty())
		{
			RemainingCapacity += ItemData->MaxStack;
		}
	}

	return RemainingCapacity;
}
