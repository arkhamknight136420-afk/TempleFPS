// Fill out your copyright notice in the Description page of Project Settings.


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

	UBlackboardComponent* BlackboardComponent =
		OwnerComp.GetBlackboardComponent();

	if (!BlackboardComponent)
	{
		return EBTNodeResult::Failed;
	}

	UObject* PlayerBlackboardKey = BlackboardComponent->GetValueAsObject(TEXT("Player"));

	if (!PlayerBlackboardKey)
	{
		return EBTNodeResult::Failed;
		
	}
	
	Player = Cast<ACharacter>(PlayerBlackboardKey);

	



	if (!IsValid(Player))
	{
		return EBTNodeResult::Failed;
	}

	ABaseAIController* AIController = Cast<ABaseAIController>(OwnerComp.GetAIOwner());

	if (!IsValid(AIController))
	{
		return EBTNodeResult::Failed;
	}

	AIController->FocusOnTarget(Player);
	
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

	UObject* CurrentPlayerKey =
		BlackboardComponent->GetValueAsObject(TEXT("Player"));

	if (!IsValid(CurrentPlayerKey))
	{
		if (ABaseAIController* AIController = Cast<ABaseAIController>(OwnerComp.GetAIOwner()))
		{
			AIController->UnfocusOnTarget();
		}

		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	if (ABaseAICharacter* AICharacter = Cast<ABaseAICharacter>(OwnerComp.GetAIOwner()->GetPawn()))
	{
		FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(AICharacter->GetActorLocation(), Player->GetActorLocation());
		FRotator CurrentRot = AICharacter->GetActorRotation();

		FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(LookAtRot, CurrentRot);

		float AbsYawDelta = FMath::Abs(DeltaRot.Yaw);

		if (AbsYawDelta <= LookAtYawTolerance)
		{
			AICharacter->StartShooting();
			return;
		}
		else
		{
			AICharacter->StopShooting();
			return;
		}
	}



	


}