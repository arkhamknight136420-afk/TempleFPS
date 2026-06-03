#include "BaseAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Kismet/GameplayStatics.h"

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
		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);


		if (Actor == PlayerPawn)
		{
			BlackboardComponent->SetValueAsObject(TEXT("Player"), Actor);
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