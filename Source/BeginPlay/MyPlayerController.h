// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

class UInventoryComponent;
class UW_Inventoryslots;

/**
 * 玩家控制器，负责玩家级 UI、输入模式和背包界面生命周期。
 *
 * 新架构职责：
 * - 不保存背包数据；背包数据由角色身上的 UInventoryComponent 管理。
 * - 不处理拾取射线；拾取仍由 AMyCharacter 完成。
 * - 只负责把玩家输入 B 转换成打开/关闭背包 UI。
 * - 打开背包时创建/显示 WBP_Inventoryslots，绑定 InventoryComponent::OnInventoryChanged。
 * - 关闭背包时移除 UI，解绑数据事件，恢复 GameOnly 输入模式。
 *
 * 主要调用链：
 * 1. 按 B -> ToggleInventory。
 * 2. ToggleInventory -> ResolveInventoryComponent，从当前 Pawn 查找 UInventoryComponent。
 * 3. OpenInventory -> AddToViewport -> RefreshInventoryWidget。
 * 4. InventoryComponent 数据变化 -> OnInventoryChanged -> HandleInventoryChanged -> RefreshInventoryWidget。
 * 5. 再按 B -> CloseInventory -> RemoveFromParent -> RestoreGameInputMode。
 */
UCLASS()
class BEGINPLAY_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	/** 绑定玩家级输入；背包 UI 输入放在 PlayerController，而不是 Character。 */
	virtual void SetupInputComponent() override;

	/** 根据当前背包 UI 状态打开或关闭背包。 */
	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	void ToggleInventory();

	/** 使用指定背包组件打开 UI，并开始监听它的数据变化。 */
	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	void OpenInventory(UInventoryComponent* InInventoryComponent);

	/** 关闭 UI，解绑背包数据事件，并恢复游戏输入状态。 */
	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	void CloseInventory();

	/** 查询背包 UI 当前是否处于打开状态。 */
	UFUNCTION(BlueprintPure, Category = "Inventory UI")
	bool IsInventoryOpen() const { return bIsInventoryOpen; }

protected:
	virtual void BeginPlay() override;

private:
	/** 背包数据变化时刷新 UI；该函数由 InventoryComponent::OnInventoryChanged 调用。 */
	UFUNCTION()
	void HandleInventoryChanged();

	/** 从当前控制的 Pawn 上查找背包数据组件。 */
	UInventoryComponent* ResolveInventoryComponent() const;

	/** 切换当前 UI 正在观察的背包组件，避免 UI 直接拥有背包数据。 */
	void BindInventoryComponent(UInventoryComponent* InInventoryComponent);

	/** 停止监听当前背包组件，避免关闭 UI 后继续收到刷新事件。 */
	void UnbindInventoryComponent();

	/** 把 ActiveInventoryComponent 的 Slots 快照传给 WBP_Inventoryslots。 */
	void RefreshInventoryWidget();

	/** 打开背包时应用 UI 输入状态：显示鼠标、禁用移动/视角输入。 */
	void ApplyInventoryInputMode();

	/** 关闭背包时恢复游戏输入状态。 */
	void RestoreGameInputMode();

	/** 背包主 UI 类，请在 PlayerController 蓝图或默认值中设置为 WBP_Inventoryslots。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory UI", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<UW_Inventoryslots> InventoryWidgetClass;

	/** 背包主 UI 实例，由 PlayerController 管理生命周期。 */
	UPROPERTY()
	TObjectPtr<UW_Inventoryslots> InventoryWidgetInstance;

	/** 当前正在显示并监听的背包数据组件。 */
	UPROPERTY()
	TObjectPtr<UInventoryComponent> ActiveInventoryComponent;

	/** PlayerController 内部 UI 状态，避免重复 AddToViewport 或重复 RemoveFromParent。 */
	bool bIsInventoryOpen = false;
};
