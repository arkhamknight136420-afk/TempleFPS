// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PrimaryWeaponBase.h"
#include "AssaultRifleWeapon.generated.h"

/**
 * 
 */
UCLASS()
class TEMPLEFPS_API AAssaultRifleWeapon : public APrimaryWeaponBase
{
	GENERATED_BODY()
	
public:

	virtual void Interact(AActor* Interactor) override;

	virtual FString GetPromptText() override;
};
