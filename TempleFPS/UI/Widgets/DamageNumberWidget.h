#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../Enums/DamageNumberTypes.h"
#include "DamageNumberWidget.generated.h"

UCLASS()
class TEMPLEFPS_API UDamageNumberWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent, Category = "Damage Number")
	void InitializeDamageNumber(float DamageAmount,EDamageNumberType DamageNumberType);
};