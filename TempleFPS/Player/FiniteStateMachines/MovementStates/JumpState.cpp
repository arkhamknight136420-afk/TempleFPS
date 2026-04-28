#include "JumpState.h"
#include "../../Characters/FPSPlayerCharacter.h"
#include "../../Bralns/FPSBrainComponent.h"
#include "../../FiniteStateMachines/MovementStates/IdleState.h"
#include "../../FiniteStateMachines/MovementStates/WalkState.h"
#include "GameFramework/CharacterMovementComponent.h"

void UJumpState::Enter(AFPSPlayerCharacter* Character)
{
	Super::Enter(Character);

	if (!Character)
	{
		return;
	}

	bHasStartedFalling = false;
	Character->StartJumpMovement();
}

void UJumpState::Update(AFPSPlayerCharacter* Character, float DeltaTime)
{
	Super::Update(Character, DeltaTime);

	if (!Character || !Character->BrainComponent)
	{
		return;
	}

	Character->HandleDirectionalMovement(Character->BrainComponent->CurrentMoveInput);

	UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement();

	if (!MovementComponent)
	{
		return;
	}

	// Do not allow landing logic until we have actually left the ground.
	if (!bHasStartedFalling)
	{
		if (MovementComponent->IsFalling())
		{
			bHasStartedFalling = true;
			UE_LOG(LogTemp, Log, TEXT("JumpState: Character has started falling"));
		}

		return;
	}

	// Once we have actually been airborne, then we can check for landing.
	if (!MovementComponent->IsFalling())
	{
		Character->BrainComponent->HandleJumpLanding();
	}
}

void UJumpState::Exit(AFPSPlayerCharacter* Character)
{
	Super::Exit(Character);
}

bool UJumpState::AllowsGroundedMoveStateTransitions() const
{
	return false;
}


const char* UJumpState::GetStateName() const
{
	return "JumpState";
}