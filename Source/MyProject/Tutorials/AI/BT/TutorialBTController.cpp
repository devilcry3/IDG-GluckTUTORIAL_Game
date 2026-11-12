
#include "TutorialBTController.h"
#include "../../Common/TutorialEnemyBase.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"

ATutorialBTController::ATutorialBTController()
{
    EnemyPerception =
        CreateDefaultSubobject<UAIPerceptionComponent>(
            TEXT("EnemyPerception"));

    SightConfig =
        CreateDefaultSubobject<UAISenseConfig_Sight>(
            TEXT("SightConfig"));

    SightConfig->SightRadius = 1500.0f;
    SightConfig->LoseSightRadius = 1600.0f;
    SightConfig->PeripheralVisionAngleDegrees = 65.0f;

    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

    EnemyPerception->ConfigureSense(*SightConfig);

    EnemyPerception->SetDominantSense(
        SightConfig->GetSenseImplementation());

    SetPerceptionComponent(*EnemyPerception);
}

void ATutorialBTController::BeginPlay()
{
    Super::BeginPlay();

    EnemyPerception->OnTargetPerceptionUpdated.AddDynamic(
        this,
        &ATutorialBTController::HandlePerceptionUpdated);
}

void ATutorialBTController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    ATutorialEnemyBase* Enemy =
        Cast<ATutorialEnemyBase>(InPawn);

    if (!Enemy)
    {
        UE_LOG(LogTemp, Error,
            TEXT("BT: Controller possessed wrong pawn class"));
        return;
    }

    Enemy->GetCharacterMovement()->MaxWalkSpeed =
        Enemy->PatrolSpeed;

    if (!BehaviorTreeAsset)
    {
        UE_LOG(LogTemp, Error,
            TEXT("BT: No Behavior Tree asset assigned"));
        return;
    }

    if (RunBehaviorTree(BehaviorTreeAsset))
    {
        UE_LOG(LogTemp, Display,
            TEXT("BT: Behavior Tree started for %s"),
            *GetNameSafe(InPawn));
    }
    else
    {
        UE_LOG(LogTemp, Error,
            TEXT("BT: Failed to start Behavior Tree"));
    }
}

void ATutorialBTController::HandlePerceptionUpdated(
    AActor* Actor,
    FAIStimulus Stimulus)
{
    // Only respond to the tutorial player.
    if (!Actor ||
        Actor != UGameplayStatics::GetPlayerPawn(this, 0))
    {
        return;
    }

    UBlackboardComponent* BlackboardComp =
        GetBlackboardComponent();

    if (!BlackboardComp)
    {
        return;
    }

    if (Stimulus.WasSuccessfullySensed())
    {
        // Store the player before activating Chase.
        BlackboardComp->SetValueAsObject(
            TEXT("TargetActor"), Actor);

        BlackboardComp->SetValueAsBool(
            TEXT("CanSeePlayer"), true);

        UE_LOG(LogTemp, Display,
            TEXT("BT Sight: Player detected"));
    }
    else
    {
        // Deactivate Chase and clear its target.
        BlackboardComp->SetValueAsBool(
            TEXT("CanSeePlayer"), false);

        BlackboardComp->ClearValue(TEXT("TargetActor"));

        UE_LOG(LogTemp, Display,
            TEXT("BT Sight: Player lost"));
    }
}
