// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "ItemData.generated.h"

/**
 * UItemData — 物品静态数据资源。
 *
 * 代码功能：
 * - 作为 DataAsset 在编辑器中创建具体物品配置，例如草药、子弹、钥匙。
 * - 保存物品不会在运行时频繁变化的属性。
 * - 背包组件只引用这个资源，不复制名称、图标、描述等数据。
 *
 * 使用方式：
 * - 在内容浏览器中创建 Data Asset，选择 ItemData 类型。
 * - 配置名称、图标、最大堆叠数量和描述。
 * - 调用 AddItem 时传入该 DataAsset 即可添加对应物品。
 */
UCLASS(BlueprintType)
class BEGINPLAY_API UItemData : public UDataAsset
{
	GENERATED_BODY()

public:
	// 生成一个特殊且唯一的id
	// m7 -> m7-1
	// m7 -> m7-2
	// awm -> awm-2xx
	FText uuid;
	
	/** 显示在 UI 上的物品名称，例如“手枪子弹”。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FText ItemName;

	/** 显示在 UI 背包格子中的物品图标。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	TObjectPtr<UTexture2D> Icon = nullptr;

	/** 单个槽位允许堆叠的最大数量；例如子弹可堆叠，钥匙通常为 1。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (ClampMin = "1"))
	int32 MaxStack = 1;

	/** 显示在 UI 详情面板中的物品说明文本。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item", meta = (MultiLine = true))
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	bool Interactive;
};
