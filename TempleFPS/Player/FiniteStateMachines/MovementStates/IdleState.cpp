#include "IdleState.h"
#include "../../Characters/FPSPlayerCharacter.h"
#include "../../Bralns/FPSBrainComponent.h"


void UIdleState::Enter(AFPSPlayerCharacter* Character)
{
	Super::Enter(Character);

}

void UIdleState::Update(AFPSPlayerCharacter* Character, float DeltaTime)
{
	Super::Update(Character, DeltaTime);
}

void UIdleState::Exit(AFPSPlayerCharacter* Character)
{
	Super::Exit(Character);
}

const char* UIdleState::GetStateName() const
{
	return "Idle State";
}