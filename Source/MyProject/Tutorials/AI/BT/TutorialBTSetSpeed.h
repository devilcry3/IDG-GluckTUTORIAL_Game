
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TutorialBTSetSpeed.generated.h"

UCLASS()
class MYPROJECT_API UTutorialBTSetSpeed
    : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UTutorialBTSetSpeed();

    // Unchecked = patrol speed; checked = chase speed.
    UPROPERTY(EditAnywhere, Category = "Tutorial|AI")
    bool bUseChaseSpeed = false;

protected:
    virtual EBTNodeResult::Type ExecuteTask(
        UBehaviorTreeComponent& OwnerComp,
        uint8* NodeMemory
    ) override;
};
