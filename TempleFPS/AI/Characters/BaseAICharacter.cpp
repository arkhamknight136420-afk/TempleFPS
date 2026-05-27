#include "BaseAICharacter.h"

#include "../../ActorComponents/HealthComponent.h"
#include "../../ActorComponents/DeathComponent.h"
#include "../Controllers/BaseAIController.h"
#include "../../Inventory/Components/InventoryComponent.h"
#include "../../Weapons/WeaponTypes/WeaponBase.h"
#include "Components/ChildActorComponent.h"

// Sets default values
ABaseAICharacter::ABaseAICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = ABaseAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	DeathComponent = CreateDefaultSubobject<UDeathComponent>(TEXT("DeathComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);

	HeldWeaponComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("HeldWeaponComponent"));
	HeldWeaponComponent->SetupAttachment(GetMesh(), TEXT("WeaponSocket"));
	
}

void ABaseAICharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("AI InventoryComponent is NULL in BeginPlay"));
	}

	if (DefaultWeaponClass)
	{
		HeldWeaponComponent->SetChildActorClass(DefaultWeaponClass);

		if (AWeaponBase* Weapon = Cast<AWeaponBase>(HeldWeaponComponent->GetChildActor()))
		{
			Weapon->SetOwner(this);
			Weapon->SetActorEnableCollision(false);
		}
	}
}

void ABaseAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

UInventoryComponent* ABaseAICharacter::GetInventoryComponent() const
{
	if (InventoryComponent)
	{
		return InventoryComponent.Get();
	}

	return FindComponentByClass<UInventoryComponent>();
}

void ABaseAICharacter::AttachWeaponToCharacter(AWeaponBase* WeaponToAttach)
{
	if (!WeaponToAttach)
	{
		UE_LOG(LogTemp, Error, TEXT("AI WeaponToAttach is null. Cannot attach weapon."));
		return;
	}

	WeaponToAttach->AttachToComponent(
		GetMesh(),
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		WeaponToAttach->AttachSocketName
	);

	WeaponToAttach->SetActorRelativeTransform(
		WeaponToAttach->ThirdPersonGripOffset
	);
}

void ABaseAICharacter::EquipPrimaryWeapon()
{
	UInventoryComponent* Inventory = GetInventoryComponent();

	if (!Inventory)
	{
		UE_LOG(LogTemp, Error, TEXT("AI InventoryComponent is null. Cannot equip primary weapon."));
		return;
	}

	Inventory->EquipPrimaryWeapon();
}

void ABaseAICharacter::EquipSecondaryWeapon()
{
	UInventoryComponent* Inventory = GetInventoryComponent();

	if (!Inventory)
	{
		UE_LOG(LogTemp, Error, TEXT("AI InventoryComponent is null. Cannot equip secondary weapon."));
		return;
	}

	Inventory->EquipSecondaryWeapon();
}

void ABaseAICharacter::StartShooting()
{
	UInventoryComponent* Inventory = GetInventoryComponent();

	if (!Inventory)
	{
		UE_LOG(LogTemp, Error, TEXT("AI InventoryComponent is null. Cannot start shooting."));
		return;
	}

	AWeaponBase* CurrentWeapon = Inventory->GetCurrentHeldWeapon();

	if (!CurrentWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("AI has no current weapon equipped. Cannot shoot."));
		return;
	}

	CurrentWeapon->StartFire();
}

void ABaseAICharacter::StopShooting()
{
	UInventoryComponent* Inventory = GetInventoryComponent();

	if (!Inventory)
	{
		UE_LOG(LogTemp, Error, TEXT("AI InventoryComponent is null. Cannot stop shooting."));
		return;
	}

	AWeaponBase* CurrentWeapon = Inventory->GetCurrentHeldWeapon();

	if (!CurrentWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("AI has no current weapon equipped. Cannot stop shooting."));
		return;
	}

	CurrentWeapon->StopFire();
}

void ABaseAICharacter::ReloadWeapon()
{
	UInventoryComponent* Inventory = GetInventoryComponent();

	if (!Inventory)
	{
		UE_LOG(LogTemp, Error, TEXT("AI InventoryComponent is null. Cannot reload."));
		return;
	}

	AWeaponBase* CurrentWeapon = Inventory->GetCurrentHeldWeapon();

	if (!CurrentWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("AI has no current weapon equipped. Cannot reload."));
		return;
	}

	CurrentWeapon->Reload();
}