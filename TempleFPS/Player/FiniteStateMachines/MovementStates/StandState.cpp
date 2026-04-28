#include "StandState.h"
#include "../../Characters/FPSPlayerCharacter.h"
#include "../../Bralns/FPSBrainComponent.h"

void UStandState::Enter(AFPSPlayerCharacter* Character)
{
	Super::Enter(Character);
}

void UStandState::Update(AFPSPlayerCharacter* Character, float DeltaTime)
{
	Super::Update(Character, DeltaTime);
}

void UStandState::Exit(AFPSPlayerCharacter* Character)
{
	Super::Exit(Character);
}

const char* UStandState::GetStateName() const
{
	return "StandState";
}