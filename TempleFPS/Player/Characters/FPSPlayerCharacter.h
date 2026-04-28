#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Camera/CameraComponent.h"
#include "FPSPlayerCharacter.generated.h"

class UBaseMovementState;
class UFPSBrainComponent;

UCLASS()
class TEMPLEFPS_API AFPSPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFPSPlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	
	//Camera
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* PlayerCamera;

	//Interaction
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	UCapsuleComponent* InteractionCapsule;



	
	// Core movement execution functions
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void StartCrouchMovement();

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void StopCrouchMovement();

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void StartSlowWalkMovement();

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void StopSlowWalkMovement();

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void StartJumpMovement();

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void HandleDirectionalMovement(FVector2D MoveInput);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void StartWalkMovement();

	void StopWalkMovement();

	bool IsGrounded();


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UFPSBrainComponent* BrainComponent = nullptr;

private:
	UPROPERTY(EditDefaultsOnly, Category = "Movement|Speed")
	float WalkSpeed = 900.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Speed")
	float SlowWalkSpeed = 600.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Speed")
	float CrouchSpeed = 300.f;


	UPROPERTY(EditDefaultsOnly, Category = "Movement|Speed")
	float AimSpeed = 10.f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Stance")
	float StandingCapsuleHalfHeight = 88.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Movement|Stance")
	float CrouchedCapsuleHalfHeight = 40.0f;

	
};