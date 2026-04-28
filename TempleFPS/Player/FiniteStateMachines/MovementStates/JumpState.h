#pragma once

#include "CoreMinimal.h"
#include "BaseMovementState.h"
#include "JumpState.generated.h"

class AFPSPlayerCharacter;

UCLASS()
class TEMPLEFPS_API UJumpState : public UBaseMovementState
{
	GENERATED_BODY()

public:
	virtual void Enter(AFPSPlayerCharacter* Character) override;
	virtual void Update(AFPSPlayerCharacter* Character, float DeltaTime) override;
	virtual void Exit(AFPSPlayerCharacter* Character) override;

	virtual bool AllowsGroundedMoveStateTransitions() const override;

	virtual const char* GetStateName() const override;

private:
	bool bHasStartedFalling = false;
};