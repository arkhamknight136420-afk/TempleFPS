#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "FPSPlayerController.generated.h"

class AFPSPlayerCharacter;
class UFPSBrainComponent;


/**
 *
 */
UCLASS()
class TEMPLEFPS_API  AFPSPlayerController : public APlayerController
{
	GENERATED_BODY()


protected:

	//Brain Reference
	UFPSBrainComponent* PlayerBrain;

	// Mapping Context
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input")
	UInputMappingContext* PlayerMappingContext;

	


	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|Movement")
	bool ToggleCrouch = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|Movement")
	bool ToggleSlowWalk = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|Movement")
	bool ToggleAim = false;



	// Movement
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|Movement")
	UInputAction* IA_Move;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|Movement")
	UInputAction* IA_Look;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|Movement")
	UInputAction* IA_Interact;


	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|Movement")
	UInputAction* IA_SlowWalk;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|Movement")
	UInputAction* IA_Crouch;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|Movement")
	UInputAction* IA_JumpAndMantle;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|Movement")
	UInputAction* IA_Reload;

	// Combat
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|Combat")
	UInputAction* IA_Shoot;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|Combat")
	UInputAction* IA_Aim;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|Combat")
	UInputAction* IA_EquipPrimary;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|Combat")
	UInputAction* IA_EquipSecondary;

	UPROPERTY(BlueprintReadOnly)
	AFPSPlayerCharacter* AFPSPlayerCharacterRef;


	virtual void OnPossess(APawn* InPawn) override;


	virtual void SetupInputComponent() override;


	virtual void BeginPlay() override;



	// Movement
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void Input_Move(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void Input_Look(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void Input_Interact(const FInputActionValue& Value);

	// Sprint
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void Input_SlowWalk_Start(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void Input_SlowWalk_End(const FInputActionValue& Value);

	// Crouch
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void Input_Crouch_Start(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void Input_Crouch_End(const FInputActionValue& Value);

	// Jump
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void Input_Jump_Start(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void Input_Jump_End(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void Input_Reload_Start(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void Input_Reload_End(const FInputActionValue& Value);

	// Shooting
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void Input_Shoot_Start(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void Input_Shoot_End(const FInputActionValue& Value);


	// Aim
	UFUNCTION(BlueprintCallable, Category = "Actions")
	void Input_Aim_Start(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void Input_Aim_End(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void Input_EquipPrimary(const FInputActionValue& Value);

	UFUNCTION(BlueprintCallable, Category = "Actions")
	void Input_EquipSecondary(const FInputActionValue& Value);




};