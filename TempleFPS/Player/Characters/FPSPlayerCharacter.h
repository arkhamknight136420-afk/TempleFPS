#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "../../Characters/BaseCharacter.h"
#include "FPSPlayerCharacter.generated.h"

class UBaseMovementState;
class UFPSBrainComponent;
class UInteractionInterface;
class UInventoryComponent;
class AWeaponBase;
class UHealthComponent;
class UDeathComponent;
class UUCharacterAudioComponent;

UCLASS()
class TEMPLEFPS_API AFPSPlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AFPSPlayerCharacter();

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;



	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	//Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* PlayerCamera;

	//Interaction
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UCapsuleComponent* InteractionCapsule;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealthComponent* HealthComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDeathComponent* DeathComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UUCharacterAudioComponent* CharacterAudioComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UFPSBrainComponent* BrainComponent = nullptr;

	AActor* CurrentInteractableActor = nullptr;

	UInventoryComponent* GetInventoryComponent() const;


	UFUNCTION()
	void OnInteractionCapsuleBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UFUNCTION()
	void OnInteractionCapsuleEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	float SpringArmTargetArmLength = 300.f;

	UFUNCTION(BlueprintCallable)
	void AttachWeaponToCharacter(AWeaponBase* WeaponToAttach);

	// Core movement execution functions
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void StartCrouchMovement();

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void StopCrouchMovement();

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void StartSlowWalkMovement();

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void StopSlowWalkMovement();

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void StartJumpMovement();

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void HandleDirectionalMovement(FVector2D MoveInput);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void StartWalkMovement();

	void StopWalkMovement();

	bool IsGrounded();

	void HandleInteract();

	UFUNCTION()
	void StartShooting();

	UFUNCTION()
	void StopShooting();

	void StartAiming();

	void StopAiming();

	UFUNCTION()
	void ReloadWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipPrimaryWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipSecondaryWeapon();


	


protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BluePrintReadOnly, Category = "Combat|Aiming")
	float DefaultFieldOfView = 90.f;

	UPROPERTY(EditDefaultsOnly, BluePrintReadOnly, Category = "Combat|Aiming")
	 float ADSFieldOfView = 60.f;

	 UPROPERTY(EditDefaultsOnly, BluePrintReadOnly, Category = "Combat|Aiming")
	 float AimInterpSpeed = 300.f;

	 bool IsInterpolatingAim = false;

	 bool IsAiming = false;

	 // Camera Crouch Settings

	

	 void HandleADSInterpolation();

	
private:
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Speed")
	float WalkSpeed = 900.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Speed")
	float SlowWalkSpeed = 200.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Speed")
	float CrouchSpeed = 200.f;


	UPROPERTY(EditDefaultsOnly, Category = "Movement|Speed")
	float AimSpeed = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Stance")
	float StandingCapsuleHalfHeight = 88.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Stance")
	float CrouchedCapsuleHalfHeight = 60.0f;



	
};