// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSCharacterSpawnPoint.h"

// Sets default values
AFPSCharacterSpawnPoint::AFPSCharacterSpawnPoint()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AFPSCharacterSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFPSCharacterSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

