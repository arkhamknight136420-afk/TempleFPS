// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UCharacterAudioComponent.generated.h"

class ACharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TEMPLEFPS_API UUCharacterAudioComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUCharacterAudioComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;



	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundWave* RunningFootStepSFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundWave* JumpStartSFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundWave* LandingSFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundWave* EnterCrouchSFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundWave* ExitCrouchSFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Audio")
	USoundWave* DeathSFX;

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayLeftFootStepSFX();

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayRightFootStepSFX();

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayJumpStartSFX();

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayLandingSFX();	

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayEnterCrouchSFX();	

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayExitCrouchSFX();	

	UFUNCTION(BlueprintCallable, Category = "Audio")
	void PlayDeathSFX();	
	

protected:
	// Called when the game starts
	virtual void BeginPlay() override;


private:

	ACharacter* OwningCharacterRef;

	FName LeftFootSocketName = TEXT("foot_l_socket");
	FName RightFootSocketName = TEXT("foot_r_socket");
		
};
