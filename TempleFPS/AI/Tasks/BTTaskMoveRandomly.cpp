#include "BTTaskMoveRandomly.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"

EBTNodeResult::Type UBTTaskMoveRandomly::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory
)
{
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (!AIController)
	{
		return EBTNodeResult::Failed;
	}

	APawn* ControlledPawn = AIController->GetPawn();

	if (!ControlledPawn)
	{
		return EBTNodeResult::Failed;
	}

	UNavigationSystemV1* NavigationSystem =
		UNavigationSystemV1::GetCurrent(GetWorld());

	if (!NavigationSystem)
	{
		return EBTNodeResult::Failed;
	}

	FNavLocation RandomLocation;

	const bool bFoundLocation =
		NavigationSystem->GetRandomReachablePointInRadius(
			ControlledPawn->GetActorLocation(),
			10000.f,
			RandomLocation
		);

	if (!bFoundLocation)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BlackboardComponent =
		OwnerComp.GetBlackboardComponent();

	if (!BlackboardComponent)
	{
		return EBTNodeResult::Failed;
	}

	BlackboardComponent->SetValueAsVector(
		TEXT("MoveLocation"),
		RandomLocation.Location
	);

	return EBTNodeResult::Succeeded;
}