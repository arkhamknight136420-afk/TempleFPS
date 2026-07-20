#include "InventoryComponent.h"

#include "../../Weapons/WeaponTypes/WeaponBase.h"
#include "../../Weapons/WeaponTypes/PrimaryWeapons/PrimaryWeaponBase.h"
#include "../../Weapons/WeaponTypes/SecondaryWeapons/SecondaryWeaponBase.h"

#include "../../Player/Characters/FPSPlayerCharacter.h"
#include "../../AI/Characters/BaseAICharacter.h"
#include "Kismet/GameplayStatics.h"

#include "GameFramework/Character.h"


// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (DefaultPrimaryWeaponClass)
	{
		PickUpWeapon(DefaultPrimaryWeaponClass);

	


	}
	else if (DefaultSecondaryWeaponClass)
	{
		PickUpWeapon(DefaultSecondaryWeaponClass);
	}
	else
	{
		CurrentHeldWeapon = nullptr;

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("No default weapon assigned.")
		);
	}
}

// Called every frame
void UInventoryComponent::TickComponent(
	float DeltaTime,
	ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction
)
{
	Super::TickComponent(
		DeltaTime,
		TickType,
		ThisTickFunction
	);
}

void UInventoryComponent::PickUpWeapon(
	TSubclassOf<AWeaponBase> WeaponClass
)
{
	if (!WeaponClass)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("PickUpWeapon failed: WeaponClass is null.")
		);

		return;
	}

	if (IsValid(CurrentHeldWeapon) &&
		WeaponClass == CurrentHeldWeapon->GetClass())
	{
		const int32 RequestedAmmo = CurrentHeldWeapon->GetAmmoGrantedPerPickup();

		const int32 ActualAmmoAdded = CurrentHeldWeapon->AddAmmo(RequestedAmmo);

		return;

	}
	

	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());

	if (!CharacterOwner)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("PickUpWeapon failed: Inventory owner is not a Character.")
		);

		return;
	}


	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = CharacterOwner;
	SpawnParams.Instigator = CharacterOwner;

	AWeaponBase* SpawnedWeapon =
		GetWorld()->SpawnActor<AWeaponBase>(
			WeaponClass,
			CharacterOwner->GetActorLocation(),
			CharacterOwner->GetActorRotation(),
			SpawnParams
		);

	if (!SpawnedWeapon)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT("PickUpWeapon failed: Could not spawn weapon.")
		);

		return;
	}

	SpawnedWeapon->SetOwner(CharacterOwner);
	SpawnedWeapon->SetActorHiddenInGame(true);
	SpawnedWeapon->SetActorEnableCollision(false);

	if (APrimaryWeaponBase* NewPrimaryWeapon =
		Cast<APrimaryWeaponBase>(SpawnedWeapon))
	{
		PrimaryWeapon = NewPrimaryWeapon;

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Picked up Primary Weapon: %s"),
			*PrimaryWeapon->GetName()
		);

		EquipPrimaryWeapon();
	}
	else if (ASecondaryWeaponBase* NewSecondaryWeapon =
		Cast<ASecondaryWeaponBase>(SpawnedWeapon))
	{
		SecondaryWeapon = NewSecondaryWeapon;

		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Picked up Secondary Weapon: %s"),
			*SecondaryWeapon->GetName()
		);

		EquipSecondaryWeapon();
	}
	else
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("Spawned weapon is not primary or secondary.")
		);

		SpawnedWeapon->Destroy();
	}
}

void UInventoryComponent::EquipPrimaryWeapon()
{
	if (!PrimaryWeapon)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("No Primary Weapon To Equip")
		);

		return;
	}

	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());

	if (!CharacterOwner)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("No Character owner to equip primary weapon to.")
		);

		return;
	}
	AWeaponBase* PreviousHeldWeapon = CurrentHeldWeapon;

	if (CurrentHeldWeapon)
	{
		CurrentHeldWeapon->SetActorHiddenInGame(true);
	}

	CurrentHeldWeapon = PrimaryWeapon;

	CurrentHeldWeapon->PlayPickupSFX();

	CurrentHeldWeapon->SetOwner(CharacterOwner);
	CurrentHeldWeapon->SetActorHiddenInGame(false);
	CurrentHeldWeapon->SetActorEnableCollision(false);

	if (UStaticMeshComponent* WeaponMesh = CurrentHeldWeapon->GetWeaponMesh())
	{
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (AFPSPlayerCharacter* Player =
		Cast<AFPSPlayerCharacter>(CharacterOwner))
	{
		Player->AttachWeaponToCharacter(CurrentHeldWeapon);
	}
	else if (ABaseAICharacter* AICharacter =
		Cast<ABaseAICharacter>(CharacterOwner))
	{
		AICharacter->AttachWeaponToCharacter(CurrentHeldWeapon);
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Equipped Primary Weapon: %s"),
		*PrimaryWeapon->GetName()
	);

	if (PreviousHeldWeapon != CurrentHeldWeapon)
	{
		OnCurrentWeaponChanged.Broadcast(CurrentHeldWeapon);
	}
}

void UInventoryComponent::EquipSecondaryWeapon()
{
	if (!SecondaryWeapon)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("No Secondary Weapon To Equip")
		);

		return;
	}

	ACharacter* CharacterOwner = Cast<ACharacter>(GetOwner());

	if (!CharacterOwner)
	{
		UE_LOG(
			LogTemp,
			Warning,
			TEXT("No Character owner to equip secondary weapon to.")
		);

		return;
	}

	if (CurrentHeldWeapon)
	{
		CurrentHeldWeapon->SetActorHiddenInGame(true);
	}

	CurrentHeldWeapon = SecondaryWeapon;

	CurrentHeldWeapon->SetOwner(CharacterOwner);
	CurrentHeldWeapon->SetActorHiddenInGame(false);
	CurrentHeldWeapon->SetActorEnableCollision(false);

	if (UStaticMeshComponent* WeaponMesh = CurrentHeldWeapon->GetWeaponMesh())
	{
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (AFPSPlayerCharacter* Player =
		Cast<AFPSPlayerCharacter>(CharacterOwner))
	{
		Player->AttachWeaponToCharacter(CurrentHeldWeapon);
	}
	else if (ABaseAICharacter* AICharacter =
		Cast<ABaseAICharacter>(CharacterOwner))
	{
		AICharacter->AttachWeaponToCharacter(CurrentHeldWeapon);
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("Equipped Secondary Weapon: %s"),
		*SecondaryWeapon->GetName()
	);
}