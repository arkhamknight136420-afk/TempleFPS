#include "AssaultRifleWeapon.h"
#include "TimerManager.h"

FString AAssaultRifleWeapon::GetPromptText_Implementation()
{
	return FString(TEXT("Pick Up Assault Rifle (E)"));
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
	IsShooting = false;

	GetWorldTimerManager().ClearTimer(AutomaticFireTimerHandle);
}