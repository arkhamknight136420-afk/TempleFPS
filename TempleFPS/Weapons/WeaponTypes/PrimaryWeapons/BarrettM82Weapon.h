// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PrimaryWeaponBase.h"
#include "BarrettM82Weapon.generated.h"

/**
 * 
 */
UCLASS()
class TEMPLEFPS_API ABarrettM82Weapon : public APrimaryWeaponBase
{
	GENERATED_BODY()
	
public:
	virtual FString GetPromptText_Implementation() override;
};
