#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_AttackPlayer.generated.h"

class ACharacter;
class ABaseAIController;
class ABaseAICharacter;
class UHealthComponent;

UCLASS()
class TEMPLEFPS_API UBTTask_AttackPlayer : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_AttackPlayer();

protected:
	
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

private:
	ACharacter* GetCurrentPlayer(UBehaviorTreeComponent& OwnerComp) const;

	ABaseAIController* GetBaseAIController(UBehaviorTreeComponent& OwnerComp) const;

	ABaseAICharacter* GetAICharacter(UBehaviorTreeComponent& OwnerComp) const;

	UHealthComponent* GetCurrentTargetHealthComponent(ACharacter* CurrentTarget) const;


	float GetYawDeltaToPlayer(ABaseAICharacter* AICharacter, ACharacter* CurrentTarget) const;

	bool IsFacingPlayer(ABaseAICharacter* AICharacter, ACharacter* CurrentTarget) const;

	void UpdateAimAtPlayer(ABaseAIController* AIController, ABaseAICharacter* AICharacter, ACharacter* CurrentTarget, float DeltaSeconds);

	void UpdateShootingState(ABaseAICharacter* AICharacter, ACharacter* CurrentTarget);

	float LookAtYawTolerance = 30.f;
};