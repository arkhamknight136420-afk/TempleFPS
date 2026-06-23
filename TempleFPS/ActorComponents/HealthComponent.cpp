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
	UE_LOG(LogTemp, Warning, TEXT("ApplyDamage called on owner: %s"),
		GetOwner() ? *GetOwner()->GetName() : TEXT("NULL"));
	if (bIsDead) return;

	CurrentHealth -= InputDamageAmount;

	UE_LOG(LogTemp, Warning, TEXT("%s took %f damage, current health is %f"),
		*GetOwner()->GetName(), InputDamageAmount, CurrentHealth);

	if (ABaseAICharacter* AICharacterRef = Cast<ABaseAICharacter>(GetOwner()))
	{
		UE_LOG(LogTemp, Warning, TEXT("Owner is an AI Character"));

		if (ABaseAIController* AIControllerRef = Cast<ABaseAIController>(AICharacterRef->GetController()))
		{
			AIControllerRef->SetPlayerBlackBoardKey();

			UE_LOG(LogTemp, Warning, TEXT("Set PlayerBlackBoardKey on AI Controller"));
		}
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
			UE_LOG(LogTemp, Error, TEXT("HealthComponent on %s has no reference to DeathComponent"),
				*GetOwner()->GetName());
		}
	}
}
void UHealthComponent::AddHealth(float InputHealthAmount)
{

	CurrentHealth = CurrentHealth + InputHealthAmount;

}