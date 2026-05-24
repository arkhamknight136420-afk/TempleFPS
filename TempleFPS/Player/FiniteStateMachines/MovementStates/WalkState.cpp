#include "WalkState.h"
#include "../../Characters/FPSPlayerCharacter.h"
#include "../../Bralns/FPSBrainComponent.h"

void UWalkState::Enter(AFPSPlayerCharacter* Character)
{
    UE_LOG(LogTemp, Warning, TEXT("[WALKSTATE] Enter called"));
    Super::Enter(Character);
    Character->StartWalkMovement();
}

void UWalkState::Update(AFPSPlayerCharacter* Character, float DeltaTime)
{
	Super::Update(Character, DeltaTime);

	if (!Character)
	{
		
		return;
	}

	UFPSBrainComponent* Brain = Character->BrainComponent;

	if (!Brain)
	{
		Brain = Character->FindComponentByClass<UFPSBrainComponent>();

		if (Brain)
		{
			Character->BrainComponent = Brain;
			
		}
	}

	if (!Brain)
	{
	
		return;
	}



	Character->HandleDirectionalMovement(Brain->CurrentMoveInput);
}

void UWalkState::Exit(AFPSPlayerCharacter* Character)
{
	Super::Exit(Character);
}

const char* UWalkState::GetStateName() const
{
	return "WalkState";
}