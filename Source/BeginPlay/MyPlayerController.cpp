// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerController.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h"
#include "InputCoreTypes.h"
#include "InventoryComponent.h"
#include "W_Inventoryslots.h"

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void AMyPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 背包属于玩家级 UI，因此 B 键绑定放在 PlayerController，而不是 Character。
	// 按下 B 后会进入 ToggleInventory，根据当前状态决定打开或关闭背包。
	if (InputComponent)
	{
		InputComponent->BindKey(EKeys::B, IE_Pressed, this, &AMyPlayerController::ToggleInventory);
	}
}

void AMyPlayerController::ToggleInventory()
{
	// 当前已经打开背包时，B 键表示关闭背包。
	if (bIsInventoryOpen)
	{
		CloseInventory();
		return;
	}

	// 当前未打开背包时，从当前控制的 Pawn 身上查找背包组件并打开 UI。
	OpenInventory(ResolveInventoryComponent());
}

void AMyPlayerController::OpenInventory(UInventoryComponent* InInventoryComponent)
{
	// 如果外部重复调用打开函数，不重复 AddToViewport，只刷新当前显示内容。
	if (bIsInventoryOpen)
	{
		RefreshInventoryWidget();
		return;
	}

	// 背包 UI 必须绑定一个数据源；没有 InventoryComponent 就不能打开背包。
	if (!InInventoryComponent)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("没有找到 InventoryComponent，无法打开背包"));
		}
		return;
	}

	// UI 实例采用懒创建：第一次打开时创建，后续重复使用同一个 Widget 实例。
	if (!InventoryWidgetInstance)
	{
		// InventoryWidgetClass 应在 BP_MyPlayerController 中设置为 WBP_Inventoryslots。
		if (!InventoryWidgetClass)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("没有设置 InventoryWidgetClass"));
			}
			return;
		}

		InventoryWidgetInstance = CreateWidget<UW_Inventoryslots>(this, InventoryWidgetClass);
	}

	if (!InventoryWidgetInstance)
	{
		return;
	}

	// 打开流程顺序：绑定数据源 -> 显示 UI -> 立即刷新 -> 切换输入模式 -> 记录状态。
	BindInventoryComponent(InInventoryComponent);
	InventoryWidgetInstance->AddToViewport();
	RefreshInventoryWidget();
	ApplyInventoryInputMode();
	bIsInventoryOpen = true;
}

void AMyPlayerController::CloseInventory()
{
	// 背包未打开时忽略关闭请求，避免重复 RemoveFromParent。
	if (!bIsInventoryOpen)
	{
		return;
	}

	if (InventoryWidgetInstance)
	{
		// 关闭前让主背包 UI 清空格子子控件，避免下次打开时重复创建残留。
		InventoryWidgetInstance->ClearInventorySlots();
		InventoryWidgetInstance->RemoveFromParent();
	}

	// 关闭 UI 后不再监听背包数据变化，并把输入状态恢复给游戏。
	UnbindInventoryComponent();
	RestoreGameInputMode();
	bIsInventoryOpen = false;
}

void AMyPlayerController::HandleInventoryChanged()
{
	// InventoryComponent 的 Slots 改变时会广播 OnInventoryChanged。
	// PlayerController 只负责把最新数据重新推给 UI。
	RefreshInventoryWidget();
}

UInventoryComponent* AMyPlayerController::ResolveInventoryComponent() const
{
	// 当前架构下，背包数据组件由 Character 拥有。
	// PlayerController 不直接保存背包数据，只在需要打开 UI 时从当前 Pawn 查找。
	const APawn* ControlledPawn = GetPawn();
	return ControlledPawn ? ControlledPawn->FindComponentByClass<UInventoryComponent>() : nullptr;
}

void AMyPlayerController::BindInventoryComponent(UInventoryComponent* InInventoryComponent)
{
	// 如果已经绑定的是同一个组件，不重复绑定委托。
	if (ActiveInventoryComponent == InInventoryComponent)
	{
		return;
	}

	// 切换数据源前先解绑旧组件，避免同一个 Controller 同时监听多个背包。
	UnbindInventoryComponent();
	ActiveInventoryComponent = InInventoryComponent;

	if (ActiveInventoryComponent)
	{
		// AddDynamic 绑定后，InventoryComponent 数据变化会回调 HandleInventoryChanged。
		ActiveInventoryComponent->OnInventoryChanged.AddDynamic(this, &AMyPlayerController::HandleInventoryChanged);
	}
}

void AMyPlayerController::UnbindInventoryComponent()
{
	if (ActiveInventoryComponent)
	{
		// 关闭背包时解绑委托，避免 UI 已关闭后仍然收到刷新回调。
		ActiveInventoryComponent->OnInventoryChanged.RemoveDynamic(this, &AMyPlayerController::HandleInventoryChanged);
		ActiveInventoryComponent = nullptr;
	}
}

void AMyPlayerController::RefreshInventoryWidget()
{
	// UI 只接收 Slots 快照；真正的背包数据仍由 InventoryComponent 维护。
	if (InventoryWidgetInstance && ActiveInventoryComponent)
	{
		InventoryWidgetInstance->RefreshInventory(ActiveInventoryComponent->GetSlots());
	}
}

void AMyPlayerController::ApplyInventoryInputMode()
{
	// 打开背包时暂停角色移动，让玩家专注于鼠标操作 UI。
	if (ACharacter* ControlledCharacter = Cast<ACharacter>(GetPawn()))
	{
		if (UCharacterMovementComponent* MovementComponent = ControlledCharacter->GetCharacterMovement())
		{
			MovementComponent->DisableMovement();
		}
	}

	// 禁止移动和视角输入，同时切换到 GameAndUI，允许 UI 接收鼠标点击。
	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);
	SetInputMode(FInputModeGameAndUI());
	bShowMouseCursor = true;
}

void AMyPlayerController::RestoreGameInputMode()
{
	// 关闭背包后恢复角色行走能力。
	if (ACharacter* ControlledCharacter = Cast<ACharacter>(GetPawn()))
	{
		if (UCharacterMovementComponent* MovementComponent = ControlledCharacter->GetCharacterMovement())
		{
			MovementComponent->SetMovementMode(MOVE_Walking);
		}
	}

	// 输入控制权交还给游戏，隐藏鼠标并恢复移动/视角输入。
	SetIgnoreMoveInput(false);
	SetIgnoreLookInput(false);
	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;
}
