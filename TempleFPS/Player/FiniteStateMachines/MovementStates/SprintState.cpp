#include "SprintState.h"
#include "../../Characters/FPSPlayerCharacter.h"
#include "../../Bralns/FPSBrainComponent.h"

void USprintState::Enter(AFPSPlayerCharacter* Character)
{
	Super::Enter(Character);
}

void USprintState::Update(AFPSPlayerCharacter* Character, float DeltaTime)
{
	Super::Update(Character, DeltaTime);

	if (!Character || !Character->BrainComponent) return;

	Character->HandleDirectionalMovement(Character->BrainComponent->CurrentMoveInput);
}

void USprintState::Exit(AFPSPlayerCharacter* Character)
{
	Super::Exit(Character);
}

const char* USprintState::GetStateName() const
{
	return "SprintState";
}