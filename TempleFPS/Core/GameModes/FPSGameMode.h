// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FPSGameMode.generated.h"

/**
 * 
 */

class AFPSCharacterSpawnPoint;

UCLASS()
class TEMPLEFPS_API AFPSGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:

	// SpawnAllCharacter()

	//array off all spawn points

	

protected:

	void IntitializeSpawnPoints();

	TArray<TObjectPtr<AFPSCharacterSpawnPoint>> SpawnPoints;


private:


};
