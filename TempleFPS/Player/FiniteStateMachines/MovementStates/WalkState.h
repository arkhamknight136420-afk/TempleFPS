// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseMovementState.h"
#include "WalkState.generated.h"

UCLASS()
class TEMPLEFPS_API UWalkState : public UBaseMovementState
{
	GENERATED_BODY()

public:
	virtual void Enter(AFPSPlayerCharacter* Character) override;
	virtual void Update(AFPSPlayerCharacter* Character, float DeltaTime) override;
	virtual void Exit(AFPSPlayerCharacter* Character) override;
	virtual const char* GetStateName() const override;
};