#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DeathComponent.generated.h"

class ABaseCharacter;
class ABaseAICharacter;
class ACharacter;
class AAIController;

UCLASS(
	ClassGroup = (Custom),
	meta = (BlueprintSpawnableComponent)
)
class TEMPLEFPS_API UDeathComponent
	: public UActorComponent
{
	GENERATED_BODY()

public:

	//=====================================================
	// Unreal Lifecycle
	//=====================================================

	UDeathComponent();

	//=====================================================
	// Death
	//=====================================================

	UFUNCTION(BlueprintCallable, Category = "Death")
	void HandleDeath();

protected:

	virtual void BeginPlay() override;

private:

	//=====================================================
	// GameMode Notification
	//=====================================================

	/*
	 * Tells AFPSGameMode that this character no longer counts as living
	 * and that a replacement should be scheduled.
	 *
	 * Returns the corpse lifetime configured by the GameMode.
	 */
	float NotifyGameModeOfDeath(
		ABaseCharacter* Character
	);

	//=====================================================
	// Controller Cleanup
	//=====================================================

	void HandleControllerDeath(
		ACharacter* Character
	);

	void HandleAIControllerDeath(
		ABaseAICharacter* AICharacter,
		AAIController* AIController
	);

	//=====================================================
	// Corpse Setup
	//=====================================================

	void EnableRagdoll(
		ACharacter* Character
	);

	void DropCharacterWeapon(
		ACharacter* Character
	);

	void ScheduleCorpseDestruction(
		ACharacter* Character,
		float CorpseLifetime
	);

	//=====================================================
	// Runtime State
	//=====================================================

	/*
	 * Prevents multiple calls from notifying the GameMode and scheduling
	 * multiple respawns for the same death.
	 */
	bool bHasHandledDeath = false;
};