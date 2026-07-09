#include "BTTask_HandleCombatMovement.h"

#include "../Characters/BaseAICharacter.h"
#include "../Controllers/BaseAIController.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"

UBTTask_HandleCombatMovement::UBTTask_HandleCombatMovement()
{
	NodeName = TEXT("Handle Combat Movement");

	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_HandleCombatMovement::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory
)
{
	ABaseAIController* AIController = GetBaseAIController(OwnerComp);
	ABaseAICharacter* AICharacter = GetAICharacter(OwnerComp);
	ACharacter* CurrentTarget = GetCurrentPlayer(OwnerComp);

	if (!IsValid(AIController) || !IsValid(AICharacter) || !IsValid(CurrentTarget))
	{
		return EBTNodeResult::Failed;
	}

	ChooseRandomStrafeDirection();

	return EBTNodeResult::InProgress;
}

void UBTTask_HandleCombatMovement::TickTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory,
	float DeltaSeconds
)
{
	ABaseAIController* AIController = GetBaseAIController(OwnerComp);
	ABaseAICharacter* AICharacter = GetAICharacter(OwnerComp);
	ACharacter* CurrentTarget = GetCurrentPlayer(OwnerComp);

	if (!IsValid(AIController) || !IsValid(AICharacter) || !IsValid(CurrentTarget))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	StrafeDecisionTimer -= DeltaSeconds;

	if (StrafeDecisionTimer <= 0.f)
	{
		ChooseRandomStrafeDirection();
	}

	const float DistanceToEnemy = GetDistanceToEnemy(AICharacter, CurrentTarget);

	if (DistanceToEnemy < AICharacter->RetreatStartDistance)
	{
		// Too close: move backward. If blocked, strafe.
		if (!TryMoveWithInput(AICharacter, FVector2D(0.f, -1.f)))
		{
			HandleFallbackStrafeMovement(AICharacter);
		}

		UE_LOG(
			LogTemp,
			Log,
			TEXT("[UBTTask_HandleCombatMovement] Too close. Moving away.")
		);
	}
	else if (DistanceToEnemy > AICharacter->AdvanceStartDistance)
	{
		// Too far: move forward. If blocked, strafe.
		if (!TryMoveWithInput(AICharacter, FVector2D(0.f, 1.f)))
		{
			HandleFallbackStrafeMovement(AICharacter);
		}

		UE_LOG(
			LogTemp,
			Log,
			TEXT("[UBTTask_HandleCombatMovement] Too far. Moving closer.")
		);
	}
	else
	{
		// Good combat range: strafe left/right.
		HandleFallbackStrafeMovement(AICharacter);

		UE_LOG(
			LogTemp,
			Log,
			TEXT("[UBTTask_HandleCombatMovement] In combat range. Strafing.")
		);
	}
}

ACharacter* UBTTask_HandleCombatMovement::GetCurrentPlayer(
	UBehaviorTreeComponent& OwnerComp
) const
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();

	if (!BlackboardComponent)
	{
		return nullptr;
	}

	UObject* CurrentPlayerKey =
		BlackboardComponent->GetValueAsObject(TEXT("Player"));

	return Cast<ACharacter>(CurrentPlayerKey);
}

ABaseAICharacter* UBTTask_HandleCombatMovement::GetAICharacter(
	UBehaviorTreeComponent& OwnerComp
) const
{
	ABaseAIController* AIController = GetBaseAIController(OwnerComp);

	if (!IsValid(AIController))
	{
		return nullptr;
	}

	return Cast<ABaseAICharacter>(AIController->GetPawn());
}

ABaseAIController* UBTTask_HandleCombatMovement::GetBaseAIController(
	UBehaviorTreeComponent& OwnerComp
) const
{
	return Cast<ABaseAIController>(OwnerComp.GetAIOwner());
}

float UBTTask_HandleCombatMovement::GetDistanceToEnemy(
	ABaseAICharacter* AICharacter,
	ACharacter* CurrentTarget
) const
{
	if (!IsValid(AICharacter) || !IsValid(CurrentTarget))
	{
		return 0.f;
	}

	return FVector::Dist2D(
		AICharacter->GetActorLocation(),
		CurrentTarget->GetActorLocation()
	);
}

bool UBTTask_HandleCombatMovement::TryMoveWithInput(
	ABaseAICharacter* AICharacter,
	const FVector2D& MoveInput
) const
{
	if (!CanMoveWithInput(AICharacter, MoveInput))
	{
		return false;
	}

	AICharacter->HandleDirectionalMovement(MoveInput);
	return true;
}

void UBTTask_HandleCombatMovement::HandleFallbackStrafeMovement(
	ABaseAICharacter* AICharacter
)
{
	const FVector2D DesiredStrafeInput(CurrentStrafeDirection, 0.f);

	if (TryMoveWithInput(AICharacter, DesiredStrafeInput))
	{
		return;
	}

	// If chosen side is blocked, flip and try the other side.
	CurrentStrafeDirection *= -1.f;

	const FVector2D OppositeStrafeInput(CurrentStrafeDirection, 0.f);

	TryMoveWithInput(AICharacter, OppositeStrafeInput);
}

void UBTTask_HandleCombatMovement::ChooseRandomStrafeDirection()
{
	CurrentStrafeDirection = FMath::RandBool() ? 1.f : -1.f;

	StrafeDecisionTimer = FMath::RandRange(
		MinStrafeDecisionTime,
		MaxStrafeDecisionTime
	);
}

bool UBTTask_HandleCombatMovement::CanMoveWithInput(
	ABaseAICharacter* AICharacter,
	const FVector2D& MoveInput
) const
{
	if (!IsValid(AICharacter))
	{
		return false;
	}

	const FVector MoveDirection =
		(AICharacter->GetActorForwardVector() * MoveInput.Y) +
		(AICharacter->GetActorRightVector() * MoveInput.X);

	if (MoveDirection.IsNearlyZero())
	{
		return false;
	}

	UCapsuleComponent* CapsuleComponent = AICharacter->GetCapsuleComponent();

	if (!CapsuleComponent)
	{
		return false;
	}

	const FVector Start = AICharacter->GetActorLocation();

	const FVector End =
		Start + MoveDirection.GetSafeNormal() * MovementCheckDistance;

	FHitResult HitResult;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(AICharacter);

	const bool bHit = AICharacter->GetWorld()->SweepSingleByChannel(
		HitResult,
		Start,
		End,
		FQuat::Identity,
		ECC_WorldStatic,
		FCollisionShape::MakeCapsule(
			CapsuleComponent->GetScaledCapsuleRadius(),
			CapsuleComponent->GetScaledCapsuleHalfHeight()
		),
		QueryParams
	);

	return !bHit;
}