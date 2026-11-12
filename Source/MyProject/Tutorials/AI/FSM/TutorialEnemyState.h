
#pragma once

#include "CoreMinimal.h"
#include "TutorialEnemyState.generated.h"

// The two behaviors available to our FSM enemy.
UENUM(BlueprintType)
enum class ETutorialEnemyState : uint8
{
    Patrol UMETA(DisplayName = "Patrol"),
    Chase UMETA(DisplayName = "Chase")
};
