#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UBehaviorTree;
class UBlackboardComponent;
class UBehaviorTreeComponent;

UCLASS()
class TEMPLEFPS_API ABaseAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABaseAIController();


	UFUNCTION()
	void FocusOnTarget(AActor* TargetActor);

	UFUNCTION()
	void UnfocusOnTarget();

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAIPerceptionComponent* AIPerception = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Components")
	UBlackboardComponent* BlackboardComponent = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	UBehaviorTree* BehaviorTree = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Perception")
	UAISenseConfig_Sight* SightConfig = nullptr;



	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void HandleSightStimulus(AActor* Actor, FAIStimulus Stimulus /*Passing in a copy of the data  not a pointer or reference*/);

	



private:


};