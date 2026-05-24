// Fill out your copyright notice in the Description page of Project Settings.


#include "UCharacterAudioComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UUCharacterAudioComponent::UUCharacterAudioComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UUCharacterAudioComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ACharacter* AttemptedOwningCharacterRef = Cast<ACharacter>(GetOwner()))
	{
		OwningCharacterRef = AttemptedOwningCharacterRef;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("UCharacterAudioComponent is not attached to a character!"));
	}
}


// Called every frame
void UUCharacterAudioComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UUCharacterAudioComponent::PlayRightFootStepSFX()
{
	UGameplayStatics::PlaySoundAtLocation(this, RunningFootStepSFX, OwningCharacterRef->GetMesh()->GetSocketLocation(RightFootSocketName));

}

void UUCharacterAudioComponent::PlayLeftFootStepSFX()
{
	UGameplayStatics::PlaySoundAtLocation(this, RunningFootStepSFX, OwningCharacterRef->GetMesh()->GetSocketLocation(LeftFootSocketName));
}

void UUCharacterAudioComponent::PlayJumpStartSFX()
{

}

void UUCharacterAudioComponent::PlayLandingSFX()
{

}

void UUCharacterAudioComponent::PlayEnterCrouchSFX()
{
	UGameplayStatics::PlaySoundAtLocation(this, EnterCrouchSFX, OwningCharacterRef->GetMesh()->GetSocketLocation(LeftFootSocketName));
}

void UUCharacterAudioComponent::PlayExitCrouchSFX()
{

}

void UUCharacterAudioComponent::PlayDeathSFX()
{

}