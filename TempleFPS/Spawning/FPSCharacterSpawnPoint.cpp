#include "FPSCharacterSpawnPoint.h"
#include "Components/DrawSphereComponent.h"
#include "EngineUtils.h"

AFPSCharacterSpawnPoint::AFPSCharacterSpawnPoint(
	const FObjectInitializer& ObjectInitializer
)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	/*
	 * Create a wireframe sphere that is visible in the editor.
	 */
	SpawnSpacingVisualizer =
		CreateDefaultSubobject<UDrawSphereComponent>(
			TEXT("SpawnSpacingVisualizer")
		);

	SpawnSpacingVisualizer->SetupAttachment(RootComponent);

	/*
	 * This is visual only. It must not affect gameplay collision.
	 */
	SpawnSpacingVisualizer->SetCollisionEnabled(
		ECollisionEnabled::NoCollision
	);

	/*
	 * Do not show the sphere while actually playing the game.
	 */
	SpawnSpacingVisualizer->SetHiddenInGame(true);

	/*
	 * Green means the point currently has acceptable spacing.
	 */
	SpawnSpacingVisualizer->ShapeColor = FColor::Green;
}

void AFPSCharacterSpawnPoint::OnConstruction(
	const FTransform& Transform
)
{
	Super::OnConstruction(Transform);

	if (!SpawnSpacingVisualizer)
	{
		return;
	}

	/*
	 * Each spawn point displays half the required spacing distance.
	 *
	 * Example:
	 *
	 * Minimum spacing = 1000
	 * Sphere radius   = 500
	 *
	 * When two spheres overlap, their centers are less than
	 * 1000 units apart.
	 */
	const float VisualizationRadius =
		MinimumSpawnPointSpacing * 0.5f;

	SpawnSpacingVisualizer->SetSphereRadius(
		VisualizationRadius
	);

	bool bIsTooCloseToAnotherSpawnPoint = false;

	UWorld* World = GetWorld();

	if (World)
	{
		const float MinimumSpacingSquared =
			FMath::Square(MinimumSpawnPointSpacing);

		/*
		 * Search for every other character spawn point in the
		 * current editor world.
		 */
		for (
			TActorIterator<AFPSCharacterSpawnPoint> Iterator(World);
			Iterator;
			++Iterator
			)
		{
			AFPSCharacterSpawnPoint* OtherSpawnPoint =
				*Iterator;

			/*
			 * Do not compare this spawn point against itself.
			 */
			if (
				!IsValid(OtherSpawnPoint)
				|| OtherSpawnPoint == this
				)
			{
				continue;
			}

			const float DistanceSquared =
				FVector::DistSquared(
					GetActorLocation(),
					OtherSpawnPoint->GetActorLocation()
				);

			/*
			 * If another spawn point is inside the configured
			 * minimum distance, this point is too close.
			 */
			if (DistanceSquared < MinimumSpacingSquared)
			{
				bIsTooCloseToAnotherSpawnPoint = true;
				break;
			}
		}
	}

	/*
	 * Red means another spawn point is too close.
	 * Green means spacing is currently acceptable.
	 */
	SpawnSpacingVisualizer->ShapeColor =
		bIsTooCloseToAnotherSpawnPoint
		? FColor::Red
		: FColor::Green;

	/*
	 * Force the editor visualization to refresh immediately.
	 */
	SpawnSpacingVisualizer->MarkRenderStateDirty();
}

bool AFPSCharacterSpawnPoint::IsAvailable() const
{
	return !bIsReserved;
}

bool AFPSCharacterSpawnPoint::TryReserve()
{
	if (!IsAvailable())
	{
		return false;
	}

	bIsReserved = true;

	return true;
}

void AFPSCharacterSpawnPoint::ReleaseReservation()
{
	bIsReserved = false;
}