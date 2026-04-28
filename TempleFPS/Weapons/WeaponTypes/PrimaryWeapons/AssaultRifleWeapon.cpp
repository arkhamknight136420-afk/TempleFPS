// Fill out your copyright notice in the Description page of Project Settings.


#include "AssaultRifleWeapon.h"
#include "../../../Player/Characters/FPSPlayerCharacter.h"
#include "../../../Inventory/Components/InventoryComponent.h"




void AAssaultRifleWeapon:: Interact(AActor * Interactor)
{
	if (Cast<AFPSPlayerCharacter>(Interactor))
	{
		AFPSPlayerCharacter* PlayerCharacter = Cast<AFPSPlayerCharacter>(Interactor);
		UInventoryComponent* InventoryComponent = PlayerCharacter->FindComponentByClass<UInventoryComponent>();

		InventoryComponent->EquipWeapon(this);


	}
}

FString AAssaultRifleWeapon:: GetPromptText()
{
		return FString(TEXT("Press E to pick up Assault Rifle"));
}