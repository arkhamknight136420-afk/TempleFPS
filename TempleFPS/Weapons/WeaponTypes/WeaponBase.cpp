#include "WeaponBase.h"
#include "../../Inventory/Components/InventoryComponent.h"
#include "../../Player/Characters/FPSPlayerCharacter.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "../../ActorComponents/HealthComponent.h"

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	MuzzleLocation->SetupAttachment(WeaponMesh);

	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	WeaponMesh->SetGenerateOverlapEvents(true);
	WeaponMesh->SetCollisionObjectType(ECC_GameTraceChannel1);
	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);


}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	AmmoInMagazine = FMath::Clamp(AmmoInMagazine, 0, MagazineSize);
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWeaponBase::Interact_Implementation(AActor* Interactor)
{
	UE_LOG(LogTemp, Warning, TEXT("Weapon interacted with"));

	if (AFPSPlayerCharacter* PlayerCharacter = Cast<AFPSPlayerCharacter>(Interactor))
	{
		if (UInventoryComponent* InventoryComponent = PlayerCharacter->FindComponentByClass<UInventoryComponent>())
		{
			InventoryComponent->PickUpWeapon(GetClass());
			Destroy();
		}
	}
}

FString AWeaponBase::GetPromptText_Implementation()
{
	return TEXT("Pick Up Weapon");
}

UStaticMesh* AWeaponBase::GetWeaponStaticMesh() const
{
	return WeaponMesh ? WeaponMesh->GetStaticMesh() : nullptr;
}

void AWeaponBase::TryFire()
{
	FireOnce();
}

void AWeaponBase::StartFire()
{
	TryFire();
}

void AWeaponBase::StopFire()
{
}

bool AWeaponBase::CanFire() const
{
	if (bIsReloading)
	{
		return false;
	}

	if (!bCanFire)
	{
		return false;
	}

	if (AmmoInMagazine <= 0)
	{
		return false;
	}

	return true;
}

void AWeaponBase::FireOnce()
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
		&AWeaponBase::ResetFireCooldown,
		FireRate,
		false
	);

	UE_LOG(LogTemp, Warning, TEXT("[WEAPON] Fired: %s | Ammo: %d / %d"),
		*GetName(),
		AmmoInMagazine,
		AmmoInReserve
	);

	FHitResult PlayerHit;
	FVector AimPoint;

	const bool bPlayerTraceHit = CreatePlayerBulletTrace(PlayerHit, AimPoint);

	FHitResult WeaponHit;
	const bool bWeaponTraceHit = CreateWeaponBulletTrace(AimPoint, WeaponHit);

	if (bWeaponTraceHit)
	{
		ResolveBulletHitResult(WeaponHit);
	}
	else if (bPlayerTraceHit)
	{
		ResolveBulletHitResult(PlayerHit);
	}
}

void AWeaponBase::ResetFireCooldown()
{
	bCanFire = true;
}

bool AWeaponBase::CanReload() const
{
	if (bIsReloading)
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

	return true;
}

// WeaponBase.cpp

void AWeaponBase::Reload()
{
	if (!CanReload())
	{
		UE_LOG(LogTemp, Warning, TEXT("[WEAPON] Cannot reload."));
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
	bCanFire = false;

	UE_LOG(LogTemp, Warning,
		TEXT("[WEAPON] Reload started: %s | ReloadDuration: %.2f | AmmoInsertTime: %.2f"),
		*GetName(),
		ReloadDuration,
		ActualAmmoInsertTime
	);

	GetWorldTimerManager().SetTimer(
		AmmoInsertTimerHandle,
		this,
		&AWeaponBase::InsertAmmoIntoMagazine,
		ActualAmmoInsertTime,
		false
	);

	GetWorldTimerManager().SetTimer(
		ReloadFinishedTimerHandle,
		this,
		&AWeaponBase::FinishReload,
		ReloadDuration,
		false
	);
}

void AWeaponBase::InsertAmmoIntoMagazine()
{
	const int32 AmmoNeeded = MagazineSize - AmmoInMagazine;
	const int32 AmmoToInsert = FMath::Min(AmmoNeeded, AmmoInReserve);

	AmmoInMagazine += AmmoToInsert;
	AmmoInReserve -= AmmoToInsert;

	UE_LOG(LogTemp, Warning, TEXT("[WEAPON] Ammo inserted: %s | Ammo: %d / %d"),
		*GetName(),
		AmmoInMagazine,
		AmmoInReserve
	);
}

void AWeaponBase::FinishReload()
{
	bIsReloading = false;
	bCanFire = true;

	GetWorldTimerManager().ClearTimer(AmmoInsertTimerHandle);
	GetWorldTimerManager().ClearTimer(ReloadFinishedTimerHandle);

	UE_LOG(LogTemp, Warning, TEXT("[WEAPON] Reload finished: %s"), *GetName());
}

bool AWeaponBase::CreatePlayerBulletTrace(FHitResult& OutPlayerHit, FVector& OutAimPoint)
{
	AFPSPlayerCharacter* PlayerCharacter = Cast<AFPSPlayerCharacter>(GetOwner());

	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("[PLAYER TRACE] Failed: Weapon has no valid AFPSPlayerCharacter owner."));
		return false;
	}

	if (!PlayerCharacter->PlayerCamera)
	{
		UE_LOG(LogTemp, Error, TEXT("[PLAYER TRACE] Failed: PlayerCamera is null."));
		return false;
	}

	const FVector StartLocation = PlayerCharacter->PlayerCamera->GetComponentLocation();
	const FVector EndLocation = StartLocation + PlayerCharacter->PlayerCamera->GetForwardVector() * MaxTravelDistance;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(PlayerCharacter);
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

bool AWeaponBase::CreateWeaponBulletTrace(const FVector& AimPoint, FHitResult& OutWeaponHit)
{
	if (!MuzzleLocation)
	{
		UE_LOG(LogTemp, Error, TEXT("[WEAPON TRACE] Failed: MuzzleLocation is null."));
		return false;
	}

	AFPSPlayerCharacter* PlayerCharacter = Cast<AFPSPlayerCharacter>(GetOwner());

	const FVector StartLocation = MuzzleLocation->GetComponentLocation();
	const FVector Direction = (AimPoint - StartLocation).GetSafeNormal();
	const FVector EndLocation = AimPoint + Direction * MuzzleTraceOvershootDistance;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.bTraceComplex = bBulletTraceComplex;

	if (PlayerCharacter)
	{
		QueryParams.AddIgnoredActor(PlayerCharacter);
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

void AWeaponBase::ResolveBulletHitResult(const FHitResult& HitResult)
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

