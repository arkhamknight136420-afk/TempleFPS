#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "FPSCharacterSpawnPoint.generated.h"

/*
 * A shared spawn location that may be used by either:
 *
 * - The human player
 * - An AI character
 *
 * The spawn point does not decide what type of character spawns.
 * AFPSGameMode makes that decision.
 *
 * The only state this class tracks is whether another spawning
 * operation has temporarily reserved this location.
 */

class UDrawSphereComponent;
UCLASS()
class TEMPLEFPS_API AFPSCharacterSpawnPoint : public APlayerStart
{
	GENERATED_BODY()

public:

	//=====================================================
	// Unreal Lifecycle
	//=====================================================

	/*
	 * APlayerStart requires the FObjectInitializer constructor.
	 */
	AFPSCharacterSpawnPoint(
		const FObjectInitializer& ObjectInitializer
	);

	//=====================================================
	// Reservation
	//=====================================================

	/*
	 * Returns true when no spawning operation currently owns this point.
	 *
	 * This does not check distance, collision, or line of sight.
	 * Those are match-level rules handled by AFPSGameMode.
	 */
	bool IsAvailable() const;

	/*
	 * Attempts to temporarily reserve this point.
	 *
	 * Returns true if this call successfully reserved it.
	 * Returns false if something else already reserved it.
	 */
	bool TryReserve();

	/*
	 * Releases the temporary reservation after the spawning attempt
	 * finishes, regardless of whether spawning succeeded or failed.
	 */
	void ReleaseReservation();

protected:
	virtual void OnConstruction(
		const FTransform& Transform
	) override;



private:

	//=====================================================
	// Runtime State
	//=====================================================

	/*
	 * True only while a player or AI spawning operation is actively
	 * using this point.
	 *
	 * This should not remain true for the entire life of the character.
	 */
	UPROPERTY(
		VisibleInstanceOnly,
		BlueprintReadOnly,
		Category = "Spawning|Runtime",
		meta = (AllowPrivateAccess = "true")
	)
	bool bIsReserved = false;

	/*
 * Desired minimum center-to-center distance between two
 * character spawn points.
 *
 * Each visual sphere uses half this value as its radius,
 * so overlapping spheres indicate spawn points are too close.
 */
	UPROPERTY(
		EditAnywhere,
		BlueprintReadOnly,
		Category = "Spawning|Placement",
		meta = (
			AllowPrivateAccess = "true",
			ClampMin = "0.0",
			UIMin = "0.0"
			)
	)
	float MinimumSpawnPointSpacing = 1000.f;

	/*
	 * Editor-visible wire sphere used to visualize spacing.
	 *
	 * This component has no collision and is hidden during gameplay.
	 */
	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Spawning|Placement",
		meta = (AllowPrivateAccess = "true")
	)
	TObjectPtr<UDrawSphereComponent> SpawnSpacingVisualizer;
};