#include "BTTask_AttackPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "../Characters/BaseAICharacter.h"
#include "../Controllers/BaseAIController.h"
#include "../../ActorComponents/HealthComponent.h"


UBTTask_AttackPlayer::UBTTask_AttackPlayer()
{
	NodeName = TEXT("Attack Player");

	bNotifyTick = true;
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_AttackPlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ABaseAIController* AIController = GetBaseAIController(OwnerComp);

	ACharacter* CurrentTarget = GetCombatTarget(OwnerComp);

	bool IsVisible = GetCanSeeTarget(OwnerComp);


	if (!IsValid(AIController) || !IsValid(CurrentTarget) || !IsVisible)
	{
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_AttackPlayer::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	ABaseAIController* AIController = GetBaseAIController(OwnerComp);
	ABaseAICharacter* AICharacter = GetAICharacter(OwnerComp);
	ACharacter* CurrentTarget = GetCombatTarget(OwnerComp);
	UHealthComponent* CurrentTargetHealthComponent = GetCurrentTargetHealthComponent(CurrentTarget);
	bool IsVisible = GetCanSeeTarget(OwnerComp);

	if (!IsValid(AIController) || !IsValid(AICharacter) || !IsValid(CurrentTarget) || !IsValid(CurrentTargetHealthComponent) || CurrentTargetHealthComponent->IsDead() || !IsVisible)
	{
		if (IsValid(AICharacter))
		{
			AICharacter->SetAimTarget(nullptr);
			AICharacter->StopShooting();
		}

		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	AICharacter->SetAimTarget(CurrentTarget);

	UpdateAimAtTarget(AIController, AICharacter, CurrentTarget, DeltaSeconds);
	UpdateShootingState(AICharacter, CurrentTarget);
}

ACharacter* UBTTask_AttackPlayer::GetCombatTarget(UBehaviorTreeComponent& OwnerComp) const
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();

	if (!BlackboardComponent)
	{
		return nullptr;
	}

	UObject* CurrentPlayerKey = BlackboardComponent->GetValueAsObject(TEXT("CombatTarget"));

	return Cast<ACharacter>(CurrentPlayerKey);
}

bool UBTTask_AttackPlayer::GetCanSeeTarget(UBehaviorTreeComponent& OwnerComp) const
{
	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();

	if (!BlackboardComponent)
	{
		return false;
	}
	return BlackboardComponent->GetValueAsBool(TEXT("CanSeeTarget"));


}


ABaseAIController* UBTTask_AttackPlayer::GetBaseAIController(UBehaviorTreeComponent& OwnerComp) const
{
	return Cast<ABaseAIController>(OwnerComp.GetAIOwner());
}

ABaseAICharacter* UBTTask_AttackPlayer::GetAICharacter(UBehaviorTreeComponent& OwnerComp) const
{
	ABaseAIController* AIController = GetBaseAIController(OwnerComp);

	if (!IsValid(AIController))
	{
		return nullptr;
	}

	return Cast<ABaseAICharacter>(AIController->GetPawn());
}

UHealthComponent* UBTTask_AttackPlayer::GetCurrentTargetHealthComponent(ACharacter* CurrentTarget) const
{
	if (!IsValid(CurrentTarget))
	{
		return nullptr;
	}

	return CurrentTarget->FindComponentByClass<UHealthComponent>();
}

float UBTTask_AttackPlayer::GetYawDeltaToTarget(ABaseAICharacter* AICharacter, ACharacter* CurrentTarget) const
{
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(AICharacter->GetActorLocation(), CurrentTarget->GetActorLocation());

	FRotator CurrentRot = AICharacter->GetActorRotation();

	FRotator DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(LookAtRot, CurrentRot);

	return FMath::Abs(DeltaRot.Yaw);
}

bool UBTTask_AttackPlayer::IsFacingTarget(ABaseAICharacter* AICharacter, ACharacter* CurrentTarget) const
{
	return GetYawDeltaToTarget(AICharacter, CurrentTarget) <= LookAtYawTolerance;
}

void UBTTask_AttackPlayer::UpdateAimAtTarget(ABaseAIController* AIController, ABaseAICharacter* AICharacter, ACharacter* CurrentTarget, float DeltaSeconds)
{
	AICharacter->UpdateEyePitch(CurrentTarget);
	AIController->YawFocusOnTarget(CurrentTarget, DeltaSeconds);
}

void UBTTask_AttackPlayer::UpdateShootingState(ABaseAICharacter* AICharacter, ACharacter* CurrentTarget)
{
	if (IsFacingTarget(AICharacter, CurrentTarget))
	{ 
		AICharacter->StartShooting();
	}
	else
	{
		AICharacter->StopShooting();
	}
}