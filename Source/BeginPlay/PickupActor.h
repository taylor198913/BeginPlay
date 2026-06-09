// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupActor.generated.h"

class UItemData;
class UStaticMeshComponent;

/**
 * APickupActor — 场景中的可拾取物 Actor。
 *
 * 代码功能：
 * - 作为 BP_PickupActor 蓝图类的 C++ 父类。
 * - 提供一个静态网格体组件，用于在关卡中显示物品模型。
 * - 提供 ItemData 和 PickupCount，让每个场景实例可以配置“拾取什么”和“拾取几个”。
 *
 * 设计说明：
 * - ItemData 表示物品类型，例如草药、子弹、钥匙。
 * - PickupCount 表示当前这个场景实例被拾取时给予玩家的数量。
 * - 背包添加逻辑仍然由 UInventoryComponent 负责，不写进 Character。
 */
UCLASS(Blueprintable)
class BEGINPLAY_API APickupActor : public AActor
{
	GENERATED_BODY()

public:
	/** 构造函数：创建默认组件，并关闭 Tick。 */
	APickupActor();

	/** 当前拾取物对应的物品数据，可在蓝图或场景实例中设置。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup")
	TObjectPtr<UItemData> ItemData;
	
	/** 当前拾取物被拾取时给予玩家的数量，可针对每个场景实例单独设置。 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pickup", meta = (ClampMin = "1"))
	int32 PickupCount = 1;

	/** 获取静态网格体组件，方便蓝图或 C++ 设置模型。 */
	UFUNCTION(BlueprintPure, Category = "Pickup")
	UStaticMeshComponent* GetStaticMeshComponent() const;

	/** 获取当前拾取物绑定的物品数据，供交互逻辑读取后添加到背包。 */
	UFUNCTION(BlueprintPure, Category = "Pickup")
	UItemData* GetItemData() const;

	/** 获取当前拾取物的拾取数量，供交互逻辑传给背包组件。 */
	UFUNCTION(BlueprintPure, Category = "Pickup")
	int32 GetPickupCount() const;

protected:
	/** 场景中显示拾取物外观的静态网格体组件。 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
};
