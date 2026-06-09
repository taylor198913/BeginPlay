// Fill out your copyright notice in the Description page of Project Settings.

#include "MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/SpringArmComponent.h"
#include "InputCoreTypes.h"
#include "InventoryComponent.h"
#include "ItemData.h"
#include "PickupActor.h"

AMyCharacter::AMyCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// 创建第三人称摄像机弹簧臂，摄像机会挂在弹簧臂上。
	SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring Arm Comp"));
	SpringArm->SetupAttachment(GetRootComponent());

	// 创建第三人称摄像机。
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera Comp"));
	Camera->SetupAttachment(SpringArm);
}

void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// 背包组件由 BP_MyCharacter 蓝图挂载，C++ 在运行开始时查找引用。
	InventoryComponent = FindComponentByClass<UInventoryComponent>();
	if (!InventoryComponent && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("角色没有挂载 InventoryComponent"));
	}
}

void AMyCharacter::MoveForward(float Value)
{
	if (Controller && (Value != 0.f))
	{
		const FRotator ControlRotator = GetControlRotation();
		const FRotator YawRotator(0.f, ControlRotator.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotator).GetScaledAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AMyCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void AMyCharacter::MoveRight(float Value)
{
	if (Controller && (Value != 0.f))
	{
		const FRotator ControlRotator = GetControlRotation();
		const FRotator YawRotator(0.f, ControlRotator.Yaw, 0.f);
		const FVector Direction = FRotationMatrix(YawRotator).GetScaledAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AMyCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void AMyCharacter::Interact()
{
	if (!Camera)
	{
		return;
	}

	// 从摄像机位置向前发射射线，用来检测玩家准星前方的可拾取 Actor。
	const FVector TraceStart = Camera->GetComponentLocation();
	const FVector TraceEnd = TraceStart + Camera->GetForwardVector() * InteractionTraceDistance;

	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECC_Visibility, QueryParams);

	DrawDebugLine(GetWorld(), TraceStart, TraceEnd, bHit ? FColor::Green : FColor::Red, false, 2.0f, 0, 1.5f);

	if (!bHit)
	{
		return;
	}

	// 只有命中 APickupActor 时才进入拾取逻辑。
	APickupActor* PickupActor = Cast<APickupActor>(HitResult.GetActor());
	if (!PickupActor)
	{
		return;
	}

	UItemData* PickupItemData = PickupActor->GetItemData();
	if (!PickupItemData)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, TEXT("命中的 PickupActor 没有设置 ItemData"));
		}
		return;
	}

	const int32 PickupCount = PickupActor->GetPickupCount();

	// 把物品数据交给背包组件处理，角色本身不直接修改 Slots。
	if (!InventoryComponent || !InventoryComponent->AddItem(PickupItemData, PickupCount))
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("背包空间不足，无法拾取"));
		}
		return;
	}

	if (GEngine)
	{
		const FString Message = FString::Printf(TEXT("拾取：%s x%d"), *PickupItemData->ItemName.ToString(), PickupCount);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Cyan, Message);
	}

	PickupActor->Destroy();
}

void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(FName("MoveForward"), this, &AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis(FName("Turn"), this, &AMyCharacter::Turn);
	PlayerInputComponent->BindAxis(FName("MoveRight"), this, &AMyCharacter::MoveRight);
	PlayerInputComponent->BindAxis(FName("LookUp"), this, &AMyCharacter::LookUp);

	PlayerInputComponent->BindAction(FName("Interact"), IE_Pressed, this, &AMyCharacter::Interact);
}
