// Fill out your copyright notice in the Description page of Project Settings.


#include "Remington870Weapon.h"
#include "../../../Player/Characters/FPSPlayerCharacter.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "../../../Characters/BaseCharacter.h"
#include"../../../ActorComponents/HealthComponent.h"

void ARemington870Weapon::FireOnce()
{
	if (!CanFire())
	{
		if (AmmoInMagazine <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("[REMINGTON] Cannot fire. Magazine empty."));
		}

		return;
	}
	GetWorldTimerManager().ClearTimer(ReloadFinishedTimerHandle);
	bIsChamberingRound = false;
	AmmoInMagazine--;

	bRoundChambered = false;
	bCanFire = false;
	IsShooting = true;
	PlayFireSFX();
	PlayMuzzleFlashEffect();

	GetWorldTimerManager().SetTimer(
		FireCooldownTimerHandle,
		this,
		&ARemington870Weapon::ResetFireCooldown,
		FireRate,
		false
	);

	UE_LOG(LogTemp, Warning, TEXT("[REMINGTON] Fired: %s | Ammo: %d / %d"),
		*GetName(),
		AmmoInMagazine,
		AmmoInReserve
	);
	PelletHitResults.Empty();

	for (int i = 0; i < 6; i++)
	{
		FHitResult PlayerHit;
		FVector AimPoint;

		const bool bPlayerTraceHit = CreatePlayerBulletTrace(PlayerHit, AimPoint);

		FHitResult WeaponHit;
		const bool bWeaponTraceHit = CreateWeaponBulletTrace(AimPoint, WeaponHit);

		if (bWeaponTraceHit)
		{
			PelletHitResults.Add(WeaponHit);
		}
		else if (bPlayerTraceHit)
		{
			PelletHitResults.Add(PlayerHit);
		}
	}

	ResolveBulletHitResults(PelletHitResults);


	
}

void ARemington870Weapon::ResetFireCooldown()
{
	if (AmmoInMagazine <= 0)
	{
		bRoundChambered = false;
		UE_LOG(LogTemp, Warning, TEXT("[REMINGTON] Cooldown finished: no ammo left to chamber"));
	}
	else
	{
		bRoundChambered = true;
		UE_LOG(LogTemp, Warning, TEXT("[REMINGTON] Auto-chambered next round"));
	}

	bCanFire = true;
}

void ARemington870Weapon::TryFire()
{
	if (AmmoInMagazine <= 0) // if theres no ammo in the gun at all
	{
		UE_LOG(LogTemp, Warning, TEXT("[REMINGTON] No ammo left in the magazine at all cannot fire   "));
		return;
	}
	if (!bRoundChambered)
	{
		if (bIsChamberingRound)
		{
			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("[REMINGTON] Chambering A round"));

		bIsChamberingRound = true;
		bIsReloading = false;

		GetWorldTimerManager().ClearTimer(AmmoInsertTimerHandle);

		GetWorldTimerManager().SetTimer(
			ReloadFinishedTimerHandle,
			this,
			&ARemington870Weapon::FinishReload,
			ReloadDuration,
			false
		);

		return;
	}

	FireOnce();
}

bool ARemington870Weapon::CanFire() const
{
	if (!bRoundChambered)
	{
		UE_LOG(LogTemp, Warning, TEXT("[REMINGTON] Cannot Fire No Round Chambered   "));
		return false;
	}
	if (!bCanFire)
	{
		return false;
	}

	if (AmmoInMagazine <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[REMINGTON]  Cannot Fire No More Ammo In Magazine "));
		return false;
	}

	return true;
}

bool ARemington870Weapon::CreatePlayerBulletTrace(FHitResult& OutPlayerHit, FVector& OutAimPoint)
{
	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(GetOwner()); // HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE 

	if (!BaseCharacter) // HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE 
	{
		UE_LOG(LogTemp, Error, TEXT("[PLAYER TRACE] Failed: Weapon has no valid ABaseCharacter owner."));
		return false;
	}

	if (!BaseCharacter->EyesLocation) // HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE 
	{
		UE_LOG(LogTemp, Error, TEXT("[PLAYER TRACE] Failed: PlayerCamera is null."));
		return false;
	}

	const FVector StartLocation = BaseCharacter->EyesLocation->GetComponentLocation(); // HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE 
	
	FVector ShotDirection = BaseCharacter->EyesLocation->GetForwardVector(); // HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE 

	FRotator SpreadRotation = CreateRandomSpread();

	ShotDirection = SpreadRotation.RotateVector(ShotDirection);

	const FVector EndLocation = StartLocation + ShotDirection * MaxTravelDistance;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(BaseCharacter); // HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE 
	QueryParams.bTraceComplex = bBulletTraceComplex;

	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		OutPlayerHit,
		StartLocation,
		EndLocation,
		BulletTraceChannel,
		QueryParams
	);

	OutAimPoint = bHit ? OutPlayerHit.ImpactPoint : EndLocation;

	if (bHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("[PLAYER TRACE HIT] Actor: %s | Component: %s | Impact: %s | Distance: %f | Channel: %d"),
			OutPlayerHit.GetActor() ? *OutPlayerHit.GetActor()->GetName() : TEXT("None"),
			OutPlayerHit.GetComponent() ? *OutPlayerHit.GetComponent()->GetName() : TEXT("None"),
			*OutPlayerHit.ImpactPoint.ToString(),
			OutPlayerHit.Distance,
			(int32)BulletTraceChannel.GetValue()
		);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[PLAYER TRACE NO HIT] Start: %s | End: %s | Channel: %d"),
			*StartLocation.ToString(),
			*EndLocation.ToString(),
			(int32)BulletTraceChannel.GetValue()
		);
	}

	const FColor TraceColor = bHit ? FColor::Green : FColor::Red;

	DrawDebugLine(GetWorld(), StartLocation, OutAimPoint, TraceColor, false, 2.0f, 0, 2.0f);

	if (bHit)
	{
		DrawDebugSphere(GetWorld(), OutPlayerHit.ImpactPoint, 8.0f, 12, FColor::Green, false, 2.0f);
	}

	return bHit;
}

bool ARemington870Weapon::CreateWeaponBulletTrace(const FVector& AimPoint, FHitResult& OutWeaponHit)
{
	if (!MuzzleLocation)
	{
		UE_LOG(LogTemp, Error, TEXT("[WEAPON TRACE] Failed: MuzzleLocation is null."));
		return false;
	}

	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(GetOwner()); // HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE 

	const FVector StartLocation = MuzzleLocation->GetComponentLocation();
	const FVector Direction = (AimPoint - StartLocation).GetSafeNormal();
	const FVector EndLocation = AimPoint + Direction * MuzzleTraceOvershootDistance;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = bBulletTraceComplex;

	if (BaseCharacter) // HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE 
	{
		QueryParams.AddIgnoredActor(BaseCharacter);
	}

	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		OutWeaponHit,
		StartLocation,
		EndLocation,
		BulletTraceChannel,
		QueryParams
	);

	if (bHit)
	{
		UE_LOG(LogTemp, Warning, TEXT("[WEAPON TRACE HIT] Actor: %s | Component: %s | Impact: %s | Distance: %f | Channel: %d"),
			OutWeaponHit.GetActor() ? *OutWeaponHit.GetActor()->GetName() : TEXT("None"),
			OutWeaponHit.GetComponent() ? *OutWeaponHit.GetComponent()->GetName() : TEXT("None"),
			*OutWeaponHit.ImpactPoint.ToString(),
			OutWeaponHit.Distance,
			(int32)BulletTraceChannel.GetValue()
		);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[WEAPON TRACE NO HIT] Start: %s | End: %s | Channel: %d"),
			*StartLocation.ToString(),
			*EndLocation.ToString(),
			(int32)BulletTraceChannel.GetValue()
		);
	}

	const FVector DebugEnd = bHit ? OutWeaponHit.ImpactPoint : EndLocation;
	const FColor TraceColor = bHit ? FColor::Cyan : FColor::Orange;

	DrawDebugLine(GetWorld(), StartLocation, DebugEnd, TraceColor, false, 2.0f, 0, 2.0f);

	if (bHit)
	{
		DrawDebugSphere(GetWorld(), OutWeaponHit.ImpactPoint, 8.0f, 12, FColor::Cyan, false, 2.0f);
	}

	return bHit;
}

FRotator ARemington870Weapon::CreateRandomSpread()
{
	return FRotator(
		FMath::FRandRange(-PelletSpreadAngle, PelletSpreadAngle),
		FMath::FRandRange(-PelletSpreadAngle, PelletSpreadAngle),
		0.f
	);

}

void ARemington870Weapon::ResolveBulletHitResults(const TArray<FHitResult>& HitResults)
{
	for (const FHitResult& HitResult : HitResults)
	{
			DrawDebugSphere(GetWorld(), HitResult.ImpactPoint, 20.f, 16, FColor::Yellow, false, 2.f);
			if (USkeletalMeshComponent* CharacterMesh = Cast<USkeletalMeshComponent>(HitResult.GetComponent())) // if what we hit was a skeletal mesh
			{
				if (UHealthComponent* HealthComponent = HitResult.GetActor()->GetComponentByClass<UHealthComponent>()) // if the Hit actor has a health component
				{
					if (WasHeadShot(HitResult))
					{
						UE_LOG(LogTemp, Log, TEXT("[WEAPON BASE] Head Shot"))
							float HeadShotDamage = Damage * HeadShotDamageMultiplier;
						HealthComponent->ApplyDamage(HeadShotDamage);
					}
					else
					{
						UE_LOG(LogTemp, Log, TEXT("[WEAPON BASE] Body Shot"))
							HealthComponent->ApplyDamage(Damage);

					}
				}
			}
	}
}

void ARemington870Weapon::Reload()
{
	if (!CanReload())
	{
		UE_LOG(LogTemp, Warning, TEXT("[REMINGTON] Cannot reload."));
		return;
	}

	ReloadDuration = FMath::Max(ReloadDuration, 0.01f);

	AmmoInsertNormalizedTime = FMath::Clamp(
		AmmoInsertNormalizedTime,
		0.0f,
		1.0f
	);

	const float ActualAmmoInsertTime =
		ReloadDuration * AmmoInsertNormalizedTime;

	bIsReloading = true;

	

	UE_LOG(LogTemp, Warning,
		TEXT("[REMINGTON] Reload started: %s | ReloadDuration: %.2f | AmmoInsertTime: %.2f"),
		*GetName(),
		ReloadDuration,
		ActualAmmoInsertTime
	);

	GetWorldTimerManager().SetTimer(
		AmmoInsertTimerHandle,
		this,
		&ARemington870Weapon::InsertAmmoIntoMagazine,
		ActualAmmoInsertTime,
		bLoopReload
	);


	
}

void ARemington870Weapon::InsertAmmoIntoMagazine()
{
	const int32 AmmoNeeded = 1;
	const int32 AmmoToInsert = FMath::Min(AmmoNeeded, AmmoInReserve);

	AmmoInMagazine += AmmoToInsert;
	AmmoInReserve -= AmmoToInsert;

	UE_LOG(LogTemp, Warning, TEXT("[REMINGTON] Ammo inserted: %s | Ammo: %d / %d"),
		*GetName(),
		AmmoInMagazine,
		AmmoInReserve
	);

	if (AmmoInMagazine == MagazineSize)
	{

		GetWorldTimerManager().ClearTimer(AmmoInsertTimerHandle);
		
		//call reload pump animation 

		GetWorldTimerManager().SetTimer(
			ReloadFinishedTimerHandle,
			this,
			&ARemington870Weapon::FinishReload,
			ReloadDuration,
			false
		);
		UE_LOG(LogTemp, Warning, TEXT("[REMINGTON] Gun Is fully Loaded Chambering round Now  "));
	}

	

}

void ARemington870Weapon::FinishReload()
{
	UE_LOG(LogTemp, Warning, TEXT("[REMINGTON] Round Chambered  "));
	bIsReloading = false;
	bRoundChambered = true;
	bIsChamberingRound = false;



}

bool ARemington870Weapon::CanReload() const
{
	if (bIsChamberingRound)
	{
		return false;
	}

	if (AmmoInMagazine >= MagazineSize)
	{
		return false;
	}

	if (AmmoInReserve <= 0)
	{
		return false;
	}

	if (bIsReloading)
	{
		return false;
	}

	return true;
}