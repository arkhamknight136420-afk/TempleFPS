#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FPSGameMode.generated.h"

class ABaseCharacter;
class ABaseAICharacter;
class AController;
class APlayerController;
class AFPSCharacterSpawnPoint;

/*
 * AFPSGameMode owns the complete population and respawn system.
 *
 * Responsibilities:
 *
 * 1. Cache all shared spawn points.
 * 2. Choose a safe initial location for the player.
 * 3. Spawn enough AI to reach MaxCharacters.
 * 4. Track all currently living characters.
 * 5. Receive death notifications from UDeathComponent.
 * 6. Delay respawns after death.
 * 7. Retry failed respawns until a safe point becomes available.
 * 8. Prevent the total living population from exceeding MaxCharacters.
 */
UCLASS()
class TEMPLEFPS_API AFPSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	//=====================================================
	// Unreal Lifecycle
	//=====================================================

	AFPSGameMode();

	//=====================================================
	// Death Notifications
	//=====================================================

	/*
	 * Called directly by UDeathComponent when a character dies.
	 *
	 * The character is immediately removed from the living population.
	 * Its replacement becomes eligible after RespawnDelay.
	 */
	void HandleCharacterDeath(ABaseCharacter* DeadCharacter);

	//=====================================================
	// Configuration Getters
	//=====================================================

	/*
	 * Used by UDeathComponent to determine how long ragdolls remain.
	 */
	float GetCorpseLifetime() const
	{
		return CorpseLifetime;
	}

protected:

	//=====================================================
	// Unreal Lifecycle
	//=====================================================

	virtual void BeginPlay() override;

	//=====================================================
	// Initial Player Spawning
	//=====================================================

	/*
	 * Called by Unreal when the initial player needs a spawn location.
	 *
	 * This selects a location through the same safety system later used
	 * for player and AI respawning.
	 */
	virtual AActor* ChoosePlayerStart_Implementation(
		AController* Player
	) override;

	/*
	 * The parent implementation creates and possesses the initial player
	 * pawn. Afterward, this function registers that player as living and
	 * begins filling the remaining population with AI.
	 */
	virtual void HandleStartingNewPlayer_Implementation(
		APlayerController* NewPlayer
	) override;

private:

	//=====================================================
	// Initial Population
	//=====================================================

	/*
	 * Creates immediate AI-spawn requests until the desired initial
	 * population has been reached.
	 */
	void QueueInitialAIPopulation();

	//=====================================================
	// Spawn-Point Management
	//=====================================================

	/*
	 * Finds every AFPSCharacterSpawnPoint placed in the current world.
	 */
	void CacheCharacterSpawnPoints();

	/*
	 * Finds all currently safe spawn points for the supplied character
	 * class and randomly returns one of them.
	 */
	AFPSCharacterSpawnPoint* FindSafeSpawnPoint(
		TSubclassOf<ABaseCharacter> CharacterClass,
		bool bRequireHiddenFromLivingCharacters
	) const;
	/*
	 * Runs every safety condition for one spawn point.
	 */
	bool IsSpawnPointSafe(
		const AFPSCharacterSpawnPoint* SpawnPoint,
		TSubclassOf<ABaseCharacter> CharacterClass,
		bool bRequireHiddenFromLivingCharacters
	) const;

	/*
	 * Checks whether the new character capsule would overlap blocking
	 * geometry at this spawn point.
	 */
	bool IsSpawnCapsuleClear(
		const AFPSCharacterSpawnPoint* SpawnPoint,
		TSubclassOf<ABaseCharacter> CharacterClass
	) const;

	/*
	 * Checks whether every living character is at least
	 * MinimumSpawnDistance away.
	 */
	bool IsFarEnoughFromLivingCharacters(
		const AFPSCharacterSpawnPoint* SpawnPoint
	) const;

	/*
	 * Returns false if any living character has an unobstructed
	 * visibility trace to the spawn location.
	 */
	bool IsHiddenFromLivingCharacters(
		const AFPSCharacterSpawnPoint* SpawnPoint,
		TSubclassOf<ABaseCharacter> CharacterClass
	) const;

	//=====================================================
	// Player Spawning
	//=====================================================

	/*
	 * Attempts to respawn the existing PlayerController at a safe point.
	 *
	 * RestartPlayerAtPlayerStart creates the new default pawn and makes
	 * the controller possess it.
	 */
	bool TryRespawnPlayer();

	/*
	 * Releases the spawn point temporarily reserved by
	 * ChoosePlayerStart_Implementation.
	 */
	void ReleaseReservedPlayerSpawnPoint();

	/*
	 * Gets the configured DefaultPawnClass and confirms it derives from
	 * ABaseCharacter.
	 */
	TSubclassOf<ABaseCharacter> GetPlayerCharacterClass(
		AController* PlayerController
	);

	//=====================================================
	// AI Spawning
	//=====================================================

	/*
	 * Attempts to spawn one AI at one randomly selected safe spawn point.
	 */
	bool TrySpawnAICharacter();

	/*
	 * Randomly selects one valid AI class from the global GameMode list.
	 */
	TSubclassOf<ABaseAICharacter>
		SelectRandomAICharacterClass() const;

	//=====================================================
	// Respawn Scheduling
	//=====================================================

	/*
	 * Starts the shared respawn delay for the dead player.
	 */
	void SchedulePlayerRespawn(
		APlayerController* PlayerController
	);

	/*
	 * Starts one independent respawn-delay timer for an AI death.
	 */
	void ScheduleAIRespawn();

	/*
	 * Called after the player respawn delay finishes.
	 */
	void MarkPlayerRespawnReady();

	/*
	 * Called after one AI respawn delay finishes.
	 */
	void AddReadyAIRespawnRequest();

	/*
	 * Attempts all currently ready player and AI respawns.
	 *
	 * If no safe point exists, the requests remain queued.
	 */
	void TryProcessRespawnQueue();

	/*
	 * Starts a repeating retry timer when ready requests cannot currently
	 * find safe spawn points.
	 */
	void EnsureRespawnRetryTimer();

	/*
	 * Stops the retry timer once no ready requests remain.
	 */
	void ClearRespawnRetryTimerIfIdle();

	bool HasReadyRespawnRequests() const;

	//=====================================================
	// Population Tracking
	//=====================================================

	void RegisterLivingCharacter(
		ABaseCharacter* Character
	);

	void UnregisterLivingCharacter(
		ABaseCharacter* Character
	);

	void RemoveInvalidLivingCharacters();

	int32 GetLivingCharacterCount() const;

	int32 GetLivingAICharacterCount() const;

	/*
	 * The player permanently owns one population slot, even while dead
	 * and waiting to respawn.
	 *
	 * Example:
	 *
	 * MaxCharacters = 5
	 * Desired AI count = 4
	 */
	int32 GetDesiredAICharacterCount() const;

	//=====================================================
	// Spawn Configuration
	//=====================================================

	/*
	 * Total desired living population:
	 *
	 * Player + AI combined.
	 */
	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Spawning|Population",
		meta = (
			AllowPrivateAccess = "true",
			ClampMin = "1",
			UIMin = "1"
			)
	)
	int32 MaxCharacters = 5;

	/*
 * Maximum distance at which a living character can make a spawn
 * unsafe through direct line of sight.
 *
 * Characters farther away than this do not prevent spawning,
 * even when the geometry between them is unobstructed.
 */
	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Spawning|Safety",
		meta = (
			AllowPrivateAccess = "true",
			ClampMin = "0.0",
			UIMin = "0.0"
			)
	)
	float SpawnLineOfSightDangerDistance = 3000.f;

	/*
	 * Half-angle of the character's spawn-blocking vision cone.
	 *
	 * A value of 60 degrees creates a total 120-degree field of view:
	 *
	 * 60 degrees left
	 * 60 degrees right
	 */
	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Spawning|Safety",
		meta = (
			AllowPrivateAccess = "true",
			ClampMin = "0.0",
			ClampMax = "180.0",
			UIMin = "0.0",
			UIMax = "180.0"
			)
	)
	float SpawnLineOfSightHalfAngle = 60.f;

	/*
	 * Shared delay between a character dying and becoming eligible
	 * to respawn.
	 */
	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Spawning|Respawn",
		meta = (
			AllowPrivateAccess = "true",
			ClampMin = "0.0",
			UIMin = "0.0"
			)
	)
	float RespawnDelay = 5.f;

	/*
	 * How frequently ready respawns retry when no safe point exists.
	 */
	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Spawning|Respawn",
		meta = (
			AllowPrivateAccess = "true",
			ClampMin = "0.1",
			UIMin = "0.1"
			)
	)
	float RespawnRetryDelay = 1.f;

	/*
	 * How long dead ragdolls remain before their actor is destroyed.
	 */
	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Spawning|Respawn",
		meta = (
			AllowPrivateAccess = "true",
			ClampMin = "0.0",
			UIMin = "0.0"
			)
	)
	float CorpseLifetime = 10.f;

	/*
	 * Minimum distance required between a spawn point and every
	 * currently living character.
	 *
	 * Unreal uses centimeters, so 1000 units equals approximately
	 * 10 meters.
	 */
	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Spawning|Safety",
		meta = (
			AllowPrivateAccess = "true",
			ClampMin = "0.0",
			UIMin = "0.0"
			)
	)
	float MinimumSpawnDistance = 1000.f;

	/*
	 * Collision channel used to determine whether a living character
	 * can directly see a spawn point.
	 */
	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Spawning|Safety",
		meta = (AllowPrivateAccess = "true")
	)
	TEnumAsByte<ECollisionChannel> SpawnLineOfSightTraceChannel =
		ECC_Visibility;

	/*
	 * Global list of AI character classes.
	 *
	 * Every spawn point uses this same list.
	 */
	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Spawning|AI",
		meta = (AllowPrivateAccess = "true")
	)
	TArray<TSubclassOf<ABaseAICharacter>> AICharacterClasses;

	//=====================================================
	// Cached Runtime State
	//=====================================================

	UPROPERTY()
	TArray<TObjectPtr<AFPSCharacterSpawnPoint>> SpawnPoints;

	/*
	 * Contains only characters that are currently alive.
	 *
	 * Dead ragdolls are removed immediately when death is reported,
	 * even though their actors remain in the world temporarily.
	 */
	UPROPERTY()
	TSet<TObjectPtr<ABaseCharacter>> LivingCharacters;

	/*
	 * PlayerController that needs a replacement pawn.
	 */
	UPROPERTY()
	TObjectPtr<APlayerController> PendingPlayerController = nullptr;

	/*
	 * Spawn point temporarily reserved during the initial Unreal
	 * player-start flow.
	 */
	UPROPERTY()
	TObjectPtr<AFPSCharacterSpawnPoint>
		ReservedPlayerSpawnPoint = nullptr;

	/*
	 * Becomes true immediately when the player dies and remains true
	 * until the player successfully respawns.
	 *
	 * This ensures AI never consumes the player's population slot.
	 */
	bool bPlayerRespawnPending = false;

	/*
	 * Becomes true after RespawnDelay has completed.
	 */
	bool bPlayerRespawnReady = false;

	/*
	 * Number of AI deaths whose RespawnDelay has completed and are
	 * currently waiting for a safe spawn point.
	 */
	int32 ReadyAIRespawnRequests = 0;

	bool bInitialPopulationQueued = false;

	FTimerHandle PlayerRespawnDelayTimerHandle;
	FTimerHandle RespawnRetryTimerHandle;
};