#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Enums/DamageNumberTypes.h"
#include "DamageNumberActor.generated.h"

class UWidgetComponent;

UCLASS()
class TEMPLEFPS_API ADamageNumberActor : public AActor
{
	GENERATED_BODY()

public:

	ADamageNumberActor();

	UFUNCTION(BlueprintCallable, Category = "Damage Number")
	void ConfigureDamageNumber(
		float DamageAmount,
		EDamageNumberType DamageNumberType
	);

protected:

	virtual void BeginPlay() override;

	UPROPERTY(
		VisibleAnywhere,
		BlueprintReadOnly,
		Category = "Damage Number"
	)
	TObjectPtr<UWidgetComponent> DamageNumberWidgetComponent;

	UPROPERTY(
		EditDefaultsOnly,
		BlueprintReadOnly,
		Category = "Damage Number",
		meta = (ClampMin = "0.1")
	)
	float DamageNumberLifetime = 0.75f;
};