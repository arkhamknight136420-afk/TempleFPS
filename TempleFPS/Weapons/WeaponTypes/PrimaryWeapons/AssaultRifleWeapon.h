#pragma once

#include "CoreMinimal.h"
#include "PrimaryWeaponBase.h"
#include "AssaultRifleWeapon.generated.h"

UCLASS()
class TEMPLEFPS_API AAssaultRifleWeapon : public APrimaryWeaponBase
{
	GENERATED_BODY()

public:
	virtual FString GetPromptText_Implementation() override;

	virtual void StartFire() override;
	virtual void StopFire() override;

private:
	FTimerHandle AutomaticFireTimerHandle;

	UPROPERTY(VisibleAnywhere, Category = "State")
	bool bIsTriggerHeld = false;
};