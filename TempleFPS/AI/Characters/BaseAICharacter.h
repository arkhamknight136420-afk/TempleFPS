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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UChildActorComponent* HeldWeaponComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TSubclassOf<AWeaponBase> DefaultWeaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UHealthComponent* HealthComponent;

	

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

protected:
	virtual void BeginPlay() override;

	

	

	
};