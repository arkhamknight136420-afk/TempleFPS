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
	FAIStimulus Stimulus)
{
	if (!IsValid(BlackboardComponent))
	{
		return;
	}

	ABaseCharacter* SeenCharacter = Cast<ABaseCharacter>(Actor);

	if (!IsValid(SeenCharacter) || SeenCharacter == GetPawn())
	{
		return;
	}

	if (Stimulus.WasSuccessfullySensed())
	{
		SetCombatTargetBlackboardKey(SeenCharacter);

		SetCanSeeTargetBlackboardKey(true);

		GetWorldTimerManager().ClearTimer(ReacquireTargetTimer);

		TickUpdateMoveLocation = false;


		return;
	}

	UObject* CurrentTarget =
		BlackboardComponent->GetValueAsObject(TEXT("CombatTarget"));

	SetCanSeeTargetBlackboardKey(false);
	
	TickUpdateMoveLocation = true;

	StartReacquireTargetTimer();
	

}

void ABaseAIController::SetCombatTargetBlackboardKey(ABaseCharacter* TargetCharacter)
{
	
	if (IsValid(TargetCharacter))
	{
		BlackboardComponent->SetValueAsObject(TEXT("CombatTarget"), TargetCharacter);
	}
}

void ABaseAIController::ClearCombatTargetBlackboardKey()
{
		BlackboardComponent->ClearValue(TEXT("CombatTarget"));
}

void ABaseAIController::SetCanSeeTargetBlackboardKey(bool IsVisible)
{
	BlackboardComponent->SetValueAsBool(TEXT("CanSeeTarget"), IsVisible);
}

void ABaseAIController::SetMoveLocationBlackBoardKey(FVector DesiredLocation)
{
	BlackboardComponent->SetValueAsVector(TEXT("MoveLocation"), DesiredLocation);
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


		
