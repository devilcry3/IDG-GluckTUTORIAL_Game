
#include "TutorialBTAdvancePatrol.h"
#include "../../Common/TutorialEnemyBase.h"

#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"

UTutorialBTAdvancePatrol::UTutorialBTAdvancePatrol()
{
    NodeName = TEXT("Advance Patrol Index");
}

EBTNodeResult::Type UTutorialBTAdvancePatrol::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory)
{
    AAIController* AI = OwnerComp.GetAIOwner();

    ATutorialEnemyBase* Enemy = AI
        ? Cast<ATutorialEnemyBase>(AI->GetPawn())
        : nullptr;

    if (!Enemy || Enemy->PatrolPoints.IsEmpty())
    {
        return EBTNodeResult::Failed;
    }

    Enemy->CurrentPatrolIndex =
        (Enemy->CurrentPatrolIndex + 1)
        % Enemy->PatrolPoints.Num();

    return EBTNodeResult::Succeeded;
}
