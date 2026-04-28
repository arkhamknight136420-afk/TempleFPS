// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"
#include "../../Weapons/WeaponTypes/PrimaryWeapons/PrimaryWeaponBase.h"
#include "../../Weapons/WeaponTypes/SecondaryWeapons/SecondaryWeaponBase.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UInventoryComponent::EquipWeapon(AWeaponBase* NewWeapon)
{
	if (Cast<APrimaryWeaponBase>(NewWeapon))
	{
		PrimaryWeapon = Cast<APrimaryWeaponBase>(NewWeapon);
	}
	else if (Cast<ASecondaryWeaponBase>(NewWeapon))
	{
		SecondaryWeapon = Cast<ASecondaryWeaponBase>(NewWeapon);


	}

}

