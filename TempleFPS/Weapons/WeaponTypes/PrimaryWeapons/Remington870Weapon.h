#pragma once

#include "CoreMinimal.h"
#include "PrimaryWeaponBase.h"
#include "Remington870Weapon.generated.h"

UCLASS()
class TEMPLEFPS_API ARemington870Weapon
	: public APrimaryWeaponBase
{
	GENERATED_BODY()

public:

	virtual FString GetPromptText_Implementation() override;

	virtual void FireOnce() override;

	virtual bool CreatePlayerBulletTrace(
		FHitResult& OutPlayerHit,
		FVector& OutAimPoint
	) override;

	virtual bool CreateWeaponBulletTrace(
		const FVector& AimPoint,
		FHitResult& OutWeaponHit
	) override;

	virtual bool CanFire() const override;

	UFUNCTION(BlueprintCallable)
	void ResolveBulletHitResults(
		const TArray<FHitResult>& HitResults
	);

	virtual int32 AddAmmo(int32 AdditionalAmmo) override;

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Pellet")
	float PelletSpreadAngle = 5.0f;

	UFUNCTION(BlueprintCallable)
	FRotator CreateRandomSpread();

	virtual void Reload() override;

	virtual void TryFire() override;

	virtual void InsertAmmoIntoMagazine() override;

	virtual bool CanReload() const override;

	virtual void FinishReload() override;

	virtual void ResetFireCooldown() override;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool bLoopReload = true;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool bRoundChambered = true;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool bIsChamberingRound = false;

private:

	TArray<FHitResult> PelletHitResults;
};