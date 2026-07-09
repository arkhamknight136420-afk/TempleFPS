#include "BaseAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h" 
#include "../../Characters/BaseCharacter.h"

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

void ABaseAIController::HandleSightStimulus(AActor* Actor, FAIStimulus Stimulus)
{

	if (Stimulus.WasSuccessfullySensed())
	{
		if (ABaseCharacter* SeenCharacter = Cast<ABaseCharacter>(Actor))
		{
			BlackboardComponent->SetValueAsObject(TEXT("Player"), SeenCharacter);
		}
	}
	else
	{
		BlackboardComponent->ClearValue(TEXT("Player"));	
	}	
}

void ABaseAIController::FocusOnTarget(AActor* TargetActor)
{

	SetFocus(TargetActor, EAIFocusPriority::Gameplay);
}

void ABaseAIController::UnfocusOnTarget()
{
	ClearFocus(EAIFocusPriority::Gameplay);
}

void ABaseAIController::SetPlayerBlackBoardKey()
{
	APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);

	if (PlayerPawn)
	{
		BlackboardComponent->SetValueAsObject(TEXT("Player"), PlayerPawn);
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


		
