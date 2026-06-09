// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInventoryComponent;

/**
 * 玩家角色类，负责角色本体行为和背包数据组件的归属。
 *
 * 新架构职责：
 * - 负责移动、摄像机、视角和拾取交互。
 * - 持有 UInventoryComponent，作为玩家背包数据的拥有者。
 * - 拾取物品时只调用 InventoryComponent->AddItem，不直接操作 UI。
 * - 不创建背包 Widget，不切换输入模式，不显示/隐藏鼠标。
 *
 * 主要调用链：
 * 1. Interact 输入 -> AMyCharacter::Interact。
 * 2. 角色从摄像机方向做射线检测。
 * 3. 命中 APickupActor 后读取 ItemData 和 PickupCount。
 * 4. 调用 InventoryComponent->AddItem。
 * 5. InventoryComponent 广播 OnInventoryChanged，后续由 AMyPlayerController 刷新 UI。
 */
UCLASS()
class BEGINPLAY_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AMyCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/** 获取角色身上的背包组件，供 PlayerController、UI 或交互系统读取背包数据入口。 */
	UFUNCTION(BlueprintPure, Category = "Inventory")
	UInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

protected:
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void Turn(float Value);
	void MoveRight(float Value);
	void LookUp(float Value);
	void Interact();

private:
	/** 第三人称摄像机弹簧臂。 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* SpringArm;

	/** 第三人称摄像机，也是拾取射线的起点。 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	/** 背包组件引用，由 BP_MyCharacter 挂载 BP_InventoryComponent，BeginPlay 时查找。 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory", meta = (AllowPrivateAccess = "true"))
	UInventoryComponent* InventoryComponent = nullptr;

	/** 交互射线距离，用于从摄像机方向检测 PickupActor。 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true", ClampMin = "0"))
	float InteractionTraceDistance = 500.0f;
};
