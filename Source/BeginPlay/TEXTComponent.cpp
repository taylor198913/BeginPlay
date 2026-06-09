// Fill out your copyright notice in the Description page of Project Settings.


#include "TEXTComponent.h"

#include "IDetailTreeNode.h"
#include "ItemData.h"
#include "Chaos/ChaosPerfTest.h"

// Sets default values for this component's properties
UTEXTComponent::UTEXTComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

/**
 * 我的目标是添加物品进入背包数组，
 * 当函数触发首先检查添加进入的数量是否大于了背包相同物品格子最大还能装多少的数量加上空白格子乘上该物品的最大堆叠数量，
 * 
 * 如果大于则返回false,如果为true，
 * 
 * 则遍历每个格子，如果交互物品的信息和数组的信息相同，
 * 或者是空白格子，则执行添加操作，
 * 
 * 一次添加可能不够，
 * 创建一个变量承接剩下的数量去下一个格子添加直到为0返回true
 */
bool UTEXTComponent::AddItem(UItemData* ItemData, int32 Count)
{
	if (ItemData == nullptr) return false;
	if (Count <= 0) return false;
	
	// 如果背包剩余数量小于将要添加数量则直接终止函数
	if (RemainingCount(ItemData)<Count)
	{
		return false;
	}

	// RemainingCount 表示还有多少数量没有放入背包。
	int32 RemainingCount = Count;
	
	for (FInventorySlot& slot :Slots)
   	{
   		//优先堆叠背白已有的，如果有剩下的，则找空格子，塞入其中
	 if(slot.ItemData==ItemData)
	 {
		  const int32 StackSpace = ItemData->MaxStack - slot.Count;
		  const int32 AmountToAdd = FMath::Min(StackSpace,RemainingCount);
	 	
		  slot.Count += AmountToAdd;
		  RemainingCount -= AmountToAdd;

	 	if (RemainingCount <= 0) break;
	 }
   }

	for (FInventorySlot& slot :Slots)
	{
		//优先堆叠背白已有的，如果有剩下的，则找空格子，塞入其中
		if(slot.ItemData==nullptr && slot.Count == 0)
		{
			
		}
	}

	
	return true;
}


// Called when the game starts
void UTEXTComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


int UTEXTComponent::RemainingCount(UItemData*ItemData)
{
	//检查放入物品数量能否一次性进入背包
	//如果可以返回false
	int TotalAmount=0;
	//
    for (FInventorySlot& slot :Slots)
    {
    	//
    	
	    if (slot.ItemData==nullptr||slot.ItemData==ItemData)
	    {
	    	//如果为空，则按照交互物品的最大数
		    if (slot.ItemData==nullptr)
		    {
			    TotalAmount += ItemData->MaxStack;
		    }

	    	if (slot.ItemData==ItemData)
	    	{
	    		const int RemainingCount=ItemData->MaxStack-slot.Count;
	    		TotalAmount += RemainingCount;
	    	}
	    }
    return TotalAmount;
    	
    }
	
	return false;
}

// Called every frame
void UTEXTComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

