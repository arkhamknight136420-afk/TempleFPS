// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathComponent.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UDeathComponent::UDeathComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDeathComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UDeathComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDeathComponent::HandleDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("%s has died."), *GetOwner()->GetName());


}