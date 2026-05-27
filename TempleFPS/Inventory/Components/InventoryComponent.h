// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class AWeaponBase;
class APrimaryWeaponBase;
class ASecondaryWeaponBase;
class AFPSPlayerCharacter;
class ABaseAICharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEMPLEFPS_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();


protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	// AI / starting loadout selection
	

	// Runtime weapon instances
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapons|Runtime")
	APrimaryWeaponBase* PrimaryWeapon = nullptr;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapons|Runtime")
	ASecondaryWeaponBase* SecondaryWeapon = nullptr;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Weapons|Runtime")
	AWeaponBase* CurrentHeldWeapon = nullptr;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void PickUpWeapon(TSubclassOf<AWeaponBase> WeaponClass);

	void EquipPrimaryWeapon();	

	void EquipSecondaryWeapon();

	AWeaponBase* GetCurrentHeldWeapon() const { return CurrentHeldWeapon; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons|Defaults")
	TSubclassOf<APrimaryWeaponBase> DefaultPrimaryWeaponClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapons|Defaults")
	TSubclassOf<ASecondaryWeaponBase> DefaultSecondaryWeaponClass;


	

	

private: 
	

};
