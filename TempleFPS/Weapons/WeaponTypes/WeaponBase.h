// WeaponBase.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "../../InteractionInterface.h"
#include "WeaponBase.generated.h"

class AFPSPlayerCharacter;
class ABaseCharacter;
class UInventoryComponent;
class UHealthComponent;
class UNiagaraSystem;
class UTextur2d;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(
	FOnWeaponAmmoChangedSignature,
	int32, CurrentAmmo,
	int32, MaximumAmmo,
	int32, AmmoDelta
);


UCLASS()
class TEMPLEFPS_API AWeaponBase : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

public:

	//=====================================================
	// LIFECYCLE
	//=====================================================

	AWeaponBase();

	virtual void Tick(float DeltaTime) override;


	//=====================================================
	// INTERACTION
	//=====================================================

	virtual void Interact_Implementation(AActor* Interactor) override;

	virtual FString GetPromptText_Implementation() override;


	//=====================================================
	// EQUIPMENT AND ATTACHMENT
	//=====================================================

	void SetWeaponEquipped();

	UStaticMesh* GetWeaponStaticMesh() const;

	UStaticMeshComponent* GetWeaponMesh() const
	{
		return WeaponMesh;
	}

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Attachment")
	FName AttachSocketName = TEXT("HandGrip_R");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Attachment")
	FTransform ThirdPersonGripOffset;


	//=====================================================
	// FIRING
	//=====================================================

	UFUNCTION(BlueprintCallable)
	virtual void TryFire();

	UFUNCTION(BlueprintCallable)
	virtual void StartFire();

	UFUNCTION(BlueprintCallable)
	virtual void StopFire();

	UFUNCTION(BlueprintCallable)
	virtual void FireOnce();

	UFUNCTION(BlueprintCallable)
	virtual bool CanFire() const;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool IsShooting = false;


	//=====================================================
	// RELOADING
	//=====================================================

	UFUNCTION(BlueprintCallable)
	virtual void Reload();

	UFUNCTION(BlueprintCallable)
	virtual bool CanReload() const;


	//=====================================================
	// BULLET TRACING
	//=====================================================

	UFUNCTION(BlueprintCallable)
	virtual bool CreatePlayerBulletTrace(
		FHitResult& OutPlayerHit,
		FVector& OutAimPoint
	);

	UFUNCTION(BlueprintCallable)
	virtual bool CreateWeaponBulletTrace(
		const FVector& AimPoint,
		FHitResult& OutWeaponHit
	);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool DebugBullets = true;

	//=====================================================
	// HIT RESOLUTION
	//=====================================================

	UFUNCTION(BlueprintCallable)
	virtual void ResolveBulletHitResult(const FHitResult& HitResult);


	//=====================================================
	// AMMUNITION
	//=====================================================

	UPROPERTY(BlueprintAssignable, Category = "Weapon|Events")
	FOnWeaponAmmoChangedSignature OnAmmoChanged;

	UFUNCTION()
	bool IsMagazineEmpty() const;

	UFUNCTION(BlueprintPure, Category = "Weapon | Ammo")
	int32 GetCurrentAmmo() const
	{
		return AmmoInMagazine;
	}

	UFUNCTION(BlueprintPure, Category = "Weapon | Ammo")
	int32 GetMaximumAmmo() const
	{
		return MagazineSize;
	}

	UFUNCTION(BlueprintPure, Category = "Weapon|Ammo")
	int32 GetAmmoGrantedPerPickup() const
	{
		return AddedReserveAmmo;
	}

	UFUNCTION(BlueprintPure, Category = "Weapon | UI")
	UTexture2D* GetWeaponIcon() const
	{
		return WeaponIcon.Get();
	}

	

	

	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
	virtual int32 AddAmmo(int32 AdditionalAmmo);


	//=====================================================
		// AUDIO
		//=====================================================


	UFUNCTION()
	void PlayPickupSFX();

protected:

	//=====================================================
	// LIFECYCLE
	//=====================================================

	virtual void BeginPlay() override;


	//=====================================================
	// FIRING
	//=====================================================

	UFUNCTION()
	virtual void ResetFireCooldown();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float FireRate = 0.25f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bCanFire = true;

	FTimerHandle FireCooldownTimerHandle;


	//=====================================================
	// DAMAGE
	//=====================================================

	UFUNCTION()
	bool WasHeadShot(const FHitResult& HitResult);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float Damage = 20.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float HeadShotDamageMultiplier = 2.0f;


	//=====================================================
	// AMMUNITION
	//=====================================================

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	int32 MagazineSize = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	int32 AmmoInMagazine = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	int32 AmmoInReserve = 90;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	int32 AddedReserveAmmo = 20;


	//=====================================================
	// RELOADING
	//=====================================================

	UFUNCTION()
	virtual void InsertAmmoIntoMagazine();

	UFUNCTION()
	virtual void FinishReload();

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Reload",
		meta = (
			ToolTip = "Total amount of time, in seconds, required to fully complete the reload process. This includes the full reload animation, recovery time, and the moment the weapon becomes usable again."
			)
	)
	float ReloadDuration = 2.2f;

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Reload",
		meta = (
			ClampMin = "0.0",
			ClampMax = "1.0",
			UIMin = "0.0",
			UIMax = "1.0",
			ToolTip = "Normalized point during the reload where ammo is inserted into the magazine. 0.0 means ammo is inserted immediately when reload starts. 1.0 means ammo is inserted at the exact end of the reload. Example: 0.65 means ammo is inserted 65% through the reload duration."
			)
	)
	float AmmoInsertNormalizedTime = 0.65f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsReloading = false;

	FTimerHandle ReloadFinishedTimerHandle;

	FTimerHandle AmmoInsertTimerHandle;


	//=====================================================
	// BULLET TRACING
	//=====================================================

	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Muzzle",
		meta = (AllowPrivateAccess = "true")
	)
	USceneComponent* MuzzleLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float MaxTravelDistance = 9000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace")
	float MuzzleTraceOvershootDistance = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace")
	TEnumAsByte<ECollisionChannel> BulletTraceChannel = ECC_GameTraceChannel2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace")
	bool bBulletTraceComplex = true;


	//=====================================================
	// AUDIO
	//=====================================================

	UFUNCTION()
	void PlayFireSFX();

	UFUNCTION()
	void PlayFireOnClipEmptySFX();

	UFUNCTION()
	void PlayReloadStartSFX();

	UFUNCTION()
	void PlayReloadInsetSFX();

	UFUNCTION()
	void PlayReloadEndSFX();

	UFUNCTION()
	void PlayHitSound(EDamageNumberType DamageNumberType);

	

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Audio")
	USoundBase* FireSFX;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Audio")
	USoundBase* FireOnClipEmptySFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundBase* ReloadStartSFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundBase* ReloadInsertSFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundBase* ReloadFinishSFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundBase* BodyShotHitSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundBase* HeadShotHitSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundBase* KillShotHitSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundBase* AmmoAddedSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundBase* PickUpSound;


	//=====================================================
	// VFX
	//=====================================================

	UFUNCTION(BlueprintCallable, Category = "VFX")
	void PlayMuzzleFlashEffect();

	UFUNCTION(BlueprintCallable, Category = "VFX")
	void SpawnBulletTracerEffect(FVector MuzzlePosition, FVector ImpactPosition);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	UNiagaraSystem* MuzzleFlashEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	UNiagaraSystem* TracerEffect;


	//=====================================================
	// UI
	//=====================================================
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|UI")
	TObjectPtr<UTexture2D> WeaponIcon = nullptr;
};