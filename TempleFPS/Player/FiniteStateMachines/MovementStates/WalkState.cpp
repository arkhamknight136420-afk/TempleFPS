#include "WalkState.h"
#include "../../Characters/FPSPlayerCharacter.h"
#include "../../Bralns/FPSBrainComponent.h"

void UWalkState::Enter(AFPSPlayerCharacter* Character)
{
	Super::Enter(Character);

	Character->StartWalkMovement();
}

void UWalkState::Update(AFPSPlayerCharacter* Character, float DeltaTime)
{
	Super::Update(Character, DeltaTime);

	if (!Character || !Character->BrainComponent) return;

	Character->HandleDirectionalMovement(Character->BrainComponent->CurrentMoveInput);


}

void UWalkState::Exit(AFPSPlayerCharacter* Character)
{
	Super::Exit(Character);
}

const char* UWalkState::GetStateName() const
{
	return "WalkState";
}