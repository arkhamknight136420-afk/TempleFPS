// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseMovementState.h"
#include "../../Bralns/FPSBrainComponent.h"
#include "../../Characters/FPSPlayerCharacter.h"



// Sets default values for this component's properties
UBaseMovementState::UBaseMovementState()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UBaseMovementState::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void UBaseMovementState::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UBaseMovementState::Enter(AFPSPlayerCharacter* Character)
{
	
}

void UBaseMovementState::Update(AFPSPlayerCharacter* Character, float DeltaTime)
{

}

void UBaseMovementState::Exit(AFPSPlayerCharacter* Character)
{
}

bool UBaseMovementState::AllowsGroundedMoveStateTransitions() const
{
	return true;
}


const char* UBaseMovementState::GetStateName() const
{
	return "BaseState";
}