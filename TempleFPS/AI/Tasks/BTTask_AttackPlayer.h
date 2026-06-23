// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameFramework/Character.h"
#include "BTTask_AttackPlayer.generated.h" 

class ABaseAIController;


UCLASS()
class TEMPLEFPS_API UBTTask_AttackPlayer : public UBTTaskNode
{
	GENERATED_BODY()


public:

	UBTTask_AttackPlayer();

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

	bool bIsWithinLookRange = false;

	float LookAtYawTolerance = 30.f;
	
		ACharacter* Player = nullptr;



private:


};
	
