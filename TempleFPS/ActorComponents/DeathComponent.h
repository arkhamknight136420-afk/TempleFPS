// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DeathComponent.generated.h"

class ACharacter;
class ABaseAICharacter;
class AAIController;


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TEMPLEFPS_API UDeathComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UDeathComponent();

	UFUNCTION(BlueprintCallable, Category = "Death")
	void HandleDeath();

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

private:
	void HandleControllerDeath(ACharacter* Character);

	void HandleAIControllerDeath(
		ABaseAICharacter* AICharacter,
		AAIController* AIController
	);

	void EnableRagdoll(ACharacter* Character);


	void DropCharacterWeapon(ACharacter* Character);
};