// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseMovementState.generated.h"

class AFPSPlayerCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TEMPLEFPS_API UBaseMovementState : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UBaseMovementState();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

	virtual void Enter(AFPSPlayerCharacter* Character);
	virtual void Update(AFPSPlayerCharacter* Character, float DeltaTime);
	virtual void Exit(AFPSPlayerCharacter* Character);

	virtual bool AllowsGroundedMoveStateTransitions() const;

	virtual const char* GetStateName() const;

};