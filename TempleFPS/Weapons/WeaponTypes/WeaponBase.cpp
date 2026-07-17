#include "WeaponBase.h"
#include "../../Inventory/Components/InventoryComponent.h"
#include "../../Player/Characters/FPSPlayerCharacter.h"
#include "DrawDebugHelpers.h"
#include "TimerManager.h"
#include "../../ActorComponents/HealthComponent.h"
#include "../../Characters/BaseCharacter.h"
#include "NiagaraSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "NiagaraDataInterfaceArrayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "../../Player/Controllers/FPSPlayerController.h"
#include "../../AI/Characters/BaseAICharacter.h"
#include "../../UI/Enums/DamageNumberTypes.h"


	//=====================================================
	// LIFECYCLE
	//=====================================================

AWeaponBase::AWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);

	MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzle"));
	MuzzleLocation->SetupAttachment(WeaponMesh);

	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	WeaponMesh->SetGenerateOverlapEvents(true);
	WeaponMesh->SetCollisionObjectType(ECC_GameTraceChannel1);
	WeaponMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
	WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	WeaponMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	WeaponMesh->SetSimulatePhysics(true);
	WeaponMesh->SetEnableGravity(true);


}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();

	MagazineSize = FMath::Max(MagazineSize, 0);

	AmmoInMagazine = FMath::Clamp(AmmoInMagazine, 0, MagazineSize);
}

void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

	//=====================================================
	// INTERACTION
	//=====================================================

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
	return TEXT("Pick Up Weapon (E)");
}

	//=====================================================
	// EQUIPMENT AND ATTACHMENT
	//=====================================================

void AWeaponBase::SetWeaponEquipped()
{
	if (!WeaponMesh) return;

	WeaponMesh->SetSimulatePhysics(false);
	WeaponMesh->SetEnableGravity(false);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SetActorHiddenInGame(false);
	SetActorEnableCollision(false);
}

UStaticMesh* AWeaponBase::GetWeaponStaticMesh() const
{
	return WeaponMesh ? WeaponMesh->GetStaticMesh() : nullptr;
}

	//=====================================================
	// FIRING
	//=====================================================

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
	IsShooting = false;
	UE_LOG(LogTemp, Warning, TEXT("StopFire Called"));
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

	OnAmmoChanged.Broadcast(
		AmmoInMagazine,
		MagazineSize,
		-1
	);

	bCanFire = false;
	IsShooting = true;

	PlayFireSFX();
	PlayMuzzleFlashEffect();

	GetWorldTimerManager().SetTimer(
		FireCooldownTimerHandle,
		this,
		&AWeaponBase::ResetFireCooldown,
		FireRate,
		false
	);

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("[WEAPON] Fired: %s | Ammo: %d / %d"),
		*GetName(),
		AmmoInMagazine,
		MagazineSize
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

	//=====================================================
	// DAMAGE
	//=====================================================

bool AWeaponBase::WasHeadShot(const FHitResult& HitResult)
{
	if (HitResult.BoneName == TEXT("head"))
	{
		return true;
	}
	return false;
}

	//=====================================================
	// RELOADING
	//=====================================================

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

bool AWeaponBase::IsMagazineEmpty() const
{
	if (AmmoInMagazine <= 0)
	{
		return true;
	}
	return false;
}

int32 AWeaponBase::AddAmmo(int32 AdditionalAmmo)
{
	if (AdditionalAmmo <= 0)
	{
		return 0;
	}

	const int32 PreviousAmmo = AmmoInMagazine;

	AmmoInMagazine = FMath::Clamp(
		AmmoInMagazine + AdditionalAmmo,
		0,
		MagazineSize
	);

	const int32 ActualAmmoAdded = AmmoInMagazine - PreviousAmmo;

	if (ActualAmmoAdded <= 0)
	{
		return 0;
	}

	OnAmmoChanged.Broadcast(
		AmmoInMagazine,
		MagazineSize,
		ActualAmmoAdded
	);
	UE_LOG(
		LogTemp,
		Log,
		TEXT("[WeaponBase] Added %d ammo | Ammo: %d / %d"),
		ActualAmmoAdded,
		AmmoInMagazine,
		MagazineSize
	);

	UGameplayStatics::PlaySound2D(GetWorld(), AmmoAddedSound);
	return ActualAmmoAdded;
}



	//=====================================================
	// BULLET TRACING
	//=====================================================

bool AWeaponBase::CreatePlayerBulletTrace(FHitResult& OutPlayerHit, FVector& OutAimPoint)
{
	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(GetOwner()); // HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE 

	if (!BaseCharacter) // HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE 
	{
		UE_LOG(LogTemp, Error, TEXT("[PLAYER TRACE] Failed: Weapon has no valid AFPSPlayerCharacter owner."));
		return false;
	}

	if (!BaseCharacter->EyesLocation) // HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE 
	{
		UE_LOG(LogTemp, Error, TEXT("[PLAYER TRACE] Failed: EyesLocation is null."));
		return false;
	}

	const FVector StartLocation = BaseCharacter->GetAimStartLocation();
	const FVector EndLocation = StartLocation + BaseCharacter->GetAimDirection() * MaxTravelDistance;

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

	if (DebugBullets)
	{
		const FColor TraceColor = bHit ? FColor::Green : FColor::Red;

		DrawDebugLine(GetWorld(), StartLocation, OutAimPoint, TraceColor, false, .5f, 0, .5f);

		if (bHit)
		{
			DrawDebugSphere(GetWorld(), OutPlayerHit.ImpactPoint, 8.0f, 12, FColor::Green, false, .5f);
		}
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

	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(GetOwner());  // HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE HERE 

	const FVector StartLocation = MuzzleLocation->GetComponentLocation();
	const FVector Direction = (AimPoint - StartLocation).GetSafeNormal();
	const FVector EndLocation = AimPoint + Direction * MuzzleTraceOvershootDistance;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.bTraceComplex = bBulletTraceComplex;



	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		OutWeaponHit,
		StartLocation,
		EndLocation,
		BulletTraceChannel,
		QueryParams
	);
	const FVector TracerEnd =
		bHit ? OutWeaponHit.ImpactPoint : EndLocation;

	SpawnBulletTracerEffect(StartLocation, TracerEnd);

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

	if (DebugBullets)
	{
		const FVector DebugEnd = bHit ? OutWeaponHit.ImpactPoint : EndLocation;
		const FColor TraceColor = bHit ? FColor::Cyan : FColor::Orange;

		DrawDebugLine(GetWorld(), StartLocation, DebugEnd, TraceColor, false, .5f, 0, .5f);

		if (bHit)
		{
			DrawDebugSphere(GetWorld(), OutWeaponHit.ImpactPoint, 8.0f, 12, FColor::Cyan, false, .5f);
		}
	}

	

	return bHit;
}

	//=====================================================
	// HIT RESOLUTION
	//=====================================================

void AWeaponBase::ResolveBulletHitResult(
	const FHitResult& HitResult
)
{
	if (DebugBullets)
	{
		DrawDebugSphere(
			GetWorld(),
			HitResult.ImpactPoint,
			20.f,
			16,
			FColor::Yellow,
			false,
			0.5f
		);
	}

	AActor* HitActor = HitResult.GetActor();

	if (!HitActor)
	{
		return;
	}

	// Only process character skeletal-mesh hits.
	if (!Cast<USkeletalMeshComponent>(
		HitResult.GetComponent()
	))
	{
		return;
	}

	UHealthComponent* HealthComponent =
		HitActor->GetComponentByClass<UHealthComponent>();

	if (!HealthComponent)
	{
		return;
	}

	// Do not apply damage or display numbers for corpses.
	if (HealthComponent->IsDead())
	{
		return;
	}

	const bool bHeadShot = WasHeadShot(HitResult);

	const float DamageToApply = bHeadShot
		? Damage * HeadShotDamageMultiplier
		: Damage;

	if (bHeadShot)
	{
		UE_LOG(
			LogTemp,
			Log,
			TEXT("[WeaponBase] Headshot")
		);
	}
	else
	{
		UE_LOG(
			LogTemp,
			Log,
			TEXT("[WeaponBase] Body shot")
		);
	}

	// This finishes before execution continues.
	HealthComponent->ApplyDamage(DamageToApply);

	
	// The actor was confirmed alive before ApplyDamage. Therefore,
	// if it is dead now, this specific hit killed it.
	const bool bJustDied = HealthComponent->IsDead();

	EDamageNumberType DamageNumberType;

	if (bJustDied)
	{
		DamageNumberType = EDamageNumberType::Kill;
	}
	else if (bHeadShot)
	{
		DamageNumberType = EDamageNumberType::HeadShot;
	}
	else
	{
		DamageNumberType = EDamageNumberType::BodyShot;
	}

	// Damage still happens for every valid shooter. Only the UI
	// request is restricted to the local player damaging an AI.
	AFPSPlayerCharacter* PlayerShooter =
		Cast<AFPSPlayerCharacter>(GetOwner());

	ABaseAICharacter* AITarget =
		Cast<ABaseAICharacter>(HitActor);

	if (!PlayerShooter ||
		!PlayerShooter->IsLocallyControlled() ||
		!AITarget)
	{
		return;
	}

	AFPSPlayerController* PlayerController =
		Cast<AFPSPlayerController>(
			PlayerShooter->GetController()
		);

	if (!PlayerController)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"[WeaponBase] Player shooter does not have "
				"an AFPSPlayerController."
			)
		);

		return;
	}

	PlayHitSound(DamageNumberType);

	const FVector DamageNumberLocation =
		HitResult.ImpactPoint +
		FVector::UpVector * 25.f;

	PlayerController->ShowDamageNumber(
		DamageToApply,
		DamageNumberType,
		DamageNumberLocation
	);
}

	//=====================================================
	// AUDIO
	//=====================================================

void AWeaponBase::PlayFireSFX()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSFX, MuzzleLocation->GetComponentLocation(), 1.0f);
}

void AWeaponBase::PlayFireOnClipEmptySFX()
{
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireOnClipEmptySFX, MuzzleLocation->GetComponentLocation(), 1.0f);
}

void AWeaponBase::PlayReloadStartSFX()
{

}

void AWeaponBase::PlayReloadInsetSFX()
{

}

void AWeaponBase::PlayReloadEndSFX()
{

}

void AWeaponBase::PlayHitSound(EDamageNumberType DamageNumberType)
{
	USoundBase* HitSoundEffect = nullptr;
	if (DamageNumberType == EDamageNumberType::HeadShot)
	{
		HitSoundEffect = HeadShotHitSound;
	}
	else if (DamageNumberType == EDamageNumberType::BodyShot)
	{
		HitSoundEffect = BodyShotHitSound;
	}
	else if (DamageNumberType == EDamageNumberType::Kill)
	{
		HitSoundEffect = KillShotHitSound;
	}

	if (!IsValid(HitSoundEffect))
	{
		return;
	}

	UGameplayStatics::PlaySound2D(GetWorld(), HitSoundEffect);
}


	//=====================================================
	// VFX
	//=====================================================

void AWeaponBase::PlayMuzzleFlashEffect()
{
	UNiagaraFunctionLibrary::SpawnSystemAttached(
		MuzzleFlashEffect,
		MuzzleLocation,
		NAME_None,
		FVector::ZeroVector,
		FRotator(0.f, -90.f, 0.f),
		EAttachLocation::SnapToTarget,
		true
	);





}

void AWeaponBase::SpawnBulletTracerEffect(FVector MuzzlePosition, FVector ImpactPosition)
{
	if (!TracerEffect || !GetWorld())
	{
		return;
	}

	UNiagaraComponent* TracerComponent =
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),
			TracerEffect,
			MuzzlePosition,
			FRotator::ZeroRotator,
			FVector::OneVector,
			true,                  // Auto destroy
			false,                 // Do not activate yet
			ENCPoolMethod::None,
			true                   // Pre-cull check
		);

	if (!TracerComponent)
	{
		return;
	}

	TracerComponent->SetVariableVec3(
		TEXT("User.MuzzlePosition"), 
		MuzzlePosition
	);


	// this is supposed to store an arrat of impact positons but we only have one so this is just adding one position
	const TArray<FVector> ImpactPositions =
	{
		ImpactPosition
	};

	UNiagaraDataInterfaceArrayFunctionLibrary::SetNiagaraArrayVector(
		TracerComponent,
		TEXT("User.ImpactPositions"),
		ImpactPositions
	);

	TracerComponent->SetVariableBool(TEXT("User.Trigger"), true);

	TracerComponent->Activate(true);

}

	

