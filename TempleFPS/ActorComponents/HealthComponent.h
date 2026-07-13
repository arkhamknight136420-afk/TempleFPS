// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class UDeathComponent;
class ABaseAICharacter;
class ABaseAIController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnHealthChangedSignature,
	float, CurrentHealth,
	float, MaxHealth,
	float, HealthDelta
);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEMPLEFPS_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	UDeathComponent* DeathComponent;

	UPROPERTY(BlueprintAssignable, Category = "Health|Events")
	FOnHealthChangedSignature OnHealthChanged;



	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category = "Health")
	void ApplyDamage(float InputDamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Health")
	void AddHealth(float InputHealthAmount);

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetCurrentHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintPure, Category = "Health")
	float GetMaxHealth() const { return MaxHealth; }

	UFUNCTION(BlueprintPure, Category = "Health")
		float GetHealthPercent() const
	{
		return MaxHealth > 0.f ? CurrentHealth / MaxHealth : 0.f;
	}

	UFUNCTION(BlueprintCallable, Category = "Health")
	bool IsDead() const { return bIsDead; }

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category = "Health")
	float CurrentHealth = 100;

	UPROPERTY(EditDefaultsOnly, Category = "Health")
	float MaxHealth = 100.f;

	UPROPERTY(BlueprintReadOnly)
	bool bIsDead = false;

private:



};