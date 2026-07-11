#include "DeathComponent.h"

#include "Engine/World.h"

#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"

#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "AIController.h"
#include "BrainComponent.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"

#include "../Characters/BaseCharacter.h"
#include "../AI/Characters/BaseAICharacter.h"
#include "../Core/GameModes/FPSGameMode.h"
#include "../Player/Characters/FPSPlayerCharacter.h"

UDeathComponent::UDeathComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UDeathComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UDeathComponent::HandleDeath()
{
	/*
	 * Death may only be processed once for this actor.
	 */
	if (bHasHandledDeath)
	{
		return;
	}

	bHasHandledDeath = true;

	ABaseCharacter* Character =
		Cast<ABaseCharacter>(GetOwner());

	if (!IsValid(Character))
	{
		return;
	}

	UE_LOG(
		LogTemp,
		Warning,
		TEXT("%s has died."),
		*Character->GetName()
	);

	/*
	 * Notify the GameMode before unpossessing the controller.
	 *
	 * The GameMode needs the current controller reference to determine
	 * whether this was the human player or an AI character.
	 */
	const float CorpseLifetime =
		NotifyGameModeOfDeath(Character);

	/*
	 * Disable or destroy the current controller behavior.
	 */
	HandleControllerDeath(Character);

	/*
	 * Drop the weapon before the corpse actor is eventually destroyed.
	 */
	DropCharacterWeapon(Character);

	/*
	 * Convert the character into a physics ragdoll.
	 */
	EnableRagdoll(Character);

	/*
	 * The dead actor remains visible as a corpse but no longer counts
	 * toward MaxCharacters.
	 */
	ScheduleCorpseDestruction(
		Character,
		CorpseLifetime
	);
}

float UDeathComponent::NotifyGameModeOfDeath(
	ABaseCharacter* Character
)
{
	constexpr float DefaultCorpseLifetime = 10.f;

	UWorld* World = GetWorld();

	if (!World)
	{
		return DefaultCorpseLifetime;
	}

	AFPSGameMode* GameMode =
		World->GetAuthGameMode<AFPSGameMode>();

	if (!IsValid(GameMode))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"[DeathComponent] Could not find "
				"AFPSGameMode."
			)
		);

		return DefaultCorpseLifetime;
	}

	GameMode->HandleCharacterDeath(Character);

	return GameMode->GetCorpseLifetime();
}

void UDeathComponent::HandleControllerDeath(
	ACharacter* Character
)
{
	if (!IsValid(Character))
	{
		return;
	}

	/*
	 * Player death:
	 *
	 * - Disable input on the dead pawn.
	 * - Unpossess it.
	 * - Keep the PlayerController alive for RestartPlayerAtPlayerStart.
	 */
	if (APlayerController* PlayerController =
		Cast<APlayerController>(
			Character->GetController()
		))
	{
		/*
		 * Disable input on both the pawn and the PlayerController.
		 *
		 * Disabling only the pawn is not enough when input actions are
		 * processed through the PlayerController.
		 */
		Character->DisableInput(PlayerController);
		PlayerController->DisableInput(PlayerController);

		/*
		 * Clear any keys that were held when death occurred.
		 */
		PlayerController->FlushPressedKeys();

		/*
		 * Disconnect the controller from the dead character while preserving
		 * the controller for respawning.
		 */
		PlayerController->UnPossess();

		return;
	}

	/*
	 * AI death:
	 *
	 * Completely shut down and destroy the AIController.
	 */
	if (AAIController* AIController =
		Cast<AAIController>(
			Character->GetController()
		))
	{
		if (ABaseAICharacter* AICharacter =
			Cast<ABaseAICharacter>(Character))
		{
			HandleAIControllerDeath(
				AICharacter,
				AIController
			);
		}
	}
}

void UDeathComponent::HandleAIControllerDeath(
	ABaseAICharacter* AICharacter,
	AAIController* AIController
)
{
	if (
		!IsValid(AICharacter)
		|| !IsValid(AIController)
		)
	{
		return;
	}

	AICharacter->StopShooting();

	if (UBlackboardComponent* Blackboard =
		AIController->GetBlackboardComponent())
	{
		Blackboard->ClearValue(TEXT("Player"));
	}

	if (UBrainComponent* Brain =
		AIController->GetBrainComponent())
	{
		Brain->StopLogic(TEXT("Death"));
		Brain->SetComponentTickEnabled(false);
	}

	if (UAIPerceptionComponent* Perception =
		AIController->GetPerceptionComponent())
	{
		Perception->ForgetAll();
		Perception->Deactivate();
		Perception->SetComponentTickEnabled(false);
	}

	AIController->ClearFocus(
		EAIFocusPriority::Gameplay
	);

	AIController->StopMovement();
	AIController->UnPossess();
	AIController->Destroy();

	UE_LOG(
		LogTemp,
		Log,
		TEXT(
			"AI controller fully cleaned up after death."
		)
	);
}

void UDeathComponent::EnableRagdoll(
	ACharacter* Character
)
{
	if (!IsValid(Character))
	{
		return;
	}

	USkeletalMeshComponent* Mesh =
		Character->GetMesh();

	UCharacterMovementComponent* MovementComponent =
		Character->GetCharacterMovement();

	UCapsuleComponent* CapsuleComponent =
		Character->GetCapsuleComponent();

	if (
		!IsValid(Mesh)
		|| !IsValid(MovementComponent)
		|| !IsValid(CapsuleComponent)
		)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"DeathComponent on %s could not find "
				"the required character components."
			),
			*Character->GetName()
		);

		return;
	}

	MovementComponent->DisableMovement();
	MovementComponent->StopMovementImmediately();

	/*
	 * The dead capsule should no longer block navigation or count as
	 * a living character collision body.
	 */
	CapsuleComponent->SetCollisionEnabled(
		ECollisionEnabled::NoCollision
	);

	Mesh->SetCollisionProfileName(TEXT("Ragdoll"));

	Mesh->SetCollisionEnabled(
		ECollisionEnabled::QueryAndPhysics
	);

	Mesh->SetAllBodiesSimulatePhysics(true);
	Mesh->SetSimulatePhysics(true);
	Mesh->WakeAllRigidBodies();

	Mesh->DetachFromComponent(
		FDetachmentTransformRules::KeepWorldTransform
	);
}

void UDeathComponent::DropCharacterWeapon(
	ACharacter* Character
)
{
	if (ABaseAICharacter* AICharacter =
		Cast<ABaseAICharacter>(Character))
	{
		AICharacter->DropCurrentHeldWeapon();
	}
}

void UDeathComponent::ScheduleCorpseDestruction(
	ACharacter* Character,
	float CorpseLifetime
)
{
	if (!IsValid(Character))
	{
		return;
	}

	if (CorpseLifetime <= 0.f)
	{
		Character->Destroy();
		return;
	}

	/*
	 * SetLifeSpan causes Unreal to destroy the actor after the specified
	 * number of seconds.
	 */
	Character->SetLifeSpan(CorpseLifetime);
}