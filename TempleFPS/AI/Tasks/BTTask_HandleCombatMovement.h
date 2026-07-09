#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_HandleCombatMovement.generated.h"

class ACharacter;
class ABaseAIController;
class ABaseAICharacter;

UCLASS()
class TEMPLEFPS_API UBTTask_HandleCombatMovement : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_HandleCombatMovement();

protected:
	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory
	) override;

	virtual void TickTask(
		UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory,
		float DeltaSeconds
	) override;

private:
	ACharacter* GetCurrentPlayer(UBehaviorTreeComponent& OwnerComp) const;
	ABaseAIController* GetBaseAIController(UBehaviorTreeComponent& OwnerComp) const;
	ABaseAICharacter* GetAICharacter(UBehaviorTreeComponent& OwnerComp) const;

	float GetDistanceToEnemy(
		ABaseAICharacter* AICharacter,
		ACharacter* CurrentTarget
	) const;

	bool CanMoveWithInput(
		ABaseAICharacter* AICharacter,
		const FVector2D& MoveInput
	) const;

	bool TryMoveWithInput(
		ABaseAICharacter* AICharacter,
		const FVector2D& MoveInput
	) const;

	void ChooseRandomStrafeDirection();

	void HandleFallbackStrafeMovement(ABaseAICharacter* AICharacter);

	float CurrentStrafeDirection = 1.f;
	float StrafeDecisionTimer = 0.f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Combat Movement")
	float MinStrafeDecisionTime = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Combat Movement")
	float MaxStrafeDecisionTime = 2.5f;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Combat Movement")
	float MovementCheckDistance = 150.f;
};