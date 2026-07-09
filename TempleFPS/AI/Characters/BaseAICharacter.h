#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "../../Characters/BaseCharacter.h"
#include "BaseAICharacter.generated.h"


class UChildActorComponent;
class AWeaponBase;
class UHealthComponent;
class UDeathComponent;
class AWeaponBase;

UCLASS()
class TEMPLEFPS_API ABaseAICharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	ABaseAICharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void HandleDirectionalMovement(FVector2D MoveInput);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Accuracy")
	float MissRadius = 150.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Accuracy")
	float MinBloomDegrees = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Accuracy")
	float MaxBloomDegrees = 10.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UChildActorComponent* HeldWeaponComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AWeaponBase> DefaultWeaponClass;

	UPROPERTY(BlueprintReadOnly)
	AWeaponBase* CurrentHeldWeapon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealthComponent* HealthComponent;

	UPROPERTY()
	AActor* CurrentAimTarget = nullptr;

	void SetAimTarget(AActor* Target);

	virtual FVector GetAimDirection() const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UDeathComponent* DeathComponent;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void AttachWeaponToCharacter(AWeaponBase* WeaponToAttach);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipPrimaryWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void EquipSecondaryWeapon();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StartShooting();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StopShooting();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void ReloadWeapon();

	void UpdateEyePitch(AActor* Target);

	// Starts backing away when closer than this.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat Movement", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float RetreatStartDistance = 700.f;

	// Stops backing away once farther than this.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat Movement", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float RetreatStopDistance = 1000.f;

	// Starts moving closer when farther than this.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat Movement", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float AdvanceStartDistance = 2400.f;

	// Stops moving closer once closer than this.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Combat Movement", meta = (ClampMin = "0.0", UIMin = "0.0"))
	float AdvanceStopDistance = 1900.f;
protected:
	virtual void BeginPlay() override;

	

	

	
};