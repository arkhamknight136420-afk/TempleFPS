#include "FPSPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../FiniteStateMachines/MovementStates/BaseMovementState.h"
#include "../Bralns/FPSBrainComponent.h"
#include "Components/CapsuleComponent.h"

AFPSPlayerCharacter::AFPSPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;

	BrainComponent = CreateDefaultSubobject<UFPSBrainComponent>(TEXT("BrainComponent"));

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
	PlayerCamera->SetupAttachment(GetCapsuleComponent());
	PlayerCamera->bUsePawnControlRotation = true;

	InteractionCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Interaction Capsule"));
	InteractionCapsule->SetupAttachment(GetCapsuleComponent());
}

void AFPSPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AFPSPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

void AFPSPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
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