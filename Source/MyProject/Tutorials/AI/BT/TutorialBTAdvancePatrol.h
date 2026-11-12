
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "TutorialBTAdvancePatrol.generated.h"

UCLASS()
class MYPROJECT_API UTutorialBTAdvancePatrol
    : public UBTTaskNode
{
    GENERATED_BODY()

public:
    UTutorialBTAdvancePatrol();

protected:
    virtual EBTNodeResult::Type ExecuteTask(
        UBehaviorTreeComponent& OwnerComp,
        uint8* NodeMemory
    ) override;
};
