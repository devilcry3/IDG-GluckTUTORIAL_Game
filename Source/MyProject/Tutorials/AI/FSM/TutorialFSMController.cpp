// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialFSMController.h"
#include "../../Common/TutorialEnemyBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Navigation/PathFollowingComponent.h"
#include "TimerManager.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Kismet/GameplayStatics.h"

ATutorialFSMController::ATutorialFSMController()
{
    // Create the AI perception component.
    EnemyPerception =
        CreateDefaultSubobject<UAIPerceptionComponent>(
            TEXT("EnemyPerception")
        );

    // Create a sight configuration.
    SightConfig =
        CreateDefaultSubobject<UAISenseConfig_Sight>(
            TEXT("SightConfig")
        );

    // Configure the enemy's vision.
    SightConfig->SightRadius = 1500.0f;
    SightConfig->LoseSightRadius = 1600.0f;
    SightConfig->PeripheralVisionAngleDegrees = 65.0f;

    // Allow detection regardless of team affiliation.
    SightConfig->DetectionByAffiliation.bDetectEnemies = true;
    SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
    SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

    // Register sight with the perception component.
    EnemyPerception->ConfigureSense(*SightConfig);

    EnemyPerception->SetDominantSense(
        SightConfig->GetSenseImplementation()
    );

    SetPerceptionComponent(*EnemyPerception);
}

void ATutorialFSMController::BeginPlay()
{
    Super::BeginPlay();

    // Connect perception updates to our C++ function.
    EnemyPerception->OnTargetPerceptionUpdated.AddDynamic(
        this,
        &ATutorialFSMController::HandleTargetPerceptionUpdated
    );
}

void ATutorialFSMController::OnPossess(APawn* InPawn)
{
    Super::OnPossess(InPawn);

    // Start in the Patrol state.
    CurrentState = ETutorialEnemyState::Patrol;

    UE_LOG(LogTemp, Display,
        TEXT("FSM controller possessed: %s | State: Patrol"),
        *GetNameSafe(InPawn));

    // Begin the enemy's initial behavior.
    EnterState(ETutorialEnemyState::Patrol);
}

void ATutorialFSMController::HandleTargetPerceptionUpdated(
    AActor* Actor,
    FAIStimulus Stimulus)
{
    // Only respond to the player character.
    if (!Actor ||
        Actor != UGameplayStatics::GetPlayerPawn(this, 0))
    {
        return;
    }

    // PLAYER DETECTED
    if (Stimulus.WasSuccessfullySensed())
    {
        TargetActor = Actor;

        UE_LOG(LogTemp, Display,
            TEXT("FSM Sight: Player detected"));

        // Switch from Patrol to Chase.
        if (CurrentState != ETutorialEnemyState::Chase)
        {
            EnterState(ETutorialEnemyState::Chase);
        }
    }

    // PLAYER LOST
    else if (TargetActor == Actor)
    {
        TargetActor = nullptr;

        UE_LOG(LogTemp, Display,
            TEXT("FSM Sight: Player lost"));

        // Switch from Chase back to Patrol.
        if (CurrentState == ETutorialEnemyState::Chase)
        {
            EnterState(ETutorialEnemyState::Patrol);
        }
    }
}



void ATutorialFSMController::EnterState(
    ETutorialEnemyState NewState)
{
    // Avoid repeatedly entering the same state.
    // Allow the first Patrol initialization.
    if (bFSMInitialized && CurrentState == NewState)
    {
        return;
    }

    CurrentState = NewState;
    bFSMInitialized = true;

    // Run the appropriate behavior for this state.
    switch (CurrentState)
    {

    case ETutorialEnemyState::Patrol:
    {
        UE_LOG(LogTemp, Display,
            TEXT("FSM entered PATROL state"));

        ATutorialEnemyBase* Enemy =
            Cast<ATutorialEnemyBase>(GetPawn());

        if (!Enemy)
        {
            UE_LOG(LogTemp, Warning,
                TEXT("FSM Patrol: No valid enemy pawn"));
            break;
        }

        // Cancel any previous patrol timer.
        GetWorldTimerManager().ClearTimer(PatrolTimerHandle);

        GetWorldTimerManager().ClearTimer(ChaseTimerHandle);

        // Stop the previous movement request.
        StopMovement();

        // Apply this enemy's configured patrol speed.
        Enemy->GetCharacterMovement()->MaxWalkSpeed =
            Enemy->PatrolSpeed;

        // Start moving along the patrol route.
        MoveToCurrentPatrolPoint();

        break;
    }



    case ETutorialEnemyState::Chase:
    {
        UE_LOG(LogTemp, Display,
            TEXT("FSM entered CHASE state"));

        ATutorialEnemyBase* Enemy =
            Cast<ATutorialEnemyBase>(GetPawn());

        if (!Enemy || !IsValid(TargetActor.Get()))
        {
            UE_LOG(LogTemp, Warning,
                TEXT("FSM Chase: Missing enemy or target"));

            EnterState(ETutorialEnemyState::Patrol);
            break;
        }

        // Cancel any scheduled patrol movement.
        GetWorldTimerManager().ClearTimer(PatrolTimerHandle);

        // Stop the current patrol movement.
        StopMovement();

        // Increase the enemy's walking speed.
        Enemy->GetCharacterMovement()->MaxWalkSpeed =
            Enemy->ChaseSpeed;

        // Begin moving toward the player.
        ChasePlayer();

        break;
    }

    }
}


void ATutorialFSMController::MoveToCurrentPatrolPoint()
{
    if (CurrentState != ETutorialEnemyState::Patrol)
    {
        return;
    }

    // Get the enemy character controlled by this AI.
    ATutorialEnemyBase* Enemy =
        Cast<ATutorialEnemyBase>(GetPawn());

    if (!Enemy)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("FSM Patrol: Enemy character invalid"));
        return;
    }

    // The enemy needs at least one patrol destination.
    if (Enemy->PatrolPoints.IsEmpty())
    {
        UE_LOG(LogTemp, Warning,
            TEXT("FSM Patrol: No patrol points assigned"));
        return;
    }

    // Prevent an invalid array index.
    if (!Enemy->PatrolPoints.IsValidIndex(
        Enemy->CurrentPatrolIndex))
    {
        Enemy->CurrentPatrolIndex = 0;
    }

    AActor* Destination =
        Enemy->PatrolPoints[Enemy->CurrentPatrolIndex].Get();

    // Skip missing or deleted patrol points.
    if (!IsValid(Destination))
    {
        UE_LOG(LogTemp, Warning,
            TEXT("FSM Patrol: Invalid patrol point"));

        GetWorldTimerManager().SetTimer(
            PatrolTimerHandle,
            this,
            &ATutorialFSMController::AdvanceToNextPatrolPoint,
            0.5f,
            false
        );
        return;
    }

    UE_LOG(LogTemp, Display,
        TEXT("FSM Patrol: Moving to point %d - %s"),
        Enemy->CurrentPatrolIndex,
        *GetNameSafe(Destination));

    // Request navigation movement toward the patrol actor.
    const EPathFollowingRequestResult::Type MoveResult =
        MoveToActor(Destination, 75.0f, true);

    // A failed request may not generate a completion callback.
    if (MoveResult == EPathFollowingRequestResult::Failed)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("FSM Patrol: Movement request failed"));

        GetWorldTimerManager().SetTimer(
            PatrolTimerHandle,
            this,
            &ATutorialFSMController::AdvanceToNextPatrolPoint,
            0.5f,
            false
        );
    }
    else if (MoveResult ==
        EPathFollowingRequestResult::AlreadyAtGoal)
    {
        // Already within the 75 cm acceptance radius.
        GetWorldTimerManager().SetTimer(
            PatrolTimerHandle,
            this,
            &ATutorialFSMController::AdvanceToNextPatrolPoint,
            1.0f,
            false
        );
    }

    // Otherwise wait for OnMoveCompleted().
}


void ATutorialFSMController::AdvanceToNextPatrolPoint()
{
    if (CurrentState != ETutorialEnemyState::Patrol)
    {
        return;
    }

    ATutorialEnemyBase* Enemy =
        Cast<ATutorialEnemyBase>(GetPawn());

    if (!Enemy || Enemy->PatrolPoints.IsEmpty())
    {
        return;
    }

    // Move to the next index, wrapping back to zero.
    Enemy->CurrentPatrolIndex =
        (Enemy->CurrentPatrolIndex + 1)
        % Enemy->PatrolPoints.Num();

    MoveToCurrentPatrolPoint();
}


void ATutorialFSMController::OnMoveCompleted(
    FAIRequestID RequestID,
    const FPathFollowingResult& Result)
{
    Super::OnMoveCompleted(RequestID, Result);


    // Ignore movement intentionally interrupted by a state change.
    if (Result.IsInterrupted())
    {
        return;
    }

    // Handle Chase movement completion.
    if (CurrentState == ETutorialEnemyState::Chase)
    {
        const float RetryDelay =
            Result.IsSuccess() ? 0.25f : 0.5f;

        GetWorldTimerManager().SetTimer(
            ChaseTimerHandle,
            this,
            &ATutorialFSMController::ChasePlayer,
            RetryDelay,
            false
        );

        return;
    }

    // Continue with the existing Patrol movement logic.
    if (CurrentState != ETutorialEnemyState::Patrol)
    {
        return;
    }


    const bool bSucceeded = Result.IsSuccess();

    UE_LOG(LogTemp, Display,
        TEXT("FSM Patrol: Move completed. Success: %s"),
        bSucceeded ? TEXT("True") : TEXT("False"));

    // Pause one second after success, or half a second
    // before skipping a failed destination.
    const float WaitSeconds = bSucceeded ? 1.0f : 0.5f;

    GetWorldTimerManager().SetTimer(
        PatrolTimerHandle,
        this,
        &ATutorialFSMController::AdvanceToNextPatrolPoint,
        WaitSeconds,
        false
    );
}


void ATutorialFSMController::ChasePlayer()
{
    // Only chase while in the Chase state.
    if (CurrentState != ETutorialEnemyState::Chase)
    {
        return;
    }

    // If the player is no longer a valid target,
    // return to patrol.
    if (!IsValid(TargetActor.Get()))
    {
        EnterState(ETutorialEnemyState::Patrol);
        return;
    }

    // Ask Unreal's navigation system to follow
    // the target actor.
    const EPathFollowingRequestResult::Type MoveResult =
        MoveToActor(
            TargetActor.Get(),
            150.0f,
            true
        );

    if (MoveResult == EPathFollowingRequestResult::Failed)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("FSM Chase: Movement failed, retrying"));

        GetWorldTimerManager().SetTimer(
            ChaseTimerHandle,
            this,
            &ATutorialFSMController::ChasePlayer,
            0.5f,
            false
        );
    }
    else if (
        MoveResult == EPathFollowingRequestResult::AlreadyAtGoal)
    {
        // If we're already near the player, check again
        // shortly in case they move away.
        GetWorldTimerManager().SetTimer(
            ChaseTimerHandle,
            this,
            &ATutorialFSMController::ChasePlayer,
            0.25f,
            false
        );
    }

    // For a successful movement request, Unreal will
    // call OnMoveCompleted when the request finishes.
}



