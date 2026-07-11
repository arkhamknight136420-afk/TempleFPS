#include "FPSGameMode.h"

#include "EngineUtils.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "CollisionShape.h"

#include "GameFramework/PlayerController.h"
#include "Components/CapsuleComponent.h"

#include "../../Characters/BaseCharacter.h"
#include "../../AI/Characters/BaseAICharacter.h"
#include "../../Spawning/FPSCharacterSpawnPoint.h"
#include "DrawDebugHelpers.h"

AFPSGameMode::AFPSGameMode()
{
	/*
	 * Respawning uses timers, not Tick.
	 */
	PrimaryActorTick.bCanEverTick = false;
}

void AFPSGameMode::BeginPlay()
{
	Super::BeginPlay();

	CacheCharacterSpawnPoints();
}

//=====================================================
// Initial Player Spawning
//=====================================================

AActor* AFPSGameMode::ChoosePlayerStart_Implementation(
	AController* Player
)
{
	if (SpawnPoints.IsEmpty())
	{
		CacheCharacterSpawnPoints();
	}

	const TSubclassOf<ABaseCharacter> PlayerCharacterClass =
		GetPlayerCharacterClass(Player);

	AFPSCharacterSpawnPoint* SelectedSpawnPoint =
		FindSafeSpawnPoint(
			PlayerCharacterClass,
			false
		);

	if (!IsValid(SelectedSpawnPoint))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"[FPSGameMode] Could not find a safe custom "
				"spawn point for the player."
			)
		);

		return Super::ChoosePlayerStart_Implementation(Player);
	}

	if (!SelectedSpawnPoint->TryReserve())
	{
		return Super::ChoosePlayerStart_Implementation(Player);
	}

	/*
	 * Remember which point was reserved so it can be released after
	 * Unreal finishes spawning the player.
	 */
	ReservedPlayerSpawnPoint = SelectedSpawnPoint;

	return SelectedSpawnPoint;
}

void AFPSGameMode::HandleStartingNewPlayer_Implementation(
	APlayerController* NewPlayer
)
{
	/*
	 * Creates the initial player pawn and possesses it.
	 */
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);

	/*
	 * The spawn operation is finished, so the location may now be reused
	 * later, provided it passes all safety checks.
	 */
	ReleaseReservedPlayerSpawnPoint();

	if (!IsValid(NewPlayer))
	{
		return;
	}

	ABaseCharacter* PlayerCharacter =
		Cast<ABaseCharacter>(NewPlayer->GetPawn());

	if (!IsValid(PlayerCharacter))
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"[FPSGameMode] Initial player pawn was not a "
				"valid ABaseCharacter."
			)
		);

		return;
	}

	RegisterLivingCharacter(PlayerCharacter);

	if (!bInitialPopulationQueued)
	{
		QueueInitialAIPopulation();
		bInitialPopulationQueued = true;
	}
}

void AFPSGameMode::QueueInitialAIPopulation()
{
	RemoveInvalidLivingCharacters();

	const int32 MissingAICharacters =
		FMath::Max(
			0,
			GetDesiredAICharacterCount()
			- GetLivingAICharacterCount()
		);

	/*
	 * Initial population does not wait for RespawnDelay because these
	 * characters have not died. They are part of match setup.
	 */
	ReadyAIRespawnRequests += MissingAICharacters;

	TryProcessRespawnQueue();
}

//=====================================================
// Death Notifications
//=====================================================

void AFPSGameMode::HandleCharacterDeath(
	ABaseCharacter* DeadCharacter
)
{
	if (!IsValid(DeadCharacter))
	{
		return;
	}

	/*
	 * Capture the controller before UDeathComponent unpossesses it.
	 */
	AController* DeadController =
		DeadCharacter->GetController();

	UnregisterLivingCharacter(DeadCharacter);

	if (APlayerController* PlayerController =
		Cast<APlayerController>(DeadController))
	{
		SchedulePlayerRespawn(PlayerController);
		return;
	}

	if (DeadCharacter->IsA<ABaseAICharacter>())
	{
		ScheduleAIRespawn();
	}
}

//=====================================================
// Respawn Scheduling
//=====================================================

void AFPSGameMode::SchedulePlayerRespawn(
	APlayerController* PlayerController
)
{
	if (!IsValid(PlayerController))
	{
		return;
	}

	/*
	 * There can only be one player respawn request in this single-player
	 * system at a time.
	 */
	if (bPlayerRespawnPending)
	{
		return;
	}

	PendingPlayerController = PlayerController;
	bPlayerRespawnPending = true;
	bPlayerRespawnReady = false;

	if (RespawnDelay <= 0.f)
	{
		MarkPlayerRespawnReady();
		return;
	}

	GetWorldTimerManager().SetTimer(
		PlayerRespawnDelayTimerHandle,
		this,
		&AFPSGameMode::MarkPlayerRespawnReady,
		RespawnDelay,
		false
	);
}

void AFPSGameMode::ScheduleAIRespawn()
{
	if (RespawnDelay <= 0.f)
	{
		AddReadyAIRespawnRequest();
		return;
	}

	/*
	 * Each AI death receives its own one-shot timer.
	 *
	 * The local timer handle does not need to be stored because this
	 * timer never needs to be manually cancelled.
	 */
	FTimerHandle AIRespawnDelayTimerHandle;

	GetWorldTimerManager().SetTimer(
		AIRespawnDelayTimerHandle,
		this,
		&AFPSGameMode::AddReadyAIRespawnRequest,
		RespawnDelay,
		false
	);
}

void AFPSGameMode::MarkPlayerRespawnReady()
{
	if (!bPlayerRespawnPending)
	{
		return;
	}

	bPlayerRespawnReady = true;

	TryProcessRespawnQueue();
}

void AFPSGameMode::AddReadyAIRespawnRequest()
{
	++ReadyAIRespawnRequests;

	TryProcessRespawnQueue();
}

void AFPSGameMode::TryProcessRespawnQueue()
{
	RemoveInvalidLivingCharacters();

	//=====================================================
	// Player Respawn
	//=====================================================

	/*
	 * Player respawning receives priority because one population slot
	 * has been permanently reserved for the human player.
	 */
	if (bPlayerRespawnReady)
	{
		if (TryRespawnPlayer())
		{
			bPlayerRespawnReady = false;
			bPlayerRespawnPending = false;
			PendingPlayerController = nullptr;
		}
	}

	//=====================================================
	// AI Respawns
	//=====================================================

	const int32 MissingAICharacters =
		FMath::Max(
			0,
			GetDesiredAICharacterCount()
			- GetLivingAICharacterCount()
		);

	/*
	 * Do not retain more AI requests than the number of AI population
	 * slots that are actually missing.
	 */
	ReadyAIRespawnRequests =
		FMath::Min(
			ReadyAIRespawnRequests,
			MissingAICharacters
		);

	while (ReadyAIRespawnRequests > 0)
	{
		if (!TrySpawnAICharacter())
		{
			/*
			 * No safe spawn currently exists.
			 *
			 * Leave the request queued so the retry timer can attempt
			 * it again later.
			 */
			break;
		}

		--ReadyAIRespawnRequests;
	}

	if (HasReadyRespawnRequests())
	{
		EnsureRespawnRetryTimer();
	}
	else
	{
		ClearRespawnRetryTimerIfIdle();
	}
}

void AFPSGameMode::EnsureRespawnRetryTimer()
{
	if (GetWorldTimerManager().IsTimerActive(
		RespawnRetryTimerHandle
	))
	{
		return;
	}

	GetWorldTimerManager().SetTimer(
		RespawnRetryTimerHandle,
		this,
		&AFPSGameMode::TryProcessRespawnQueue,
		FMath::Max(RespawnRetryDelay, 0.1f),
		true
	);
}

void AFPSGameMode::ClearRespawnRetryTimerIfIdle()
{
	if (HasReadyRespawnRequests())
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(
		RespawnRetryTimerHandle
	);
}

bool AFPSGameMode::HasReadyRespawnRequests() const
{
	return bPlayerRespawnReady
		|| ReadyAIRespawnRequests > 0;
}

//=====================================================
// Player Respawning
//=====================================================

bool AFPSGameMode::TryRespawnPlayer()
{
	if (!IsValid(PendingPlayerController))
	{
		/*
		 * The controller no longer exists, so this request cannot ever
		 * succeed.
		 */
		bPlayerRespawnPending = false;
		bPlayerRespawnReady = false;
		PendingPlayerController = nullptr;

		return false;
	}

	/*
	 * A controller with a pawn does not currently need respawning.
	 */
	if (IsValid(PendingPlayerController->GetPawn()))
	{
		if (ABaseCharacter* ExistingCharacter =
			Cast<ABaseCharacter>(
				PendingPlayerController->GetPawn()
			))
		{
			RegisterLivingCharacter(ExistingCharacter);
		}

		return true;
	}

	const TSubclassOf<ABaseCharacter> PlayerCharacterClass =
		GetPlayerCharacterClass(PendingPlayerController);

	if (!PlayerCharacterClass)
	{
		return false;
	}

	AFPSCharacterSpawnPoint* SpawnPoint =
		FindSafeSpawnPoint(
			PlayerCharacterClass,
			true
		);

	if (!IsValid(SpawnPoint))
	{
		return false;
	}

	if (!SpawnPoint->TryReserve())
	{
		return false;
	}

	/*
	 * RestartPlayerAtPlayerStart:
	 *
	 * 1. Spawns the configured DefaultPawnClass.
	 * 2. Uses this exact spawn-point actor.
	 * 3. Makes PendingPlayerController possess the new pawn.
	 */
	RestartPlayerAtPlayerStart(
		PendingPlayerController,
		SpawnPoint
	);

	/*
	 * Reservation only protects the spawning operation.
	 */
	SpawnPoint->ReleaseReservation();

	ABaseCharacter* NewPlayerCharacter =
		Cast<ABaseCharacter>(
			PendingPlayerController->GetPawn()
		);

	if (!IsValid(NewPlayerCharacter))
	{
		return false;
	}

	/*
	 * Re-enable the controller and new pawn now that the player has
	 * successfully respawned.
	 */
	PendingPlayerController->EnableInput(
		PendingPlayerController
	);

	NewPlayerCharacter->EnableInput(
		PendingPlayerController
	);

	RegisterLivingCharacter(NewPlayerCharacter);

	return true;
}

void AFPSGameMode::ReleaseReservedPlayerSpawnPoint()
{
	if (!IsValid(ReservedPlayerSpawnPoint))
	{
		return;
	}

	ReservedPlayerSpawnPoint->ReleaseReservation();
	ReservedPlayerSpawnPoint = nullptr;
}

TSubclassOf<ABaseCharacter>
AFPSGameMode::GetPlayerCharacterClass(
	AController* PlayerController
) {
	if (!IsValid(PlayerController))
	{
		return nullptr;
	}

	UClass* PawnClass =
		GetDefaultPawnClassForController(PlayerController);

	if (!PawnClass)
	{
		return nullptr;
	}

	if (!PawnClass->IsChildOf(
		ABaseCharacter::StaticClass()
	))
	{
		return nullptr;
	}

	return PawnClass;
}

//=====================================================
// AI Spawning
//=====================================================

bool AFPSGameMode::TrySpawnAICharacter()
{
	RemoveInvalidLivingCharacters();

	if (
		GetLivingAICharacterCount()
		>= GetDesiredAICharacterCount()
		)
	{
		return false;
	}

	const TSubclassOf<ABaseAICharacter> AICharacterClass =
		SelectRandomAICharacterClass();

	if (!AICharacterClass)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"[FPSGameMode] No valid AI character classes "
				"were configured."
			)
		);

		return false;
	}

	const TSubclassOf<ABaseCharacter> BaseCharacterClass =
		AICharacterClass;

	AFPSCharacterSpawnPoint* SpawnPoint =
		FindSafeSpawnPoint(
			BaseCharacterClass,
			true
		);

	if (!IsValid(SpawnPoint))
	{
		return false;
	}

	if (!SpawnPoint->TryReserve())
	{
		return false;
	}

	FActorSpawnParameters SpawnParameters;

	/*
	 * The capsule test already checked this exact location.
	 *
	 * DontSpawnIfColliding adds a final defensive collision check.
	 */
	SpawnParameters.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::
		DontSpawnIfColliding;

	ABaseAICharacter* NewAICharacter =
		GetWorld()->SpawnActor<ABaseAICharacter>(
			AICharacterClass,
			SpawnPoint->GetActorTransform(),
			SpawnParameters
		);

	SpawnPoint->ReleaseReservation();

	if (!IsValid(NewAICharacter))
	{
		return false;
	}

	/*
	 * ABaseAICharacter currently uses
	 * PlacedInWorldOrSpawned, so this should usually already exist.
	 */
	if (!NewAICharacter->GetController())
	{
		NewAICharacter->SpawnDefaultController();
	}

	RegisterLivingCharacter(NewAICharacter);

	return true;
}

TSubclassOf<ABaseAICharacter>
AFPSGameMode::SelectRandomAICharacterClass() const
{
	TArray<TSubclassOf<ABaseAICharacter>> ValidClasses;

	for (
		const TSubclassOf<ABaseAICharacter>& AIClass
		: AICharacterClasses
		)
	{
		if (AIClass)
		{
			ValidClasses.Add(AIClass);
		}
	}

	if (ValidClasses.IsEmpty())
	{
		return nullptr;
	}

	const int32 RandomIndex =
		FMath::RandRange(
			0,
			ValidClasses.Num() - 1
		);

	return ValidClasses[RandomIndex];
}

//=====================================================
// Spawn-Point Management
//=====================================================

void AFPSGameMode::CacheCharacterSpawnPoints()
{
	SpawnPoints.Reset();

	UWorld* World = GetWorld();

	if (!World)
	{
		return;
	}

	for (
		TActorIterator<AFPSCharacterSpawnPoint>
		Iterator(World);
		Iterator;
		++Iterator
		)
	{
		AFPSCharacterSpawnPoint* SpawnPoint =
			*Iterator;

		if (IsValid(SpawnPoint))
		{
			SpawnPoints.Add(SpawnPoint);
		}
	}

	UE_LOG(
		LogTemp,
		Log,
		TEXT(
			"[FPSGameMode] Cached %d character spawn points."
		),
		SpawnPoints.Num()
	);
}

AFPSCharacterSpawnPoint*
AFPSGameMode::FindSafeSpawnPoint(
	TSubclassOf<ABaseCharacter> CharacterClass,
	bool bRequireHiddenFromLivingCharacters
) const
{
	if (!CharacterClass)
	{
		return nullptr;
	}

	TArray<AFPSCharacterSpawnPoint*> SafeSpawnPoints;

	for (AFPSCharacterSpawnPoint* SpawnPoint : SpawnPoints)
	{
		if (
			IsValid(SpawnPoint)
			&& IsSpawnPointSafe(
				SpawnPoint,
				CharacterClass,
				bRequireHiddenFromLivingCharacters
			)
			)
		{
			SafeSpawnPoints.Add(SpawnPoint);
		}
	}

	if (SafeSpawnPoints.IsEmpty())
	{
		return nullptr;
	}

	const int32 RandomIndex = FMath::RandRange(
		0,
		SafeSpawnPoints.Num() - 1
	);

	return SafeSpawnPoints[RandomIndex];
}

bool AFPSGameMode::IsSpawnPointSafe(
	const AFPSCharacterSpawnPoint* SpawnPoint,
	TSubclassOf<ABaseCharacter> CharacterClass,
	bool bRequireHiddenFromLivingCharacters
) const
{
	/*
	 * The spawn point and character class must both be valid.
	 */
	if (
		!IsValid(SpawnPoint)
		|| !CharacterClass
		)
	{
		return false;
	}

	/*
	 * Another spawning operation may have temporarily reserved
	 * this location.
	 */
	if (!SpawnPoint->IsAvailable())
	{
		return false;
	}

	/*
	 * Do not spawn too close to any currently living character.
	 */
	if (!IsFarEnoughFromLivingCharacters(SpawnPoint))
	{
		return false;
	}

	/*
	 * Make sure the new character's capsule will not overlap
	 * blocking geometry or another blocking actor.
	 */
	if (!IsSpawnCapsuleClear(
		SpawnPoint,
		CharacterClass
	))
	{
		return false;
	}

	/*
	 * The line-of-sight rule is optional.
	 *
	 * Initial match population can pass false so the game can fill
	 * the map without every newly spawned AI preventing the next one.
	 *
	 * Actual respawns should pass true so characters do not respawn
	 * directly within another living character's dangerous view.
	 */
	if (
		bRequireHiddenFromLivingCharacters
		&& !IsHiddenFromLivingCharacters(
			SpawnPoint,
			CharacterClass
		)
		)
	{
		return false;
	}

	/*
	 * Every required safety check passed.
	 */
	return true;
}
bool AFPSGameMode::IsFarEnoughFromLivingCharacters(
	const AFPSCharacterSpawnPoint* SpawnPoint
) const
{
	if (!IsValid(SpawnPoint))
	{
		return false;
	}

	UWorld* World = GetWorld();

	if (!World)
	{
		return false;
	}

	const FVector SpawnLocation =
		SpawnPoint->GetActorLocation();

	const float MinimumDistanceSquared =
		FMath::Square(MinimumSpawnDistance);

	for (
		const TObjectPtr<ABaseCharacter>& CharacterPointer
		: LivingCharacters
		)
	{
		ABaseCharacter* LivingCharacter =
			CharacterPointer.Get();

		if (!IsValid(LivingCharacter))
		{
			continue;
		}

		const FVector CharacterLocation =
			LivingCharacter->GetActorLocation();

		const float DistanceSquared =
			FVector::DistSquared(
				SpawnLocation,
				CharacterLocation
			);

		if (DistanceSquared < MinimumDistanceSquared)
		{
			/*
			 * Red sphere:
			 * This spawn point failed because a living character
			 * is inside the minimum spawn-distance radius.
			 */
			DrawDebugSphere(
				World,
				SpawnLocation,
				MinimumSpawnDistance,
				32,
				FColor::Red,
				false,
				1.1f,
				0,
				3.f
			);

			/*
			 * Draw a red line to the character that caused
			 * this spawn point to fail.
			 */
			DrawDebugLine(
				World,
				SpawnLocation,
				CharacterLocation,
				FColor::Red,
				false,
				1.1f,
				0,
				3.f
			);

			return false;
		}
	}

	/*
	 * Green sphere:
	 * No living character was found inside the required distance.
	 */
	DrawDebugSphere(
		World,
		SpawnLocation,
		MinimumSpawnDistance,
		32,
		FColor::Green,
		false,
		1.1f,
		0,
		3.f
	);

	return true;
}

bool AFPSGameMode::IsSpawnCapsuleClear(
	const AFPSCharacterSpawnPoint* SpawnPoint,
	TSubclassOf<ABaseCharacter> CharacterClass
) const
{
	UWorld* World = GetWorld();

	if (
		!World
		|| !IsValid(SpawnPoint)
		|| !CharacterClass
		)
	{
		return false;
	}

	/*
	 * The Class Default Object contains the default capsule dimensions
	 * configured for this character class without spawning an instance.
	 */
	const ABaseCharacter* CharacterDefaultObject =
		CharacterClass.GetDefaultObject();

	if (!IsValid(CharacterDefaultObject))
	{
		return false;
	}

	const UCapsuleComponent* Capsule =
		CharacterDefaultObject->GetCapsuleComponent();

	if (!IsValid(Capsule))
	{
		return false;
	}

	const FCollisionShape CapsuleShape =
		FCollisionShape::MakeCapsule(
			Capsule->GetUnscaledCapsuleRadius(),
			Capsule->GetUnscaledCapsuleHalfHeight()
		);

	FCollisionQueryParams QueryParameters(
		SCENE_QUERY_STAT(CharacterSpawnCapsuleCheck),
		false
	);

	QueryParameters.AddIgnoredActor(SpawnPoint);

	const bool bHasBlockingOverlap =
		World->OverlapBlockingTestByProfile(
			SpawnPoint->GetActorLocation(),
			SpawnPoint->GetActorQuat(),
			Capsule->GetCollisionProfileName(),
			CapsuleShape,
			QueryParameters
		);

	return !bHasBlockingOverlap;
}

bool AFPSGameMode::IsHiddenFromLivingCharacters(
	const AFPSCharacterSpawnPoint* SpawnPoint,
	TSubclassOf<ABaseCharacter> CharacterClass
) const
{
	UWorld* World = GetWorld();

	if (
		!World
		|| !IsValid(SpawnPoint)
		|| !CharacterClass
		)
	{
		return false;
	}

	const ABaseCharacter* CharacterDefaultObject =
		CharacterClass.GetDefaultObject();

	if (!IsValid(CharacterDefaultObject))
	{
		return false;
	}

	const UCapsuleComponent* Capsule =
		CharacterDefaultObject->GetCapsuleComponent();

	if (!IsValid(Capsule))
	{
		return false;
	}

	/*
	 * Trace toward approximately chest/head height instead of tracing
	 * directly toward the floor beneath the spawn point.
	 */
	const FVector SpawnSightLocation =
		SpawnPoint->GetActorLocation()
		+ FVector(
			0.f,
			0.f,
			Capsule->GetUnscaledCapsuleHalfHeight()
			* 0.75f
		);

	const float MaximumDangerDistanceSquared =
		FMath::Square(SpawnLineOfSightDangerDistance);

	/*
	 * Convert the editable angle into the minimum acceptable dot product.
	 *
	 * At 60 degrees:
	 *
	 * cos(60) = 0.5
	 *
	 * Therefore, the character only blocks the spawn when the direction
	 * to the spawn has a dot product of at least 0.5 with the direction
	 * the character is facing.
	 */
	const float MinimumViewDot =
		FMath::Cos(
			FMath::DegreesToRadians(
				SpawnLineOfSightHalfAngle
			)
		);

	for (
		const TObjectPtr<ABaseCharacter>& CharacterPointer
		: LivingCharacters
		)
	{
		ABaseCharacter* LivingCharacter =
			CharacterPointer.Get();

		if (!IsValid(LivingCharacter))
		{
			continue;
		}

		FVector ViewerLocation;
		FRotator ViewerRotation;

		/*
		 * Get the character's eye location and viewing rotation.
		 *
		 * This is better than using the actor's feet and raw actor
		 * rotation for a visibility test.
		 */
		LivingCharacter->GetActorEyesViewPoint(
			ViewerLocation,
			ViewerRotation
		);

		const FVector ViewerToSpawn =
			SpawnSightLocation - ViewerLocation;

		const float DistanceSquared =
			ViewerToSpawn.SizeSquared();

		/*
		 * This character is too far away to create an immediate
		 * spawn-kill threat.
		 */
		if (
			DistanceSquared
			> MaximumDangerDistanceSquared
			)
		{
			continue;
		}

		const FVector DirectionToSpawn =
			ViewerToSpawn.GetSafeNormal();

		const FVector ViewerForwardDirection =
			ViewerRotation.Vector();

		const float ViewDot =
			FVector::DotProduct(
				ViewerForwardDirection,
				DirectionToSpawn
			);

		/*
		 * The spawn is outside this character's field of view.
		 *
		 * Even if no wall exists between them, the character is not
		 * currently looking toward the spawn.
		 */
		if (ViewDot < MinimumViewDot)
		{
			continue;
		}

		FCollisionQueryParams QueryParameters(
			SCENE_QUERY_STAT(
				CharacterSpawnVisibilityCheck
			),
			true
		);

		QueryParameters.AddIgnoredActor(
			LivingCharacter
		);

		QueryParameters.AddIgnoredActor(
			SpawnPoint
		);

		FHitResult HitResult;

		const bool bHitBlockingObject =
			World->LineTraceSingleByChannel(
				HitResult,
				ViewerLocation,
				SpawnSightLocation,
				SpawnLineOfSightTraceChannel,
				QueryParameters
			);

		/*
		 * The character:
		 *
		 * 1. Is within the danger distance.
		 * 2. Is facing toward the spawn.
		 * 3. Has no wall or other blocking object between them.
		 *
		 * Therefore, this spawn point is unsafe.
		 */
		if (!bHitBlockingObject)
		{
			return false;
		}
	}

	/*
	 * No living character had a dangerous view of the spawn point.
	 */
	return true;
}

//=====================================================
// Population Tracking
//=====================================================

void AFPSGameMode::RegisterLivingCharacter(
	ABaseCharacter* Character
)
{
	if (!IsValid(Character))
	{
		return;
	}

	LivingCharacters.Add(Character);
}

void AFPSGameMode::UnregisterLivingCharacter(
	ABaseCharacter* Character
)
{
	if (!Character)
	{
		return;
	}

	LivingCharacters.Remove(Character);
}

void AFPSGameMode::RemoveInvalidLivingCharacters()
{
	for (
		auto Iterator =
		LivingCharacters.CreateIterator();
		Iterator;
		++Iterator
		)
	{
		if (!IsValid(*Iterator))
		{
			Iterator.RemoveCurrent();
		}
	}
}

int32 AFPSGameMode::GetLivingCharacterCount() const
{
	int32 Count = 0;

	for (
		const TObjectPtr<ABaseCharacter>& Character
		: LivingCharacters
		)
	{
		if (IsValid(Character))
		{
			++Count;
		}
	}

	return Count;
}

int32 AFPSGameMode::GetLivingAICharacterCount() const
{
	int32 Count = 0;

	for (
		const TObjectPtr<ABaseCharacter>& Character
		: LivingCharacters
		)
	{
		if (
			IsValid(Character)
			&& Character->IsA<ABaseAICharacter>()
			)
		{
			++Count;
		}
	}

	return Count;
}

int32 AFPSGameMode::GetDesiredAICharacterCount() const
{
	/*
	 * One permanent slot belongs to the player.
	 */
	return FMath::Max(
		0,
		MaxCharacters - 1
	);
}