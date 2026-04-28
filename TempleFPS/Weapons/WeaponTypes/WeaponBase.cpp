// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"


// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("WeaponMesh"));

}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

 void AWeaponBase::Interact(AActor* Interactor)
{

}

 FString AWeaponBase::GetPromptText()
{
	 return FString(TEXT("Press E to pick up Base Weapon"));

}

UFUNCTION(BlueprintCallable)
void AWeaponBase::StartFire()
{

}

UFUNCTION(BluePrintCallable)
void AWeaponBase::StopFire()
{

}

UFUNCTION(BluePrintCallable)
void AWeaponBase::CanFire()
{

}

UFUNCTION(BlueprintCallable)
void AWeaponBase::Reload()
{

}

