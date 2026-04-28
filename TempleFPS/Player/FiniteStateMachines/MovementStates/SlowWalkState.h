#pragma once

#include "CoreMinimal.h"
#include "BaseMovementState.h"
#include "SlowWalkState.generated.h"

UCLASS()
class TEMPLEFPS_API USlowWalkState : public UBaseMovementState
{
	GENERATED_BODY()

public:
	virtual void Enter(AFPSPlayerCharacter* Character) override;
	virtual void Update(AFPSPlayerCharacter* Character, float DeltaTime) override;
	virtual void Exit(AFPSPlayerCharacter* Character) override;
	virtual const char* GetStateName() const override;
};