#include "FPSPlayerController.h"
#include "../Characters/FPSPlayerCharacter.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "../Bralns/FPSBrainComponent.h"
#include "../FiniteStateMachines/MovementStates/JumpState.h"





void AFPSPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	IsInputingAim = false;

	AFPSPlayerCharacterRef = Cast<AFPSPlayerCharacter>(InPawn);

	if (!AFPSPlayerCharacterRef)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed To Cast To AFPSPlayerCharacter"));
		return;
	}

	PlayerBrain = AFPSPlayerCharacterRef->FindComponentByClass<UFPSBrainComponent>();

	if (!PlayerBrain)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find FPSBrainComponent on possessed character"));
	}

	
}

void AFPSPlayerController::OnUnPossess()
{
	IsInputingAim = false;
	AFPSPlayerCharacterRef = nullptr;
	PlayerBrain = nullptr;

	Super::OnUnPossess();
}

void AFPSPlayerController::BeginPlay()
{
	Super::BeginPlay();

	UEnhancedInputLocalPlayerSubsystem* SubsSystem =
		ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (SubsSystem && PlayerMappingContext)
	{
		SubsSystem->AddMappingContext(PlayerMappingContext, 0);
	}

	if (PlayerCameraManager)
	{
		PlayerCameraManager->ViewPitchMin = -50.0f;
		PlayerCameraManager->ViewPitchMax = 50.0f;
	}
}

void AFPSPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	if (!EnhancedInputComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("EnhancedInputComponent not valid"));
		return;
	}

	if (IA_Move)
	{
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Triggered, this, &AFPSPlayerController::Input_Move);
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Completed, this, &AFPSPlayerController::Input_Move);
		EnhancedInputComponent->BindAction(IA_Move, ETriggerEvent::Canceled, this, &AFPSPlayerController::Input_Move);
	}

	if (IA_Look)
	{
		EnhancedInputComponent->BindAction(IA_Look, ETriggerEvent::Triggered, this, &AFPSPlayerController::Input_Look);
	}

	if (IA_Interact)
	{
		EnhancedInputComponent->BindAction(IA_Interact, ETriggerEvent::Started, this, &AFPSPlayerController::Input_Interact);
	}

	if (IA_SlowWalk)
	{
		EnhancedInputComponent->BindAction(IA_SlowWalk, ETriggerEvent::Started, this, &AFPSPlayerController::Input_SlowWalk_Start);
		EnhancedInputComponent->BindAction(IA_SlowWalk, ETriggerEvent::Completed, this, &AFPSPlayerController::Input_SlowWalk_End);
	}

	if (IA_Crouch)
	{
		EnhancedInputComponent->BindAction(IA_Crouch, ETriggerEvent::Started, this, &AFPSPlayerController::Input_Crouch_Start);
		EnhancedInputComponent->BindAction(IA_Crouch, ETriggerEvent::Completed, this, &AFPSPlayerController::Input_Crouch_End);
	}

	if (IA_JumpAndMantle)
	{
		EnhancedInputComponent->BindAction(IA_JumpAndMantle, ETriggerEvent::Started, this, &AFPSPlayerController::Input_Jump_Start);
		EnhancedInputComponent->BindAction(IA_JumpAndMantle, ETriggerEvent::Completed, this, &AFPSPlayerController::Input_Jump_End);
	}

	if (IA_Reload)
	{ 
		EnhancedInputComponent->BindAction(IA_Reload, ETriggerEvent::Started, this, &AFPSPlayerController::Input_Reload_Start);
		EnhancedInputComponent->BindAction(IA_Reload, ETriggerEvent::Completed, this, &AFPSPlayerController::Input_Reload_End);

	}

	if (IA_Shoot)
	{
		EnhancedInputComponent->BindAction(IA_Shoot, ETriggerEvent::Started, this, &AFPSPlayerController::Input_Shoot_Start);
		EnhancedInputComponent->BindAction(IA_Shoot, ETriggerEvent::Completed, this, &AFPSPlayerController::Input_Shoot_End);
	}

	if (IA_Aim)
	{
		EnhancedInputComponent->BindAction(IA_Aim, ETriggerEvent::Started, this, &AFPSPlayerController::Input_Aim_Start);
		EnhancedInputComponent->BindAction(IA_Aim, ETriggerEvent::Completed, this, &AFPSPlayerController::Input_Aim_End);
	}
	if(IA_EquipPrimary)
	{
		EnhancedInputComponent->BindAction(IA_EquipPrimary, ETriggerEvent::Started, this, &AFPSPlayerController::Input_EquipPrimary);
	}
	if(IA_EquipSecondary)
	{
		EnhancedInputComponent->BindAction(IA_EquipSecondary, ETriggerEvent::Started, this, &AFPSPlayerController::Input_EquipSecondary);
	}
}

void AFPSPlayerController::Input_Move(const FInputActionValue& Value)
{
	const FVector2D MoveInput = Value.Get<FVector2D>();
	
	if (!PlayerBrain)
	{
		UE_LOG(LogTemp, Error, TEXT("[INPUT] PlayerBrain is NULL"));
		return;
	}

	PlayerBrain->HandleMoveInputChanged(MoveInput);
}

void AFPSPlayerController::Input_Look(const FInputActionValue& Value)
{
	const FVector2D LookInput = Value.Get<FVector2D>();

	if (IsInputingAim)
	{

		AddYawInput(LookInput.X * ADSHorizontalSensitivityMultiplier);
		AddPitchInput(-LookInput.Y * ADSVerticalSensitivityMultiplier);
	}
	else
	{

		AddYawInput(LookInput.X * HorizontalSensitivityMultiplier);
		AddPitchInput(-LookInput.Y * VerticalSensitivityMultiplier);
	}

}

void AFPSPlayerController::Input_Interact(const FInputActionValue& Value)
{
	AFPSPlayerCharacterRef->HandleInteract();
}

void AFPSPlayerController::Input_SlowWalk_Start(const FInputActionValue& Value)
{
	if (!PlayerBrain)
	{
		return;
	}

	PlayerBrain->BufferingSlowWalk = true;
	if (ToggleSlowWalk)
	{
		if (PlayerBrain->IsSlowWalking())
		{
			PlayerBrain->StopSlowWalk();
		}
		else
		{
			PlayerBrain->TrySlowWalk();
		}
	}
	else
	{
		PlayerBrain->TrySlowWalk();
	}
}

void AFPSPlayerController::Input_SlowWalk_End(const FInputActionValue& Value)
{
	if (!PlayerBrain)
	{
		return;
	}
	PlayerBrain->BufferingSlowWalk = false;

	if (!ToggleSlowWalk)
	{
		PlayerBrain->StopSlowWalk();
	}
}

void AFPSPlayerController::Input_Crouch_Start(const FInputActionValue& Value)
{
	if (!PlayerBrain)
	{
		return;
	}
	PlayerBrain->BufferingCrouch = true;

	if (ToggleCrouch)
	{
		if (PlayerBrain->IsCrouching())
		{
			PlayerBrain->StopCrouch();
		}
		else
		{
			PlayerBrain->TryCrouch();
		}
	}
	else
	{
		PlayerBrain->TryCrouch();
	}
}

void AFPSPlayerController::Input_Crouch_End(const FInputActionValue& Value)
{
	if (!PlayerBrain)
	{
		return;
	}

	PlayerBrain->BufferingCrouch = false;

	if (!ToggleCrouch)
	{
		PlayerBrain->StopCrouch();
	}
}

void AFPSPlayerController::Input_Jump_Start(const FInputActionValue& Value)
{
	PlayerBrain->TryJump();
	UE_LOG(LogTemp, Log, TEXT("Input_Jump_Start called"));
}

void AFPSPlayerController::Input_Jump_End(const FInputActionValue& Value)
{
}


void AFPSPlayerController::Input_Reload_Start(const FInputActionValue& Value)
{
	if (!AFPSPlayerCharacterRef)
	{
		UE_LOG(LogTemp, Error, TEXT("AFPSPlayerCharacterRef is null. Cannot reload."));
		return;
	}

	AFPSPlayerCharacterRef->ReloadWeapon();
}

void AFPSPlayerController::Input_Reload_End(const FInputActionValue& Value)
{
	

}
void AFPSPlayerController::Input_Shoot_Start(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("[SHOOT INPUT] STARTED"));

	if (!AFPSPlayerCharacterRef)
	{
		return;
	}

	AFPSPlayerCharacterRef->StartShooting();
}

void AFPSPlayerController::Input_Shoot_End(const FInputActionValue& Value)
{

	if (!AFPSPlayerCharacterRef)
	{
		return;
	}

	AFPSPlayerCharacterRef->StopShooting();
}

void AFPSPlayerController::Input_Aim_Start(const FInputActionValue& Value)
{
	if (!AFPSPlayerCharacterRef)
	{
		return;
	}

	if (ToggleAim)
	{
		if (IsInputingAim)
		{
			AFPSPlayerCharacterRef->StopAiming();
			IsInputingAim = false;
		}
		else
		{
			AFPSPlayerCharacterRef->StartAiming();
			IsInputingAim = true;
		}
	}
	else
	{
		AFPSPlayerCharacterRef->StartAiming();
		IsInputingAim = true;
	}
}

void AFPSPlayerController::Input_Aim_End(const FInputActionValue& Value)
{
	if (!AFPSPlayerCharacterRef)
	{
		return;
	}

	if (!ToggleAim)
	{
		AFPSPlayerCharacterRef->StopAiming();
		IsInputingAim = false;
	}
}

void AFPSPlayerController::Input_EquipPrimary(const FInputActionValue& Value)
{
	if (!AFPSPlayerCharacterRef)
	{
		UE_LOG(LogTemp, Error, TEXT("AFPSPlayerCharacterRef is null. Cannot equip primary weapon."));
		return;
	}

	AFPSPlayerCharacterRef->EquipPrimaryWeapon();
}


void AFPSPlayerController::Input_EquipSecondary(const FInputActionValue& Value)
{
	if (!AFPSPlayerCharacterRef)
	{
		UE_LOG(LogTemp, Error, TEXT("AFPSPlayerCharacterRef is null. Cannot equip secondary weapon."));
		return;
	}

	AFPSPlayerCharacterRef->EquipSecondaryWeapon();
}