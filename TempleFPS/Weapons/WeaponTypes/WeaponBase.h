// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "../../InteractionInterface.h"
#include "WeaponBase.generated.h"

class AFPSPlayerCharacter;
class UInventoryComponent;

UCLASS()
class TEMPLEFPS_API AWeaponBase : public AActor, public IInteractionInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* WeaponMesh;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	virtual void Interact(AActor* Interactor) override;	

	virtual FString GetPromptText() override;

	UFUNCTION(BlueprintCallable)
	virtual void StartFire();

	UFUNCTION(BluePrintCallable)
	virtual void StopFire();

	UFUNCTION(BluePrintCallable)
	virtual void CanFire();

	UFUNCTION(BlueprintCallable)
	virtual void Reload();


private:

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float Damage = 20.f;	

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float FireRate = 0.25f;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float AmmoInMagazine = 30.f;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float AmmoInReserve = 90.f;

	UPROPERTY(EditDefaultsOnly, Category = "Stats")
	float MaxTravelDistance = 90.f;



};
