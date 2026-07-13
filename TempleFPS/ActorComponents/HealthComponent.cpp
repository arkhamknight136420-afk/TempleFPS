// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "DeathComponent.h"
#include "../AI/Characters/BaseAICharacter.h"
#include "../AI/Controllers/BaseAIController.h"


// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


	
}


// Called when the game starts
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentHealth = MaxHealth;

	DeathComponent = GetOwner()->FindComponentByClass<UDeathComponent>();

	if (!DeathComponent)
	{
		UE_LOG(LogTemp, Error, TEXT("HealthComponent could not find DeathComponent on %s"), *GetOwner()->GetName());
	}
	
}


// Called every frame
void UHealthComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}



void UHealthComponent::ApplyDamage(float InputDamageAmount)
{
	if (bIsDead || InputDamageAmount <= 0.f)
	{
		return;
	}

	const float PreviousHealth = CurrentHealth;

	CurrentHealth = FMath::Clamp(
		CurrentHealth - InputDamageAmount,
		0.f,
		MaxHealth
	);

	const float HealthDelta = CurrentHealth - PreviousHealth;
	const bool bJustDied = CurrentHealth <= 0.f;

	if (bJustDied)
	{
		bIsDead = true;
	}

	OnHealthChanged.Broadcast(
		CurrentHealth,
		MaxHealth,
		HealthDelta
	);

	// Keep your AI reaction logic here.

	if (bJustDied)
	{
		if (DeathComponent)
		{
			DeathComponent->HandleDeath();
		}
		else
		{
			UE_LOG(
				LogTemp,
				Error,
				TEXT("HealthComponent on %s has no DeathComponent"),
				*GetOwner()->GetName()
			);
		}
	}
}

void UHealthComponent::AddHealth(float InputHealthAmount)
{
	if (bIsDead || InputHealthAmount <= 0.f)
	{
		return;
	}

	const float PreviousHealth = CurrentHealth;

	CurrentHealth = FMath::Clamp(
		CurrentHealth + InputHealthAmount,
		0.f,
		MaxHealth
	);

	const float HealthDelta = CurrentHealth - PreviousHealth;

	if (!FMath::IsNearlyZero(HealthDelta))
	{
		OnHealthChanged.Broadcast(
			CurrentHealth,
			MaxHealth,
			HealthDelta
		);
	}
}