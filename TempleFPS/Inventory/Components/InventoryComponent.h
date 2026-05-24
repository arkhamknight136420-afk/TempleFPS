// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class AWeaponBase;
class APrimaryWeaponBase;
class ASecondaryWeaponBase;
class AFPSPlayerCharacter;

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

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly,Category = "Weapons")
	APrimaryWeaponBase* PrimaryWeapon;

	UPROPERTY(VisibleAnyWhere, BlueprintReadOnly, Category = "Weapons")
	ASecondaryWeaponBase* SecondaryWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapons")
	AWeaponBase* CurrentHeldWeapon;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void PickUpWeapon(TSubclassOf<AWeaponBase> WeaponClass);

	void EquipPrimaryWeapon();	

	void EquipSecondaryWeapon();

	AWeaponBase* GetCurrentHeldWeapon() const { return CurrentHeldWeapon; }
	

private: 
	

};
