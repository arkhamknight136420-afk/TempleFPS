// Fill out your copyright notice in the Description page of Project Settings.


#include "DeathComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"


// Sets default values for this component's properties
UDeathComponent::UDeathComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UDeathComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void UDeathComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UDeathComponent::HandleDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("%s has died."), *GetOwner()->GetName());

	ACharacter* Character = Cast<ACharacter>(GetOwner());

	
	if (!Character) return;

	APlayerController* CharacterController = Cast< APlayerController>(Character->GetController());

	if (CharacterController)
	{
		CharacterController->DisableInput(CharacterController);	
	}

	USkeletalMeshComponent* Mesh = Character->GetMesh();
	UCharacterMovementComponent* MovementComponent = Character->GetCharacterMovement();
	UCapsuleComponent* CapsuleComponent = Character->GetCapsuleComponent();

	if (!Mesh || !MovementComponent || !CapsuleComponent)
		{
		UE_LOG(LogTemp, Error, TEXT("DeathComponent on %s could not find necessary components"), *GetOwner()->GetName());
		return;
	}

	MovementComponent->DisableMovement();
	MovementComponent->StopMovementImmediately();

	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Mesh->SetCollisionProfileName(TEXT("Ragdoll"));
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	Mesh->SetAllBodiesSimulatePhysics(true);
	Mesh->SetSimulatePhysics(true);
	Mesh->WakeAllRigidBodies();
	Mesh->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);




}