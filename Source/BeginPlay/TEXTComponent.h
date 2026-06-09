// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryTypes.h"
#include "TEXTComponent.generated.h"

class UItemData;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BEGINPLAY_API UTEXTComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTEXTComponent();

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(UItemData* ItemData,int32 Count);
	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere,Blueprintreadonly,Category="Inventory")
	TArray<FInventorySlot>Slots;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	int RemainingCount(UItemData* Count);
	

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
