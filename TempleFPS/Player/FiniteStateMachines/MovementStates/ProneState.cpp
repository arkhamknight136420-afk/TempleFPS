#include "ProneState.h"
#include "../../Characters/FPSPlayerCharacter.h"
#include "../../Bralns/FPSBrainComponent.h"

void UProneState::Enter(AFPSPlayerCharacter* Character)
{
	Super::Enter(Character);
}

void UProneState::Update(AFPSPlayerCharacter* Character, float DeltaTime)
{
	Super::Update(Character, DeltaTime);

	if (!Character || !Character->BrainComponent) return;

	Character->HandleDirectionalMovement(Character->BrainComponent->CurrentMoveInput);
}

void UProneState::Exit(AFPSPlayerCharacter* Character)
{
	Super::Exit(Character);
}

const char* UProneState::GetStateName() const
{
	return "ProneState";
}