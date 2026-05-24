// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "DeathComponent.h"

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
	if (bIsDead)
	{
		return;
	}	
	if (CurrentHealth <= 0.f)
	{
		if (DeathComponent)
		{
			DeathComponent->HandleDeath();
			bIsDead = true;
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("HealthComponent on %s has no reference to DeathComponent"), *GetOwner()->GetName());
		}
	}
	else
	{
		CurrentHealth = CurrentHealth - InputDamageAmount;
		UE_LOG(LogTemp, Warning, TEXT("%s took %f damage, current health is %f"), *GetOwner()->GetName(), InputDamageAmount, CurrentHealth);
	}
}

void UHealthComponent::AddHealth(float InputHealthAmount)
{

	CurrentHealth = CurrentHealth + InputHealthAmount;

}