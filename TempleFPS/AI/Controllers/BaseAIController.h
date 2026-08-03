#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BaseAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UBehaviorTree;
class UBlackboardComponent;
class UBehaviorTreeComponent;
class ABaseCharacter;
class UHealthComponent;

UCLASS()
class TEMPLEFPS_API ABaseAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABaseAIController();


	void HandleDamageFromAttacker(
		ABaseCharacter* Attacker
	);

	UFUNCTION(BlueprintCallable, Category = "AI|Blackboard")
	void SetCombatTargetBlackboardKey(ABaseCharacter* TargetCharacter);

	UFUNCTION(BlueprintCallable, Category = "AI|Blackboard")
	void ClearCombatTargetBlackboardKey();

	UFUNCTION(BlueprintCallable, Category = "AI|Blackboard")
	void SetCanSeeTargetBlackboardKey(bool IsVisible);

	UFUNCTION(BlueprintCallable, Category = "AI|Blackboard")
	void SetMoveLocationBlackBoardKey(FVector DesiredLocation);

	void StartReacquireTargetTimer();

	

	virtual void Tick(float DeltaTime) override;

	

	UFUNCTION(BlueprintCallable, Category = "Rotation")
	void YawFocusOnTarget(AActor* Target, float DeltaTime);



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

	FTimerHandle ReacquireTargetTimer;

	bool TickUpdateMoveLocation = false;

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	UFUNCTION()
	void HandleSightStimulus(AActor* Actor, FAIStimulus Stimulus /*Passing in a copy of the data  not a pointer or reference*/);

	
	bool IsLivingCombatTarget(
		ABaseCharacter* TargetCharacter
	) const;

	void BindToCombatTargetHealth(
		ABaseCharacter* TargetCharacter
	);

	void UnbindFromCombatTargetHealth(
		ABaseCharacter* TargetCharacter
	);

	bool IsTargetCurrentlyVisible(
		ABaseCharacter* TargetCharacter
	) const;

	bool TrySelectVisibleLivingTarget(
		ABaseCharacter* ExcludedCharacter = nullptr
	);

	UFUNCTION()
	void HandleCombatTargetHealthChanged(
		float CurrentHealth,
		float MaxHealth,
		ABaseCharacter* Attacker
	);


private:


};