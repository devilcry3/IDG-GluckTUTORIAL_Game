
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "TutorialEnemyState.h"
#include "TimerManager.h"
#include "TutorialFSMController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;

UCLASS(Blueprintable)
class MYPROJECT_API ATutorialFSMController : public AAIController
{
    GENERATED_BODY()

public:
    ATutorialFSMController();

    // The component that receives AI sensory information.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Tutorial|AI")
    TObjectPtr<UAIPerceptionComponent> EnemyPerception;

    // Configuration for the sight sense.
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Tutorial|AI")
    TObjectPtr<UAISenseConfig_Sight> SightConfig;

    // Current behavior state.
    UPROPERTY(BlueprintReadOnly, Category = "Tutorial|AI")
    ETutorialEnemyState CurrentState =
        ETutorialEnemyState::Patrol;

    // Player currently detected by this enemy.
    UPROPERTY(BlueprintReadOnly, Category = "Tutorial|AI")
    TObjectPtr<AActor> TargetActor = nullptr;

protected:
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;

    // Called when an actor becomes visible or is lost.
    UFUNCTION()
    void HandleTargetPerceptionUpdated(
        AActor* Actor,
        FAIStimulus Stimulus
    );


    // Changes the enemy's current behavior.
    void EnterState(ETutorialEnemyState NewState);

    // Tracks whether the initial state has been entered.
    bool bFSMInitialized = false;


    // Sends the enemy toward its currently selected patrol point.
    void MoveToCurrentPatrolPoint();

    // Advances the patrol index and begins the next movement.
    void AdvanceToNextPatrolPoint();

    // Called by Unreal when an AI movement request finishes.
    virtual void OnMoveCompleted(
        FAIRequestID RequestID,
        const FPathFollowingResult& Result
    ) override;

    // Timer used to pause between patrol points.
    FTimerHandle PatrolTimerHandle;


    // Requests movement toward the detected player.
    void ChasePlayer();

    // Used to retry or resume chase movement.
    FTimerHandle ChaseTimerHandle;


};
