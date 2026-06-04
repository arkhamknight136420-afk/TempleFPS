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
			UE_LOG(LogTemp, Warning, TEXT("[WEAPON] Cannot fire. Magazine empty."));
		}

		return;
	}

	AmmoInMagazine--;

	bCanFire = false;

	GetWorldTimerManager().SetTimer(
		FireCooldownTimerHandle,
		this,
		&ARemington870Weapon::ResetFireCooldown,
		FireRate,
		false
	);

	UE_LOG(LogTemp, Warning, TEXT("[WEAPON] Fired: %s | Ammo: %d / %d"),
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
			if (UHealthComponent* HealthComponent = HitResult.GetActor()->GetComponentByClass<UHealthComponent>())
			{
				HealthComponent->ApplyDamage(Damage);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("[WEAPON HIT] No HealthComponent found on %s"), *HitResult.GetActor()->GetName());
			}
	}
}