// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_AttackPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "GameFramework/Pawn.h"



UBTTask_AttackPlayer::UBTTask_AttackPlayer()
{
	NodeName = TEXT("Attack Player");

	bNotifyTick = true;
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

	
	return EBTNodeResult::InProgress;
}

void UBTTask_AttackPlayer::TickTask(
	UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory,
	float DeltaSeconds
)
{
	FVector PlayerLoc = Player->GetActorLocation();

	AAIController* AIController = OwnerComp.GetAIOwner();

	if (!AIController)
	{
		FinishLatentTask(
			OwnerComp,
			EBTNodeResult::Failed
		);

		return;
	}

	APawn* AIPawn = AIController->GetPawn();

	if (!AIPawn)
	{
		FinishLatentTask(
			OwnerComp,
			EBTNodeResult::Failed
		);

		return;
	}

	FVector AICharacterLoc = AIPawn->GetActorLocation();


}