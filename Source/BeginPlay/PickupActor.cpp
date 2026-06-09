// Fill out your copyright notice in the Description page of Project Settings.

#include "PickupActor.h"
#include "Components/StaticMeshComponent.h"
#include "ItemData.h"

APickupActor::APickupActor()
{
	// 拾取物 Actor 当前不需要每帧逻辑，因此关闭 Tick 以节省性能。
	PrimaryActorTick.bCanEverTick = false;

	// 创建静态网格体组件，用于在关卡中显示拾取物模型。
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));

	// 将静态网格体组件设置为根组件，方便直接移动、旋转和缩放整个拾取物。
	RootComponent = StaticMeshComponent;

	// 默认开启碰撞，方便以后扩展射线检测、重叠检测或交互检测。
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	
}



UStaticMeshComponent* APickupActor::GetStaticMeshComponent() const
{
	// 返回静态网格体组件，供蓝图设置模型或读取组件信息。
	return StaticMeshComponent;
}

UItemData* APickupActor::GetItemData() const
{
	// 返回该拾取物绑定的物品数据，交互系统可根据它调用背包组件 AddItem。
	return ItemData;
}

int32 APickupActor::GetPickupCount() const
{
	// 返回该场景拾取物实例配置的拾取数量，最小有效值为 1。
	return FMath::Max(PickupCount, 1);
}
