#include "FPSBrainComponent.h"
#include "../Characters/FPSPlayerCharacter.h"
#include "../FiniteStateMachines/MovementStates/BaseMovementState.h"
#include "../FiniteStateMachines/MovementStates/JumpState.h"
#include "../FiniteStateMachines/MovementStates/MantleState.h"
#include "../FiniteStateMachines/MovementStates/SprintState.h"
#include "../FiniteStateMachines/MovementStates/CrouchState.h"
#include "../FiniteStateMachines/MovementStates/ProneState.h"
#include "../FiniteStateMachines/MovementStates/SlideState.h"
#include "../FiniteStateMachines/MovementStates/StandState.h"
#include "../FiniteStateMachines/MovementStates/IdleState.h"
#include "../FiniteStateMachines/MovementStates/WalkState.h"
#include "../FiniteStateMachines/MovementStates/SlowWalkState.h"

DEFINE_LOG_CATEGORY(LogFPSBrain);
DEFINE_LOG_CATEGORY(LogMovementState);


UFPSBrainComponent::UFPSBrainComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFPSBrainComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacterRef = Cast<AFPSPlayerCharacter>(GetOwner());

	if (!PlayerCharacterRef)
	{
		UE_LOG(LogFPSBrain, Error, TEXT("UFPSBrainComponent: Owner is not AFPSPlayerCharacter"));
		return;
	}

	PlayerCharacterRef->BrainComponent = this;

	

	ChangeMovementState(NewObject<UIdleState>(this));

	UE_LOG(LogFPSBrain, Log, TEXT("UFPSBrainComponent: Initializing IdleState"));
}
void UFPSBrainComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!CurrentMovementState)
	{
		UE_LOG(LogTemp, Error, TEXT("CurrentMovementState is NULL"));
		return;
	}

	if (!PlayerCharacterRef)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerCharacterRef is NULL"));
		return;
	}

	

	CurrentMovementState->Update(PlayerCharacterRef, DeltaTime);
}
void UFPSBrainComponent::SetMoveInput(const FVector2D& MoveInput)
{
	CurrentMoveInput = MoveInput;
}

void UFPSBrainComponent::HandleMoveInputChanged(const FVector2D& MoveInput)
{
	

	SetMoveInput(MoveInput);



	if (!CurrentMovementState->AllowsGroundedMoveStateTransitions())
	{
	
	}

	if (MoveInput.IsNearlyZero())
	{
	
		TryIdle();
	}
	else
	{
		TryWalk();
	}
}


bool UFPSBrainComponent::IsCurrentState(TSubclassOf<UBaseMovementState> StateClass) const
{
	if (!CurrentMovementState || !StateClass)
	{
		return false;
	}

	return CurrentMovementState->IsA(StateClass);
}

bool UFPSBrainComponent::TryIdle()
{
	if (IsCurrentState(UIdleState::StaticClass()))
	{
		return false;
	}

	if (CanIdle())
	{
		ChangeMovementState(NewObject<UIdleState>(this));
		return true;
	}
	else
	{
		UE_LOG(LogFPSBrain, Log, TEXT("Can not change to idle state"));
		return false;
	}
}

bool UFPSBrainComponent::CanIdle()
{
	if (PlayerCharacterRef->IsGrounded())
	{
		if (!CurrentMovementState->IsA(UCrouchState::StaticClass()))
		{
			return true;
		}
	}
	return false;
}

bool UFPSBrainComponent::TryWalk()
{
	if (IsCurrentState(UWalkState::StaticClass()))
	{
		return false;
	}

	if (CanWalk())
	{
		ChangeMovementState(NewObject<UWalkState>(this));
		return true;

	}
	else
	{
		UE_LOG(LogFPSBrain, Verbose, TEXT("Can not change to walk state"));
		return false;
	}
}

bool UFPSBrainComponent::CanWalk()
{
	UE_LOG(LogTemp, Warning, TEXT("[BRAIN] CanWalk check - IsGrounded: %d, CurrentState: %s, BufferingSlowWalk: %d"),
		PlayerCharacterRef->IsGrounded(),
		ANSI_TO_TCHAR(CurrentMovementState->GetStateName()),
		BufferingSlowWalk);

	if (PlayerCharacterRef->IsGrounded())
	{
		if (!CurrentMovementState->IsA(UCrouchState::StaticClass()))
		{
			if (!CurrentMovementState->IsA(USlowWalkState::StaticClass()))
			{
				if (!BufferingSlowWalk)
				{
					UE_LOG(LogTemp, Warning, TEXT("[BRAIN] CanWalk: TRUE"));
					return true;
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("[BRAIN] CanWalk: FALSE - BufferingSlowWalk"));
					TrySlowWalk();
					return false;
				}
			}
			else { UE_LOG(LogTemp, Warning, TEXT("[BRAIN] CanWalk: FALSE - In SlowWalkState")); }
		}
		else { UE_LOG(LogTemp, Warning, TEXT("[BRAIN] CanWalk: FALSE - In CrouchState")); }
	}
	else { UE_LOG(LogTemp, Warning, TEXT("[BRAIN] CanWalk: FALSE - Not grounded")); }

	return false;
}
bool UFPSBrainComponent::TrySlowWalk()
{
	if (IsCurrentState(USlowWalkState::StaticClass()))
	{
		return false;
	}

	if (CanSlowWalk())
	{
		ChangeMovementState(NewObject<USlowWalkState>(this));
		return true;

		
	}
	else
	{
		UE_LOG(LogFPSBrain, Log, TEXT("Can not change to Slow Walk state"));
		return false;
	}
}

bool UFPSBrainComponent::CanSlowWalk()
{
	if (PlayerCharacterRef->IsGrounded())
	{
		if (!CurrentMoveInput.IsNearlyZero())
		{
			return true;
		}
	}
	return false;
}

bool UFPSBrainComponent::StopSlowWalk()
{
	if (IsCurrentState(USlowWalkState::StaticClass()))
	{
		if (!CurrentMoveInput.IsNearlyZero())
		{
			if (!IsCurrentState(UWalkState::StaticClass()))
			{
				ChangeMovementState(NewObject<UWalkState>(this));
				return true;
			}
		}
		else
		{
			if (!IsCurrentState(UIdleState::StaticClass()))
			{
				ChangeMovementState(NewObject<UIdleState>(this));
				return true;
			}
		}
		return false;
	}
	return false;
}

bool UFPSBrainComponent::TryJump()
{
	if (IsCurrentState(UJumpState::StaticClass()))
	{
		return false;
	}

	if (CanJump())
	{
		ChangeMovementState(NewObject<UJumpState>(this));
		return true;
	}
	else
	{
		UE_LOG(LogFPSBrain, Log, TEXT("Can not change to jump state"));
		return false;
	}
}

bool UFPSBrainComponent::CanJump()
{
	if (PlayerCharacterRef->IsGrounded())
	{
		if(IsCurrentState(UCrouchState::StaticClass()))
		{
			if (!CurrentMoveInput.IsNearlyZero())
			{
				TryWalkFromCrouch();
				UE_LOG(LogFPSBrain, Log, TEXT("Can not change to jump state from crouch and  theres movement input attempting to change to walk state "));
				return false;
			}
			else
			{
				TryIdleFromCrouch();	
				UE_LOG(LogFPSBrain, Log, TEXT("Can not change to jump state from crouch and  theres no movement input attempting to change to idle state "));
				return false;
			}
		}
		else
		{
			return true;
		}
	}

	return false;
}

void UFPSBrainComponent::HandleJumpLanding()
{
	if(BufferingCrouch)
	{
		BufferingCrouch = false;
		TryCrouch();
	}
	else if (BufferingSlowWalk)
	{
		BufferingSlowWalk = false;
		TrySlowWalk();
	}
	else
	{
		if (CurrentMoveInput.IsNearlyZero())
		{
			TryIdle();
		}
		else
		{
			TryWalk();
		}
	}
}


bool UFPSBrainComponent::TryCrouch()
{
	if (IsCurrentState(UCrouchState::StaticClass()))
	{
		return false;
	}

	if (CanCrouch())
	{
		ChangeMovementState(NewObject<UCrouchState>(this));
		return true;
	}
	else
	{
		UE_LOG(LogFPSBrain, Log, TEXT("Can not change to crouch state"));
		return false;

	}
}

bool UFPSBrainComponent::CanCrouch()
{
	if (PlayerCharacterRef->IsGrounded())
	{
		return true;
	}
	else
	{
		UE_LOG(LogFPSBrain, Log, TEXT("Can not Change to crouch state because we are not grounded"));
		return false;
	}
}

bool UFPSBrainComponent::TryIdleFromCrouch()
{
	// if checks for capsule height etc
	ChangeMovementState(NewObject<UIdleState>(this));
	return true;
}



bool UFPSBrainComponent::TryWalkFromCrouch()
{
	// if checks for capsule height etc
	ChangeMovementState(NewObject<UWalkState>(this));
	return true;
}

bool UFPSBrainComponent::StopCrouch()
{
	if (IsCurrentState(UCrouchState::StaticClass()))
	{

	
		if (!CurrentMoveInput.IsNearlyZero())
		{
			if (!IsCurrentState(UWalkState::StaticClass()))
			{
				ChangeMovementState(NewObject<UWalkState>(this));
				return true;
			}
		}
		else
		{
			if (!IsCurrentState(UIdleState::StaticClass()))
			{
				ChangeMovementState(NewObject<UIdleState>(this));
				return true;
			}
		}
		return false;
	}
	return false;
}


bool UFPSBrainComponent::IsCrouching() const
{
	return IsCurrentState(UCrouchState::StaticClass());
}

bool UFPSBrainComponent::IsSlowWalking() const
{
	return IsCurrentState(USlowWalkState::StaticClass());
}

void UFPSBrainComponent::ChangeMovementState(UBaseMovementState* NewState)
{
	if (!NewState)
	{
		UE_LOG(LogFPSBrain, Warning, TEXT("NewState is not valid"));
		return;
	
	}

	if (CurrentMovementState)
	{
		CurrentMovementState->Exit(PlayerCharacterRef);
		PreviousMovementState = CurrentMovementState;
	}
	else
	{
		UE_LOG(LogFPSBrain, Warning, TEXT("could not exit current movement state because current movement state is not valid This warning should occur at the beginning of play as there was no previous movement state."));
	
	}

	CurrentMovementState = NewState;
	CurrentMovementState->Enter(PlayerCharacterRef);

	UE_LOG(LogMovementState, Log, TEXT("[MOVEMENT_STATE] Current Movement State Is: %s"), ANSI_TO_TCHAR(CurrentMovementState->GetStateName()));

	return;
}

FName UFPSBrainComponent::GetCurrentMovementStateName() const
{
	if (!CurrentMovementState)
	{
		return TEXT("None");

	}
	else
	{
		return CurrentMovementState->GetStateName();

	}
}