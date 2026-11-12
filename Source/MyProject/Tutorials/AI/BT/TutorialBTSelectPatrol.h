
#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "TutorialBTSelectPatrol.generated.h"

UCLASS()
class MYPROJECT_API UTutorialBTSelectPatrol
    : public UBTTask_BlackboardBase
{
    GENERATED_BODY()

public:
    UTutorialBTSelectPatrol();

protected:
    virtual EBTNodeResult::Type ExecuteTask(
        UBehaviorTreeComponent& OwnerComp,
        uint8* NodeMemory
    ) override;
};
