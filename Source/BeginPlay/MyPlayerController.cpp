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

	if (InputComponent)
	{
		InputComponent->BindKey(EKeys::B, IE_Pressed, this, &AMyPlayerController::ToggleInventory);
	}
}

void AMyPlayerController::ToggleInventory()
{
	if (bIsInventoryOpen)
	{
		CloseInventory();
		return;
	}

	OpenInventory(ResolveInventoryComponent());
}

void AMyPlayerController::OpenInventory(UInventoryComponent* InInventoryComponent)
{
	if (bIsInventoryOpen)
	{
		RefreshInventoryWidget();
		return;
	}

	if (!InInventoryComponent)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("没有找到 InventoryComponent，无法打开背包"));
		}
		return;
	}

	if (!InventoryWidgetInstance)
	{
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

	BindInventoryComponent(InInventoryComponent);
	InventoryWidgetInstance->AddToViewport();
	RefreshInventoryWidget();
	ApplyInventoryInputMode();
	bIsInventoryOpen = true;
}

void AMyPlayerController::CloseInventory()
{
	if (!bIsInventoryOpen)
	{
		return;
	}

	if (InventoryWidgetInstance)
	{
		InventoryWidgetInstance->ClearInventorySlots();
		InventoryWidgetInstance->RemoveFromParent();
	}

	UnbindInventoryComponent();
	RestoreGameInputMode();
	bIsInventoryOpen = false;
}

void AMyPlayerController::HandleInventoryChanged()
{
	RefreshInventoryWidget();
}

UInventoryComponent* AMyPlayerController::ResolveInventoryComponent() const
{
	const APawn* ControlledPawn = GetPawn();
	return ControlledPawn ? ControlledPawn->FindComponentByClass<UInventoryComponent>() : nullptr;
}

void AMyPlayerController::BindInventoryComponent(UInventoryComponent* InInventoryComponent)
{
	if (ActiveInventoryComponent == InInventoryComponent)
	{
		return;
	}

	UnbindInventoryComponent();
	ActiveInventoryComponent = InInventoryComponent;

	if (ActiveInventoryComponent)
	{
		ActiveInventoryComponent->OnInventoryChanged.AddDynamic(this, &AMyPlayerController::HandleInventoryChanged);
	}
}

void AMyPlayerController::UnbindInventoryComponent()
{
	if (ActiveInventoryComponent)
	{
		ActiveInventoryComponent->OnInventoryChanged.RemoveDynamic(this, &AMyPlayerController::HandleInventoryChanged);
		ActiveInventoryComponent = nullptr;
	}
}

void AMyPlayerController::RefreshInventoryWidget()
{
	if (InventoryWidgetInstance && ActiveInventoryComponent)
	{
		InventoryWidgetInstance->RefreshInventory(ActiveInventoryComponent->GetSlots());
	}
}

void AMyPlayerController::ApplyInventoryInputMode()
{
	if (ACharacter* ControlledCharacter = Cast<ACharacter>(GetPawn()))
	{
		if (UCharacterMovementComponent* MovementComponent = ControlledCharacter->GetCharacterMovement())
		{
			MovementComponent->DisableMovement();
		}
	}

	SetIgnoreMoveInput(true);
	SetIgnoreLookInput(true);
	SetInputMode(FInputModeGameAndUI());
	bShowMouseCursor = true;
}

void AMyPlayerController::RestoreGameInputMode()
{
	if (ACharacter* ControlledCharacter = Cast<ACharacter>(GetPawn()))
	{
		if (UCharacterMovementComponent* MovementComponent = ControlledCharacter->GetCharacterMovement())
		{
			MovementComponent->SetMovementMode(MOVE_Walking);
		}
	}

	SetIgnoreMoveInput(false);
	SetIgnoreLookInput(false);
	SetInputMode(FInputModeGameOnly());
	bShowMouseCursor = false;
}
