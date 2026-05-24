#include "AssaultRifleWeapon.h"
#include "TimerManager.h"

FString AAssaultRifleWeapon::GetPromptText_Implementation()
{
	return FString(TEXT("Press E to pick up Assault Rifle"));
}

void AAssaultRifleWeapon::StartFire()
{
	if (bIsTriggerHeld)
	{
		return;
	}

	bIsTriggerHeld = true;

	TryFire();

	GetWorldTimerManager().SetTimer(
		AutomaticFireTimerHandle,
		this,
		&AAssaultRifleWeapon::TryFire,
		FireRate,
		true
	);
}

void AAssaultRifleWeapon::StopFire()
{
	bIsTriggerHeld = false;

	GetWorldTimerManager().ClearTimer(AutomaticFireTimerHandle);
}