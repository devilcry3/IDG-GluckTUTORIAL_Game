
#include "TutorialBTSelectPatrol.h"
#include "../../Common/TutorialEnemyBase.h"

#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UTutorialBTSelectPatrol::UTutorialBTSelectPatrol()
{
    NodeName = TEXT("Select Patrol Point");
}

EBTNodeResult::Type UTutorialBTSelectPatrol::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory)
{
    AAIController* AI = OwnerComp.GetAIOwner();
    ATutorialEnemyBase* Enemy = AI
        ? Cast<ATutorialEnemyBase>(AI->GetPawn())
        : nullptr;

    UBlackboardComponent* Blackboard =
        OwnerComp.GetBlackboardComponent();

    if (!Enemy || !Blackboard ||
        Enemy->PatrolPoints.IsEmpty())
    {
        UE_LOG(LogTemp, Warning,
            TEXT("BT: No enemy, blackboard, or patrol points"));
        return EBTNodeResult::Failed;
    }

    if (!Enemy->PatrolPoints.IsValidIndex(
        Enemy->CurrentPatrolIndex))
    {
        Enemy->CurrentPatrolIndex = 0;
    }

    AActor* Point =
        Enemy->PatrolPoints[Enemy->CurrentPatrolIndex].Get();

    if (!IsValid(Point))
    {
        UE_LOG(LogTemp, Warning,
            TEXT("BT: Invalid patrol point at index %d"),
            Enemy->CurrentPatrolIndex);
        return EBTNodeResult::Failed;
    }

    Blackboard->SetValueAsObject(
        BlackboardKey.SelectedKeyName, Point);

    return EBTNodeResult::Succeeded;
}
