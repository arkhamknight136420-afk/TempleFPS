#include "BaseAICharacter.h"

#include "../../ActorComponents/HealthComponent.h"
#include "../../ActorComponents/DeathComponent.h"
#include "../Controllers/BaseAIController.h"
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
	
	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);

	HeldWeaponComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("HeldWeaponComponent"));
	HeldWeaponComponent->SetupAttachment(GetMesh(), TEXT("WeaponSocket"));
	
}

void ABaseAICharacter::BeginPlay()
{
	Super::BeginPlay();

	

	if (DefaultWeaponClass)
	{
		HeldWeaponComponent->SetChildActorClass(DefaultWeaponClass);

		if (AWeaponBase* Weapon = Cast<AWeaponBase>(HeldWeaponComponent->GetChildActor()))
		{
			Weapon->SetOwner(this);
			Weapon->SetWeaponEquipped();
			AttachWeaponToCharacter(Weapon);
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
	


}

void ABaseAICharacter::EquipSecondaryWeapon()
{
	

}

void ABaseAICharacter::StartShooting()
{
	



}

void ABaseAICharacter::StopShooting()
{


}

void ABaseAICharacter::ReloadWeapon()
{


	

	
}