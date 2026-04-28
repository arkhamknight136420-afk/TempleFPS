#include "CrouchState.h"
#include "../../Characters/FPSPlayerCharacter.h"
#include "../../Bralns/FPSBrainComponent.h"


void UCrouchState::Enter(AFPSPlayerCharacter* Character)
{
	Super::Enter(Character);
	Character->StartCrouchMovement();
}

void UCrouchState::Update(AFPSPlayerCharacter* Character, float DeltaTime)
{
	Super::Update(Character, DeltaTime);

	if (!Character || !Character->BrainComponent) return;

	Character->HandleDirectionalMovement(Character->BrainComponent->CurrentMoveInput);
}

void UCrouchState::Exit(AFPSPlayerCharacter* Character)
{
	Super::Exit(Character);
	Character->StopCrouchMovement();
}

const char* UCrouchState::GetStateName() const
{
	return "CrouchState";
}