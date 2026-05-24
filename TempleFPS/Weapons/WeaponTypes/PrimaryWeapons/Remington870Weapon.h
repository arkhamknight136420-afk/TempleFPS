// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PrimaryWeaponBase.h"
#include "Remington870Weapon.generated.h"


/**
 * 
 */

class UHealthComponent;

UCLASS()
class TEMPLEFPS_API ARemington870Weapon : public APrimaryWeaponBase
{
	GENERATED_BODY()
	

public:

	 void FireOnce() override;

	 bool CreatePlayerBulletTrace(FHitResult& OutPlayerHit, FVector& OutAimPoint) override;

	 bool CreateWeaponBulletTrace(const FVector& AimPoint, FHitResult& OutWeaponHit) override;


	 UFUNCTION(BlueprintCallable)
	  void ResolveBulletHitResults(const TArray<FHitResult>& HitResults);

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Pellet")
	float PelletSpreadAngle = 5.0f; // Degrees	

	UFUNCTION(BlueprintCallable)
	FRotator CreateRandomSpread();

private:

	

	TArray<FHitResult> PelletHitResults;
};
