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

UCLASS()
class TEMPLEFPS_API AWeaponBase : public AActor, public IInteractionInterface
{
	GENERATED_BODY()

public:
	//=====================================================
	// UPROPERTIES
	//=====================================================
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* WeaponMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Attachment")
	FName AttachSocketName = TEXT("HandGrip_R");

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Weapon|Attachment")
	FTransform ThirdPersonGripOffset;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	bool IsShooting = false;



	


	//=====================================================
	// UFUNCTIONS
	//=====================================================

	AWeaponBase();

	virtual void Tick(float DeltaTime) override;

	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FString GetPromptText_Implementation() override;

	UFUNCTION()
	bool IsMagazineEmpty() const;

	void SetWeaponEquipped();

	UStaticMesh* GetWeaponStaticMesh() const;


	UStaticMeshComponent* GetWeaponMesh() const
	{
		return WeaponMesh;
	}

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

	UFUNCTION(BlueprintCallable)
	virtual void Reload();

	UFUNCTION(BlueprintCallable)
	virtual bool CanReload() const;

	UFUNCTION(BlueprintCallable)
	virtual bool CreatePlayerBulletTrace(FHitResult& OutPlayerHit, FVector& OutAimPoint);

	UFUNCTION(BlueprintCallable)
	virtual bool CreateWeaponBulletTrace(const FVector& AimPoint, FHitResult& OutWeaponHit);

	UFUNCTION(BlueprintCallable)
	virtual void ResolveBulletHitResult(const FHitResult& HitResult);

	

protected:

	//=====================================================
	// UPROPERTIES
	//=====================================================

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Muzzle", meta = (AllowPrivateAccess = "true"))
	USceneComponent* MuzzleLocation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float Damage = 20.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float HeadShotDamageMultiplier = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float FireRate = 0.25f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	int32 MagazineSize = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	int32 AmmoInMagazine = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	int32 AmmoInReserve = 90;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Stats")
	float MaxTravelDistance = 9000.f;


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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace")
	float MuzzleTraceOvershootDistance = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace")
	TEnumAsByte<ECollisionChannel> BulletTraceChannel = ECC_GameTraceChannel2;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Trace")
	bool bBulletTraceComplex = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bCanFire = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsReloading = false;



	FTimerHandle FireCooldownTimerHandle;
	FTimerHandle ReloadFinishedTimerHandle;
	FTimerHandle AmmoInsertTimerHandle;

//AUDIO


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


//VFX

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "VFX")
	UNiagaraSystem* MuzzleFlashEffect;

	

	
	//=====================================================
	// UFUNCTIONS
	//=====================================================

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void ResetFireCooldown();

	UFUNCTION()
	virtual void InsertAmmoIntoMagazine();

	UFUNCTION()
	virtual void FinishReload();

	UFUNCTION()
	bool WasHeadShot(const FHitResult& HitResult);

//AUDIO
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






//VFX 

	UFUNCTION(BlueprintCallable, Category = "VFX")
	void PlayMuzzleFlashEffect();


	
	



};