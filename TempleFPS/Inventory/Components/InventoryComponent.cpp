// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "../../Weapons/WeaponTypes/PrimaryWeapons/PrimaryWeaponBase.h"
#include "../../Weapons/WeaponTypes/SecondaryWeapons/SecondaryWeaponBase.h"
#include "../../Player/Characters/FPSPlayerCharacter.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (PrimaryWeapon)
	{
		EquipPrimaryWeapon();
	}
	else if (SecondaryWeapon)
	{
		EquipSecondaryWeapon();
	}
	else
	{
		CurrentHeldWeapon = nullptr;
		UE_LOG(LogTemp, Warning, TEXT("No  Weapon To Equip At Start Of Round"));
	}
	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInventoryComponent::PickUpWeapon(TSubclassOf<AWeaponBase> WeaponClass)
{
	if (!WeaponClass)
	{
		UE_LOG(LogTemp, Error, TEXT("PickUpWeapon failed: WeaponClass is null."));
		return;
	}

	AFPSPlayerCharacter* Player = Cast<AFPSPlayerCharacter>(GetOwner());

	if (!Player)
	{
		UE_LOG(LogTemp, Error, TEXT("PickUpWeapon failed: Inventory owner is not AFPSPlayerCharacter."));
		return;
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Player;
	SpawnParams.Instigator = Player;

	AWeaponBase* SpawnedWeapon = GetWorld()->SpawnActor<AWeaponBase>(
		WeaponClass,
		Player->GetActorLocation(),
		Player->GetActorRotation(),
		SpawnParams
	);

	if (!SpawnedWeapon)
	{
		UE_LOG(LogTemp, Error, TEXT("PickUpWeapon failed: Could not spawn weapon."));
		return;
	}

	SpawnedWeapon->SetOwner(Player);
	SpawnedWeapon->SetActorHiddenInGame(true);
	SpawnedWeapon->SetActorEnableCollision(false);

	if (APrimaryWeaponBase* NewPrimaryWeapon = Cast<APrimaryWeaponBase>(SpawnedWeapon))
	{
		PrimaryWeapon = NewPrimaryWeapon;
		UE_LOG(LogTemp, Warning, TEXT("Picked up Primary Weapon: %s"), *PrimaryWeapon->GetName());
		EquipPrimaryWeapon();
	}
	else if (ASecondaryWeaponBase* NewSecondaryWeapon = Cast<ASecondaryWeaponBase>(SpawnedWeapon))
	{
		SecondaryWeapon = NewSecondaryWeapon;
		UE_LOG(LogTemp, Warning, TEXT("Picked up Secondary Weapon: %s"), *SecondaryWeapon->GetName());
		EquipSecondaryWeapon();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawned weapon is not primary or secondary."));
		SpawnedWeapon->Destroy();
	}
}

void UInventoryComponent::EquipPrimaryWeapon()
{
	if (!PrimaryWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Primary Weapon To Equip"));
		return;
	}

	AFPSPlayerCharacter* Player = Cast<AFPSPlayerCharacter>(GetOwner());

	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Player Character To Equip Primary Weapon To"));
		return;
	}

	if (CurrentHeldWeapon)
	{
		CurrentHeldWeapon->SetActorHiddenInGame(true);
	}

	CurrentHeldWeapon = PrimaryWeapon;

	CurrentHeldWeapon->SetActorHiddenInGame(false);
	CurrentHeldWeapon->SetActorEnableCollision(false);
	CurrentHeldWeapon->SetOwner(Player);

	Player->AttachWeaponToCharacter(CurrentHeldWeapon);

	UE_LOG(LogTemp, Warning, TEXT("Equipped Primary Weapon: %s"), *PrimaryWeapon->GetName());
}

void UInventoryComponent::EquipSecondaryWeapon()
{
	if (!SecondaryWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Secondary Weapon To Equip"));
		return;
	}

	AFPSPlayerCharacter* Player = Cast<AFPSPlayerCharacter>(GetOwner());

	if (!Player)
	{
		UE_LOG(LogTemp, Warning, TEXT("No Player Character To Equip Secondary Weapon To"));
		return;
	}

	if (CurrentHeldWeapon)
	{
		CurrentHeldWeapon->SetActorHiddenInGame(true);
	}

	CurrentHeldWeapon = SecondaryWeapon;

	CurrentHeldWeapon->SetActorHiddenInGame(false);
	CurrentHeldWeapon->SetActorEnableCollision(false);
	CurrentHeldWeapon->SetOwner(Player);

	Player->AttachWeaponToCharacter(CurrentHeldWeapon);

	UE_LOG(LogTemp, Warning, TEXT("Equipped Secondary Weapon: %s"), *SecondaryWeapon->GetName());
}