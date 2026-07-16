#include "DamageNumberActor.h"

#include "Components/WidgetComponent.h"
#include "../Widgets/DamageNumberWidget.h"

ADamageNumberActor::ADamageNumberActor()
{
	PrimaryActorTick.bCanEverTick = false;

	DamageNumberWidgetComponent =
		CreateDefaultSubobject<UWidgetComponent>(
			TEXT("DamageNumberWidgetComponent")
		);

	SetRootComponent(DamageNumberWidgetComponent);

	// The widget remains a consistent screen size and automatically
	// faces the player's camera.
	DamageNumberWidgetComponent->SetWidgetSpace(
		EWidgetSpace::Screen
	);

	DamageNumberWidgetComponent->SetDrawSize(
		FVector2D(160.f, 100.f)
	);

	DamageNumberWidgetComponent->SetPivot(
		FVector2D(0.5f, 0.5f)
	);

	DamageNumberWidgetComponent->SetCollisionEnabled(
		ECollisionEnabled::NoCollision
	);

	DamageNumberWidgetComponent->SetGenerateOverlapEvents(false);
}

void ADamageNumberActor::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(
		FMath::Max(DamageNumberLifetime, 0.1f)
	);
}

void ADamageNumberActor::ConfigureDamageNumber(
	float DamageAmount,
	EDamageNumberType DamageNumberType
)
{
	if (!DamageNumberWidgetComponent)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"[DamageNumberActor] %s has no Widget Component."
			),
			*GetName()
		);

		Destroy();
		return;
	}

	// Creates the UUserWidget instance if it has not already
	// been created.
	DamageNumberWidgetComponent->InitWidget();

	UDamageNumberWidget* DamageNumberWidget =
		Cast<UDamageNumberWidget>(
			DamageNumberWidgetComponent->GetUserWidgetObject()
		);

	if (!DamageNumberWidget)
	{
		UE_LOG(
			LogTemp,
			Error,
			TEXT(
				"[DamageNumberActor] %s does not have a valid "
				"UDamageNumberWidget assigned."
			),
			*GetName()
		);

		Destroy();
		return;
	}

	DamageNumberWidget->InitializeDamageNumber(
		DamageAmount,
		DamageNumberType
	);
}