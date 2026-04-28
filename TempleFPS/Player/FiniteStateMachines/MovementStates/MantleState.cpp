#include "MantleState.h"
#include "../../Characters/FPSPlayerCharacter.h"
#include "../../Bralns/FPSBrainComponent.h"

void UMantleState::Enter(AFPSPlayerCharacter* Character)
{
	Super::Enter(Character);
}

void UMantleState::Update(AFPSPlayerCharacter* Character, float DeltaTime)
{
	Super::Update(Character, DeltaTime);
}

void UMantleState::Exit(AFPSPlayerCharacter* Character)
{
	Super::Exit(Character);
}

const char* UMantleState::GetStateName() const
{
	return "MantleState";
}