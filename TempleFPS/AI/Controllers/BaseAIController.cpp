#include "BaseAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h" 
#include "../../Characters/BaseCharacter.h"
#include "TimerManager.h"
#include "Engine/World.h"
#include "Perception/AISense_Sight.h"
#include "../../ActorComponents/HealthComponent.h"

ABaseAIController::ABaseAIController()
{
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
	SetPerceptionComponent(*AIPerception); // you have to tell the controller which perception component to actually use

	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	AIPerception->ConfigureSense(*SightConfig);
	AIPerception->SetDominantSense(SightConfig->GetSenseImplementation());

	bSetControlRotationFromPawnOrientation = false;

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;





}

void ABaseAIController::BeginPlay()
{
	Super::BeginPlay();

	if (AIPerception)
	{
		AIPerception->OnTargetPerceptionUpdated.AddDynamic(
			this,
			&ABaseAIController::OnTargetPerceptionUpdated
		);
	}
}

void ABaseAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TickUpdateMoveLocation)
	{
		if (!IsValid(BlackboardComponent)) 
		{
			return;
		}

		UObject* CurrentTarget = BlackboardComponent->GetValueAsObject(TEXT("CombatTarget"));

		if (ABaseCharacter* LocalCurrentTarget = Cast<ABaseCharacter>(CurrentTarget))
		{
			SetMoveLocationBlackBoardKey(LocalCurrentTarget->GetActorLocation());
		}
	}

}


void ABaseAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!BehaviorTree)
	{
		UE_LOG(LogTemp, Error, TEXT("No BehaviorTree assigned"));
		return;
	}

	RunBehaviorTree(BehaviorTree);

	BlackboardComponent = GetBlackboardComponent();

	UE_LOG(LogTemp, Warning, TEXT("Behavior Tree started"));
}

void ABaseAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (!Actor)
	{
		return;
	}

	const FAISenseID SightSenseID = UAISense::GetSenseID<UAISense_Sight>();

	if (Stimulus.Type == SightSenseID)
	{
		HandleSightStimulus(Actor, Stimulus);
	}
}

void ABaseAIController::HandleSightStimulus(
	AActor* Actor,
	FAIStimulus Stimulus
)
{
	if (!IsValid(BlackboardComponent))
	{
		return;
	}

	ABaseCharacter* UpdatedCharacter =
		Cast<ABaseCharacter>(Actor);

	if (!IsValid(UpdatedCharacter) ||
		UpdatedCharacter == GetPawn())
	{
		return;
	}

	ABaseCharacter* CurrentTarget =
		Cast<ABaseCharacter>(
			BlackboardComponent->GetValueAsObject(
				TEXT("CombatTarget")
			)
		);

	// Never acquire or continue targeting a corpse.
	if (!IsLivingCombatTarget(UpdatedCharacter))
	{
		if (IsValid(AIPerception))
		{
			AIPerception->ForgetActor(
				UpdatedCharacter
			);
		}

		if (UpdatedCharacter == CurrentTarget)
		{
			ClearCombatTargetBlackboardKey();
			TrySelectVisibleLivingTarget();
		}

		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		// Our hidden current target has become visible again.
		if (UpdatedCharacter == CurrentTarget)
		{
			SetCombatTargetBlackboardKey(
				UpdatedCharacter
			);

			SetCanSeeTargetBlackboardKey(true);

			BlackboardComponent->ClearValue(
				TEXT("MoveLocation")
			);

			return;
		}

		// Remain committed to a living, visible current target.
		if (IsTargetCurrentlyVisible(CurrentTarget))
		{
			return;
		}

		// There is no visible current target. Select the closest
		// currently visible living character.
		if (!TrySelectVisibleLivingTarget())
		{
			// Defensive fallback in case perception has not yet
			// added UpdatedCharacter to its visible actor array.
			SetCombatTargetBlackboardKey(
				UpdatedCharacter
			);

			SetCanSeeTargetBlackboardKey(true);

			BlackboardComponent->ClearValue(
				TEXT("MoveLocation")
			);
		}

		return;
	}

	// Losing sight of a non-target does not affect combat.
	if (UpdatedCharacter != CurrentTarget)
	{
		return;
	}

	SetCanSeeTargetBlackboardKey(false);

	// Prefer another visible opponent over pursuing a hidden one.
	if (TrySelectVisibleLivingTarget(UpdatedCharacter))
	{
		return;
	}

	// No visible alternatives exist, so pursue the hidden
	// target using your five-second live-location behavior.
	SetMoveLocationBlackBoardKey(
		UpdatedCharacter->GetActorLocation()
	);

	TickUpdateMoveLocation = true;

	StartReacquireTargetTimer();
}

void ABaseAIController::SetCombatTargetBlackboardKey(
	ABaseCharacter* TargetCharacter
)
{
	if (!IsValid(BlackboardComponent) ||
		!IsLivingCombatTarget(TargetCharacter))
	{
		return;
	}

	ABaseCharacter* PreviousTarget = Cast<ABaseCharacter>(
		BlackboardComponent->GetValueAsObject(
			TEXT("CombatTarget")
		)
	);

	if (PreviousTarget != TargetCharacter)
	{
		UnbindFromCombatTargetHealth(PreviousTarget);
	}

	// AddUniqueDynamic prevents duplicate bindings.
	BindToCombatTargetHealth(TargetCharacter);

	GetWorldTimerManager().ClearTimer(
		ReacquireTargetTimer
	);

	TickUpdateMoveLocation = false;

	BlackboardComponent->SetValueAsObject(
		TEXT("CombatTarget"),
		TargetCharacter
	);
}

void ABaseAIController::ClearCombatTargetBlackboardKey()
{
	TickUpdateMoveLocation = false;

	GetWorldTimerManager().ClearTimer(
		ReacquireTargetTimer
	);

	if (!IsValid(BlackboardComponent))
	{
		return;
	}

	ABaseCharacter* PreviousTarget = Cast<ABaseCharacter>(
		BlackboardComponent->GetValueAsObject(
			TEXT("CombatTarget")
		)
	);

	UnbindFromCombatTargetHealth(PreviousTarget);

	StopMovement();

	BlackboardComponent->SetValueAsBool(
		TEXT("CanSeeTarget"),
		false
	);

	BlackboardComponent->ClearValue(
		TEXT("MoveLocation")
	);

	BlackboardComponent->ClearValue(
		TEXT("CombatTarget")
	);
}

void ABaseAIController::SetCanSeeTargetBlackboardKey(bool IsVisible)
{
	if (IsValid(BlackboardComponent))
	{
		BlackboardComponent->SetValueAsBool(
			TEXT("CanSeeTarget"),
			IsVisible
		);
	}
}

void ABaseAIController::SetMoveLocationBlackBoardKey(
	FVector DesiredLocation)
{
	if (IsValid(BlackboardComponent))
	{
		BlackboardComponent->SetValueAsVector(
			TEXT("MoveLocation"),
			DesiredLocation
		);
	}
}

void ABaseAIController::YawFocusOnTarget(AActor* Target, float DeltaTime)
{
	FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(GetPawn()->GetActorLocation(), Target->GetActorLocation());
	float DesiredYaw = LookAtRot.Yaw;


	float CurrentPitch = GetControlRotation().Pitch;
	float CurrentRoll = GetControlRotation().Roll;


	FRotator Newrot(CurrentPitch, DesiredYaw, CurrentRoll);

	FRotator UpdatedRotation = FMath::RInterpTo(GetControlRotation(), Newrot, DeltaTime, 6.0f);

	SetControlRotation(UpdatedRotation);


}

void ABaseAIController::StartReacquireTargetTimer()
{
	GetWorldTimerManager().SetTimer(
		ReacquireTargetTimer,
		this,
		&ABaseAIController::ClearCombatTargetBlackboardKey,
		5.0f,
		false
	);
}


bool ABaseAIController::IsLivingCombatTarget(
	ABaseCharacter* TargetCharacter
) const
{
	if (!IsValid(TargetCharacter) ||
		TargetCharacter == GetPawn())
	{
		return false;
	}

	const UHealthComponent* TargetHealth =
		TargetCharacter->FindComponentByClass<UHealthComponent>();

	return IsValid(TargetHealth) &&
		!TargetHealth->IsDead();
}

void ABaseAIController::BindToCombatTargetHealth(
	ABaseCharacter* TargetCharacter
)
{
	if (!IsValid(TargetCharacter))
	{
		return;
	}

	if (UHealthComponent* TargetHealth =
		TargetCharacter->FindComponentByClass<UHealthComponent>())
	{
		TargetHealth->OnHealthChanged.AddUniqueDynamic(
			this,
			&ABaseAIController::HandleCombatTargetHealthChanged
		);
	}
}

void ABaseAIController::UnbindFromCombatTargetHealth(
	ABaseCharacter* TargetCharacter
)
{
	if (!IsValid(TargetCharacter))
	{
		return; //s
	}

	if (UHealthComponent* TargetHealth =
		TargetCharacter->FindComponentByClass<UHealthComponent>())
	{
		TargetHealth->OnHealthChanged.RemoveDynamic(
			this,
			&ABaseAIController::HandleCombatTargetHealthChanged
		);
	}
}

void ABaseAIController::HandleCombatTargetHealthChanged(
	float CurrentHealth,
	float MaxHealth,
	ABaseCharacter* Attacker
)
{
	if (CurrentHealth > 0.f ||
		!IsValid(BlackboardComponent))
	{
		return;
	}

	ABaseCharacter* DeadTarget = Cast<ABaseCharacter>(
		BlackboardComponent->GetValueAsObject(
			TEXT("CombatTarget")
		)
	);

	if (IsValid(AIPerception) && IsValid(DeadTarget))
	{
		AIPerception->ForgetActor(DeadTarget);
	}

	ClearCombatTargetBlackboardKey();
	TrySelectVisibleLivingTarget();
}

bool ABaseAIController::TrySelectVisibleLivingTarget(
	ABaseCharacter* ExcludedCharacter
)
{
	if (!IsValid(AIPerception) ||
		!IsValid(GetPawn()) ||
		!IsValid(BlackboardComponent))
	{
		return false;
	}

	TArray<AActor*> VisibleActors;

	AIPerception->GetCurrentlyPerceivedActors(
		UAISense_Sight::StaticClass(),
		VisibleActors
	);

	ABaseCharacter* BestTarget = nullptr;
	float BestDistanceSquared = MAX_flt;

	for (AActor* VisibleActor : VisibleActors)
	{
		ABaseCharacter* Candidate =
			Cast<ABaseCharacter>(VisibleActor);

		if (Candidate == ExcludedCharacter ||
			!IsLivingCombatTarget(Candidate))
		{
			continue;
		}

		const float DistanceSquared =
			FVector::DistSquared(
				GetPawn()->GetActorLocation(),
				Candidate->GetActorLocation()
			);

		if (DistanceSquared < BestDistanceSquared)
		{
			BestDistanceSquared = DistanceSquared;
			BestTarget = Candidate;
		}
	}

	if (!IsValid(BestTarget))
	{
		return false;
	}

	SetCombatTargetBlackboardKey(BestTarget);
	SetCanSeeTargetBlackboardKey(true);

	BlackboardComponent->ClearValue(
		TEXT("MoveLocation")
	);

	return true;
}

void ABaseAIController::HandleDamageFromAttacker(
	ABaseCharacter* Attacker
)
{
	if (!IsValid(BlackboardComponent) ||
		!IsLivingCombatTarget(Attacker))
	{
		return;
	}

	ABaseCharacter* CurrentTarget =
		Cast<ABaseCharacter>(
			BlackboardComponent->GetValueAsObject(
				TEXT("CombatTarget")
			)
		);

	// Stay committed while the current target is alive
	// and actually visible through sight perception.
	if (IsTargetCurrentlyVisible(CurrentTarget))
	{
		return;
	}

	// If anyone is currently visible, prefer the closest
	// visible character over a hidden attacker.
	if (TrySelectVisibleLivingTarget())
	{
		return;
	}

	// Nothing is visible. Pursue the known attacker, but do
	// not authorize shooting until sight confirms visibility.
	SetCombatTargetBlackboardKey(Attacker);
	SetCanSeeTargetBlackboardKey(false);

	SetMoveLocationBlackBoardKey(
		Attacker->GetActorLocation()
	);

	TickUpdateMoveLocation = true;

	StartReacquireTargetTimer();
}

bool ABaseAIController::IsTargetCurrentlyVisible(
	ABaseCharacter* TargetCharacter
) const
{
	if (!IsValid(AIPerception) ||
		!IsLivingCombatTarget(TargetCharacter))
	{
		return false;
	}

	TArray<AActor*> VisibleActors;

	AIPerception->GetCurrentlyPerceivedActors(
		UAISense_Sight::StaticClass(),
		VisibleActors
	);

	return VisibleActors.Contains(TargetCharacter);
}