
#include "TutorialBTSetSpeed.h"
#include "../../Common/TutorialEnemyBase.h"

#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UTutorialBTSetSpeed::UTutorialBTSetSpeed()
{
    NodeName = TEXT("Set Movement Speed");
}

EBTNodeResult::Type UTutorialBTSetSpeed::ExecuteTask(
    UBehaviorTreeComponent& OwnerComp,
    uint8* NodeMemory)
{
    AAIController* AI = OwnerComp.GetAIOwner();

    ATutorialEnemyBase* Enemy = AI
        ? Cast<ATutorialEnemyBase>(AI->GetPawn())
        : nullptr;

    if (!Enemy || !Enemy->GetCharacterMovement())
    {
        return EBTNodeResult::Failed;
    }

    const float DesiredSpeed = bUseChaseSpeed
        ? Enemy->ChaseSpeed
        : Enemy->PatrolSpeed;

    Enemy->GetCharacterMovement()->MaxWalkSpeed =
        DesiredSpeed;

    return EBTNodeResult::Succeeded;
}
