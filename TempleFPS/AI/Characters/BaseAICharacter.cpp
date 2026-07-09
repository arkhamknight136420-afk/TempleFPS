#include "BaseAICharacter.h"

#include "../../ActorComponents/HealthComponent.h"
#include "../../ActorComponents/DeathComponent.h"
#include "../Controllers/BaseAIController.h"
#include "../../Weapons/WeaponTypes/WeaponBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../../Characters/BaseCharacter.h"
#include "Components/ChildActorComponent.h"
#include "Kismet/KismetMathLibrary.h"


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

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 180.f, 0.f);

	EyesLocation->SetRelativeLocation(FVector(0.f, 0.f, 75.f));
	
}

void ABaseAICharacter::BeginPlay()
{
	Super::BeginPlay();

	

	if (DefaultWeaponClass)
	{
		HeldWeaponComponent->SetChildActorClass(DefaultWeaponClass);

		CurrentHeldWeapon = Cast<AWeaponBase>(HeldWeaponComponent->GetChildActor());

		if(IsValid(CurrentHeldWeapon))
		{
			CurrentHeldWeapon->SetOwner(this);
			CurrentHeldWeapon->SetWeaponEquipped();
			AttachWeaponToCharacter(CurrentHeldWeapon);
		}
		else
		{
			UE_LOG(LogTemp,Error, TEXT("[BaseAICharacter] | BeginPlay: Current Held Weapon is not valid "))
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

void ABaseAICharacter::HandleDirectionalMovement(FVector2D MoveInput)
{
	AddMovementInput(GetActorForwardVector(), MoveInput.Y);
	AddMovementInput(GetActorRightVector(), MoveInput.X);
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
		if (CurrentHeldWeapon->IsMagazineEmpty())
		{
			ReloadWeapon();
			return;
		}
		CurrentHeldWeapon->StartFire();
}

void ABaseAICharacter::StopShooting()
{
	CurrentHeldWeapon->StopFire();
	

}

void ABaseAICharacter::ReloadWeapon()
{
	CurrentHeldWeapon->Reload();

	
	

	
}

void ABaseAICharacter::UpdateEyePitch(AActor* Target)
{
	if (!Target)
	{
		return;
	}

	FRotator LookAt =
		UKismetMathLibrary::FindLookAtRotation(
			EyesLocation->GetComponentLocation(),
			Target->GetActorLocation());

	EyesLocation->SetRelativeRotation(
		FRotator(LookAt.Pitch, 0.f, 0.f));
}

void ABaseAICharacter::SetAimTarget(AActor* Target)
{
	CurrentAimTarget = Target;
}

FVector ABaseAICharacter::GetAimDirection() const
{
	if (CurrentAimTarget)
	{
		FVector ToTarget =
			CurrentAimTarget->GetActorLocation() - EyesLocation->GetComponentLocation();

		const float Distance = ToTarget.Size();

		FVector ExactAimDirection = ToTarget.GetSafeNormal();

		float BloomRadians = FMath::Atan(MissRadius / Distance);

		float BloomDegrees = FMath::RadiansToDegrees(BloomRadians);

		BloomDegrees = FMath::Clamp(
			BloomDegrees,
			MinBloomDegrees,
			MaxBloomDegrees
		);

		BloomRadians = FMath::DegreesToRadians(BloomDegrees);

		return FMath::VRandCone(ExactAimDirection, BloomRadians).GetSafeNormal();
	}

	return Super::GetAimDirection();
}

// create bloom 

//