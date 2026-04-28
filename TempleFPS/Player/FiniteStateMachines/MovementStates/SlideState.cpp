#include "SlideState.h"
#include "../../Characters/FPSPlayerCharacter.h"
#include "../../Bralns/FPSBrainComponent.h"

void USlideState::Enter(AFPSPlayerCharacter* Character)
{
	Super::Enter(Character);
}

void USlideState::Update(AFPSPlayerCharacter* Character, float DeltaTime)
{
	Super::Update(Character, DeltaTime);

	if (!Character || !Character->BrainComponent) return;

	Character->HandleDirectionalMovement(Character->BrainComponent->CurrentMoveInput);
}

void USlideState::Exit(AFPSPlayerCharacter* Character)
{
	Super::Exit(Character);
}

const char* USlideState::GetStateName() const
{
	return "SlideState";
}