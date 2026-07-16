#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputAction.h"
#include "InputMappingContext.h"
#include "../../UI/Enums/DamageNumberTypes.h"
#include "FPSPlayerController.generated.h"

class AFPSPlayerCharacter;
class UFPSBrainComponent;
class ADamageNumberActor;


/**
 *
 */
UCLASS()
class TEMPLEFPS_API  AFPSPlayerController : public APlayerController
{
	GENERATED_BODY()

public: 

	void ShowDamageNumber(
		float DamageAmount,
		EDamageNumberType DamageNumberType,
		const FVector& WorldLocation
	);

protected:

	//Brain Reference
	UFPSBrainComponent* PlayerBrain;

	// Mapping Context
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input")
	UInputMappingContext* PlayerMappingContext;

	UPROPERTY(
		EditDefaultsOnly,
		Category = "UI|Damage Numbers"
	)
	TSubclassOf<ADamageNumberActor> DamageNumberActorClass;


	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|Movement")
	bool ToggleCrouch = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|Movement")
	bool ToggleSlowWalk = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|Movement")
	bool ToggleAim = false;

	UPROPERTY(BlueprintReadOnly,VisibleDefaultsOnly, Category = "Input|Movement")
	bool IsInputingAim = false;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|Sensitivity")
	float HorizontalSensitivityMultiplier = 1.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|Sensitivity")
	float VerticalSensitivityMultiplier = 1.0f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|Sensitivity")
	float ADSHorizontalSensitivityMultiplier = .25f;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Input|Sensitivity")
	float ADSVerticalSensitivityMultiplier = .25f;



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

	virtual void OnUnPossess() override;


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



	private:


};