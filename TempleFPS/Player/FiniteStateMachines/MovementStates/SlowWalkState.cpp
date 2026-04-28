#include "SlowWalkState.h"
#include "../../Characters/FPSPlayerCharacter.h"
#include "../../Bralns/FPSBrainComponent.h"

void USlowWalkState::Enter(AFPSPlayerCharacter* Character)
{
	Super::Enter(Character);

	Character->StartSlowWalkMovement();
}

void USlowWalkState::Update(AFPSPlayerCharacter* Character, float DeltaTime)
{
	Super::Update(Character, DeltaTime);

	if (!Character || !Character->BrainComponent) return;

	Character->HandleDirectionalMovement(Character->BrainComponent->CurrentMoveInput);
}

void USlowWalkState::Exit(AFPSPlayerCharacter* Character)
{
	Super::Exit(Character);
}

const char* USlowWalkState::GetStateName() const
{
	return "SlowWalkState";
}