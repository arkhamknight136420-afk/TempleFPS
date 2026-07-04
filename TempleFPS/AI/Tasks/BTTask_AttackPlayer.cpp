#include "BTTask_AttackPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"
#include "../Controllers/BaseAIController.h"
#include "../Characters/BaseAICharacter.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_AttackPlayer::UBTTask_AttackPlayer()
{
	NodeName = TEXT("Attack Player");

	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_AttackPlayer::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory
)
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();

	if (!BlackboardComponent)
	{
		return EBTNodeResult::Failed;
	}

	UObject* PlayerBlackboardKey =
		BlackboardComponent->GetValueAsObject(TEXT("Player"));

	ACharacter* CurrentPlayer = Cast<ACharacter>(PlayerBlackboardKey);

	if (!IsValid(CurrentPlayer))
	{
		return EBTNodeResult::Failed;
	}

	ABaseAIController* AIController =
		Cast<ABaseAIController>(OwnerComp.GetAIOwner());

	if (!IsValid(AIController))
	{
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_AttackPlayer::TickTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory,
	float DeltaSeconds
)
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();

	if (!BlackboardComponent)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	ABaseAIController* AIController =
		Cast<ABaseAIController>(OwnerComp.GetAIOwner());

	if (!IsValid(AIController))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	ABaseAICharacter* AICharacter =
		Cast<ABaseAICharacter>(AIController->GetPawn());

	if (!IsValid(AICharacter))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	UObject* CurrentPlayerKey =
		BlackboardComponent->GetValueAsObject(TEXT("Player"));

	ACharacter* CurrentPlayer = Cast<ACharacter>(CurrentPlayerKey);

	if (!IsValid(CurrentPlayer))
	{
		AICharacter->StopShooting();

		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}
	AICharacter->UpdateEyePitch(CurrentPlayer);
	AIController->YawFocusOnTarget(CurrentPlayer, DeltaSeconds);


	FRotator LookAtRot =
		UKismetMathLibrary::FindLookAtRotation(
			AICharacter->GetActorLocation(),
			CurrentPlayer->GetActorLocation()
		);

	FRotator CurrentRot = AICharacter->GetActorRotation();

	FRotator DeltaRot =
		UKismetMathLibrary::NormalizedDeltaRotator(
			LookAtRot,
			CurrentRot
		);

	float AbsYawDelta = FMath::Abs(DeltaRot.Yaw);

	if (AbsYawDelta <= LookAtYawTolerance)
	{
		AICharacter->StartShooting();
	}
	else
	{
		AICharacter->StopShooting();
	}
}