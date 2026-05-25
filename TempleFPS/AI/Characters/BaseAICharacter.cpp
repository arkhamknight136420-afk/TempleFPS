// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAICharacter.h"
#include "../../ActorComponents/HealthComponent.h"
#include "../../ActorComponents/DeathComponent.h"
#include "../Controllers/BaseAIController.h"


// Sets default values
ABaseAICharacter::ABaseAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AIControllerClass = ABaseAIController::StaticClass(); //set this character to be controlled by this AI controller class if there needs to be one

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;  // whenever this character is placed into the world or spawned automatically possess this character using
	// the ai controller class listed above 


	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	DeathComponent = CreateDefaultSubobject<UDeathComponent>(TEXT("DeathComponent"));

	GetMesh()->SetCollisionResponseToChannel(ECC_GameTraceChannel2, ECollisionResponse::ECR_Block);
}

// Called when the game starts or when spawned
void ABaseAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

