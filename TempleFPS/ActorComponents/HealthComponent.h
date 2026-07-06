// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class UDeathComponent;
class ABaseAICharacter;
class ABaseAIController;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEMPLEFPS_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	UDeathComponent* DeathComponent;

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void ApplyDamage(float InputDamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float InputHealthAmount);

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintCallable, Category = "Health")
	bool IsDead() const { return bIsDead; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Health")
	float CurrentHealth;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth = 100.f;

	

private:

	bool bIsDead = false;


};