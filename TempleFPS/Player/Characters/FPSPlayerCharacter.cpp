#include "FPSPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../FiniteStateMachines/MovementStates/BaseMovementState.h"
#include "../Bralns/FPSBrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "..\..\InteractionInterface.h"
#include "../../Inventory/Components/InventoryComponent.h"
#include "../../Weapons/WeaponTypes/WeaponBase.h"
#include "../../ActorComponents/HealthComponent.h"
#include "../../ActorComponents/DeathComponent.h"
#include "../../ActorComponents/UCharacterAudioComponent.h"
#include "../../Characters/BaseCharacter.h"

AFPSPlayerCharacter::AFPSPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;


	CharacterMovementComponent = GetCharacterMovement();
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	BrainComponent = CreateDefaultSubobject<UFPSBrainComponent>(TEXT("BrainComponent"));

	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(GetCapsuleComponent());
	SpringArmComponent->TargetArmLength = 300.0f;
	SpringArmComponent->bUsePawnControlRotation = true;
	SpringArmComponent->SocketOffset = FVector(0.f, 70.f, 50.f);

	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECR_Block);


	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	DeathComponent = CreateDefaultSubobject<UDeathComponent>(TEXT("DeathComponent"));

	CharacterAudioComponent = CreateDefaultSubobject<UUCharacterAudioComponent>(TEXT("CharacterAudioComponent"));

		
	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCamera->SetupAttachment(SpringArmComponent);
	PlayerCamera->bUsePawnControlRotation = false;
	PlayerCamera->FieldOfView = DefaultFieldOfView;



	

	InteractionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("InteractionCapsule"));
	InteractionCapsule->SetupAttachment(PlayerCamera);
	InteractionCapsule->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionCapsule->SetGenerateOverlapEvents(true);
	InteractionCapsule->SetCollisionObjectType(ECC_WorldDynamic);
	InteractionCapsule->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionCapsule->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	InteractionCapsule->SetRelativeLocation(
		FVector(SpringArmComponent->TargetArmLength + InteractionCapsule->GetScaledCapsuleHalfHeight() + 60.f, 0.f, 0.f)
	);
	InteractionCapsule->SetRelativeRotation(FRotator(-90, 0, 0));
	InteractionCapsule->SetCapsuleHalfHeight(125.f);
	InteractionCapsule->SetCapsuleRadius(25.f);

	EyesLocation->SetupAttachment(PlayerCamera);
	EyesLocation->SetRelativeLocation(FVector(300.f, 0.f, 0.f));
	

	
}
void AFPSPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (!InventoryComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryComponent is NULL in BeginPlay"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent exists in BeginPlay: %s"), *InventoryComponent->GetName());
	}
	if (InteractionCapsule)
	{
		InteractionCapsule->OnComponentBeginOverlap.AddDynamic(
			this,
			&AFPSPlayerCharacter::OnInteractionCapsuleBeginOverlap
		);

		InteractionCapsule->OnComponentEndOverlap.AddDynamic(
			this,
			&AFPSPlayerCharacter::OnInteractionCapsuleEndOverlap
		);
	}
}

void AFPSPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (IsInterpolatingAim)
	{
		HandleADSInterpolation();
	}

}

void AFPSPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AFPSPlayerCharacter::OnInteractionCapsuleBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	UE_LOG(LogTemp, Warning, TEXT("Begin OVERLAP"));
	if (OtherActor->Implements<UInteractionInterface>())
	{
		CurrentInteractableActor = OtherActor;
		UE_LOG(LogTemp, Warning, TEXT("CurrentInteractableActor Added"));

	}
}

void AFPSPlayerCharacter::OnInteractionCapsuleEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex
)
{
	UE_LOG(LogTemp, Warning, TEXT("End OVERLAP"));

	if (CurrentInteractableActor)
	{
		CurrentInteractableActor = nullptr;
	}

	UE_LOG(LogTemp, Warning, TEXT("CurrentInteractableActor Removed"));
	
}

void AFPSPlayerCharacter::HandleInteract()
{
	if (CurrentInteractableActor)
	{
		
		IInteractionInterface::Execute_Interact(CurrentInteractableActor, this);
	}
}

void AFPSPlayerCharacter::StartJumpMovement()
{
	Jump();
	UE_LOG(LogTemp, Log, TEXT("StartJumpMovement called"));
}

void AFPSPlayerCharacter::StartCrouchMovement()
{
	GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchSpeed;
	GetCharacterMovement()->CrouchedHalfHeight = CrouchedCapsuleHalfHeight;
	Crouch();

	UE_LOG(LogTemp, Log, TEXT("StartCrouchMovement called"));
}

void AFPSPlayerCharacter::StopCrouchMovement()
{
	UnCrouch();

	UE_LOG(LogTemp, Log, TEXT("StopCrouchMovement called"));
}

void AFPSPlayerCharacter::HandleDirectionalMovement(FVector2D MoveInput)
{
	UE_LOG(LogTemp, Warning, TEXT("[CHARACTER] HandleDirectionalMovement: X=%f Y=%f"), MoveInput.X, MoveInput.Y);

	AddMovementInput(GetActorForwardVector(), MoveInput.Y);
	AddMovementInput(GetActorRightVector(), MoveInput.X);
}

void AFPSPlayerCharacter::StartSlowWalkMovement()
{
	GetCharacterMovement()->MaxWalkSpeed = SlowWalkSpeed;
	UE_LOG(LogTemp, Log, TEXT("StartSlowWalkMovement called"));
}

void AFPSPlayerCharacter::StopSlowWalkMovement()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	UE_LOG(LogTemp, Log, TEXT("StopSprintMovement called"));
}




void AFPSPlayerCharacter::StartWalkMovement()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void AFPSPlayerCharacter::StopWalkMovement()
{

}


bool AFPSPlayerCharacter::IsGrounded()
{
	if (GetCharacterMovement()->IsMovingOnGround())
	{
		return true;
	}
	return false;
}


void AFPSPlayerCharacter::StartShooting()
{
	UInventoryComponent* Inventory = GetInventoryComponent();

	if (!Inventory)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryComponent is null. Cannot start shooting."));
		return;
	}

	AWeaponBase* CurrentWeapon = Inventory->GetCurrentHeldWeapon();

	if (!CurrentWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("No current weapon equipped. Cannot shoot."));
		return;
	}

	CurrentWeapon->StartFire();
}


void AFPSPlayerCharacter::StopShooting()
{
	UInventoryComponent* Inventory = GetInventoryComponent();

	if (!Inventory)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryComponent is null. Cannot stop shooting."));
		return;
	}

	AWeaponBase* CurrentWeapon = Inventory->GetCurrentHeldWeapon();

	if (!CurrentWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("No current weapon equipped. Cannot stop shooting."));
		return;
	}

	CurrentWeapon->StopFire();
}
void AFPSPlayerCharacter::StartAiming()
{
	IsInterpolatingAim = true;
	IsAiming = true;
	
	UE_LOG(LogTemp,Warning, TEXT("[AIMING] Started Aiming Called "))
	
}

void AFPSPlayerCharacter::StopAiming()
{
	IsAiming = false;
	IsInterpolatingAim = true;
	UE_LOG(LogTemp, Warning, TEXT("[AIMING] Stop Aiming Called "))

}

void AFPSPlayerCharacter::EquipPrimaryWeapon()
{
	UInventoryComponent* Inventory = GetInventoryComponent();

	if (!Inventory)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryComponent is null. Cannot equip primary weapon."));
		return;
	}

	Inventory->EquipPrimaryWeapon();
}

void AFPSPlayerCharacter::HandleADSInterpolation()
{
	if (IsAiming)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AIMING] Is Aiming is true"))

		if (PlayerCamera->FieldOfView == ADSFieldOfView)
		{

			IsInterpolatingAim = false;
			UE_LOG(LogTemp, Warning, TEXT("[AIMING] Player field of view is equal to ADS Interpolating Aim set false"))


		}
		else
		{
			PlayerCamera->FieldOfView = FMath::FInterpConstantTo(PlayerCamera->FieldOfView, ADSFieldOfView, GetWorld()->GetDeltaSeconds(), AimInterpSpeed);
			UE_LOG(LogTemp, Warning, TEXT("[AIMING] Interpolating player field of view to ADS field of View"))
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[AIMING] Is Aiming is false")) 
		if (PlayerCamera->FieldOfView == DefaultFieldOfView)
		{
			IsInterpolatingAim = false;

			UE_LOG(LogTemp, Warning, TEXT("[AIMING] Player field of view is equal to default. Interpolating Aim set false"))
		}
		else
		{
			PlayerCamera->FieldOfView = FMath::FInterpConstantTo(PlayerCamera->FieldOfView, DefaultFieldOfView, GetWorld()->GetDeltaSeconds(), AimInterpSpeed);
			UE_LOG(LogTemp, Warning, TEXT("[AIMING] Interpolating player field of view to default field of view"))
		}
	}
}

void AFPSPlayerCharacter::ReloadWeapon()
{
	UInventoryComponent* Inventory = GetInventoryComponent();

	if (!Inventory)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryComponent is null. Cannot reload."));
		return;
	}

	AWeaponBase* CurrentWeapon = Inventory->GetCurrentHeldWeapon();

	if (!CurrentWeapon)
	{
		UE_LOG(LogTemp, Warning, TEXT("No current weapon equipped. Cannot reload."));
		return;
	}

	CurrentWeapon->Reload();
}

void AFPSPlayerCharacter::EquipSecondaryWeapon()
{
	UInventoryComponent* Inventory = GetInventoryComponent();

	if (!Inventory)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryComponent is null. Cannot equip secondary weapon."));
		return;
	}

	Inventory->EquipSecondaryWeapon();

	
}

UInventoryComponent* AFPSPlayerCharacter::GetInventoryComponent() const
{
	if (InventoryComponent)
	{
		return InventoryComponent.Get();
	}

	return FindComponentByClass<UInventoryComponent>();
}

void  AFPSPlayerCharacter::AttachWeaponToCharacter(AWeaponBase* WeaponToAttach)
{
	if (!WeaponToAttach)
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponToAttach is null. Cannot attach weapon to character."));
		return;
	}

	WeaponToAttach->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponToAttach->AttachSocketName);

	WeaponToAttach->SetActorRelativeTransform(
		WeaponToAttach->ThirdPersonGripOffset
	);
}

