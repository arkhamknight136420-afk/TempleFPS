#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FPSBrainComponent.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(LogFPSBrain, Log, All); 
DECLARE_LOG_CATEGORY_EXTERN(LogMovementState, Log, All);



class AFPSPlayerCharacter;
class UBaseMovementState;
class UJumpState;
class UMantleState;
class USprintState;
class UCrouchState;
class UProneState;
class USlideState;
class UStandState;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TEMPLEFPS_API UFPSBrainComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFPSBrainComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(
		float DeltaTime,
		ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction
	) override;

	// Owner refs
	UPROPERTY(BlueprintReadOnly, Category = "Brain")
	AFPSPlayerCharacter* PlayerCharacterRef = nullptr;


	UBaseMovementState* CurrentMovementState = nullptr;
	UBaseMovementState* PreviousMovementState = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Input")
	FVector2D CurrentMoveInput = FVector2D::ZeroVector;

	bool BufferingSlowWalk = false;

	bool BufferingCrouch = false;


	void HandleMoveInputChanged(const FVector2D& MoveInput);

	bool IsCurrentState(TSubclassOf<UBaseMovementState> StateClass) const;

	bool TryIdle();

	bool CanIdle();

	bool TryWalk();

	bool CanWalk();

	bool TrySlowWalk();

	bool StopSlowWalk();

	bool CanSlowWalk();

	bool TryJump();

	bool CanJump();

	void HandleJumpLanding();

	bool TryCrouch();

	bool CanCrouch();

	bool TryIdleFromCrouch();

	bool TryWalkFromCrouch();	

	bool StopCrouch();

	bool IsCrouching() const;

	bool IsSlowWalking() const;


	void ChangeMovementState(UBaseMovementState* NewState);

private:

	void SetMoveInput(const FVector2D& MoveInput);

};