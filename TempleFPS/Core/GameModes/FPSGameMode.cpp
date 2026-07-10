// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "../../Spawning/FPSCharacterSpawnPoint.h"
#include "EngineUtils.h"


/*
begin play()
GetallSpawnPointsinmap()
SpawnAllCharacter()





*/

void AFPSGameMode::IntitializeSpawnPoints() // get all spawn points in world and store them in an array 
{
	TArray<AActor*> FoundActors;

	UGameplayStatics::GetAllActorsOfClass(
		GetWorld(),
		AFPSCharacterSpawnPoint::StaticClass(),
		FoundActors
	);


	for (AActor* Actor : FoundActors)
	{
		SpawnPoints.Add(
			CastChecked<AFPSCharacterSpawnPoint>(Actor)
		);
	}

}
