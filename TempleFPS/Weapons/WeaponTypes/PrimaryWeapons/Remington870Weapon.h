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
	virtual FString GetPromptText_Implementation() override;
	//Firing
	 void FireOnce() override;

	 bool CreatePlayerBulletTrace(FHitResult& OutPlayerHit, FVector& OutAimPoint) override;

	 bool CreateWeaponBulletTrace(const FVector& AimPoint, FHitResult& OutWeaponHit) override;

	 bool CanFire() const override;

	 UFUNCTION(BlueprintCallable)
	  void ResolveBulletHitResults(const TArray<FHitResult>& HitResults);

	  virtual int32 AddAmmo(int32 AdditionalAmmo) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Pellet")
	float PelletSpreadAngle = 5.0f; // Degrees	

	UFUNCTION(BlueprintCallable)
	FRotator CreateRandomSpread();

	
	 void Reload()override;

	  void TryFire() override;

	 void InsertAmmoIntoMagazine()override;

	 virtual bool CanReload() const;

	 void FinishReload() override;

	  void ResetFireCooldown() override;

	 UPROPERTY(BlueprintReadOnly, Category = "State")
	 bool bLoopReload = true;

	 UPROPERTY(BlueprintReadOnly, Category = "State")
	 bool bRoundChambered = true;

	 UPROPERTY(BlueprintReadOnly, Category = "State")
	 bool bIsChamberingRound = false;

private:

	

	TArray<FHitResult> PelletHitResults;
};
