#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EDamageNumberType : uint8
{
	BodyShot UMETA(DisplayName = "Body Shot"),
	HeadShot UMETA(DisplayName = "Head Shot"),
	Kill UMETA(DisplayName = "Kill")
};