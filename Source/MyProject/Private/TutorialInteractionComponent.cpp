#include "TutorialInteractionComponent.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "DrawDebugHelpers.h"


UTutorialInteractionComponent::UTutorialInteractionComponent()
{
    // Character Blueprint will call our trace.
    // This component doesn't need its own Tick.
    PrimaryComponentTick.bCanEverTick = false;
}


void UTutorialInteractionComponent::BeginPlay()
{
    Super::BeginPlay();

    UE_LOG(LogTemp, Display,
        TEXT("Interaction component initialized on %s"),
        *GetNameSafe(GetOwner()));
}


// Run our actual interaction collision test.
bool UTutorialInteractionComponent::PerformInteractionTrace(
    FHitResult& OutHit)
{
    OutHit = FHitResult();

    // Find the pawn that owns this component.
    APawn* OwnerPawn = Cast<APawn>(GetOwner());

    if (!OwnerPawn)
        return false;

    // Find its camera and optional camera boom.
    UCameraComponent* Camera =
        OwnerPawn->FindComponentByClass<UCameraComponent>();

    USpringArmComponent* CameraBoom =
        OwnerPawn->FindComponentByClass<USpringArmComponent>();

    if (!Camera)
        return false;

    UWorld* World = GetWorld();

    if (!World)
        return false;

    // Position where our ray begins.
    FVector TraceStart = Camera->GetComponentLocation();

    // Normalized direction the camera faces.
    FVector Forward = Camera->GetForwardVector();

    // Account for the third-person camera distance.
    float TotalDistance = InteractionDistance;

    if (CameraBoom)
    {
        TotalDistance += CameraBoom->TargetArmLength;
    }

    // Calculate the end of the ray.
    FVector TraceEnd =
        TraceStart + (Forward * TotalDistance);

    // Configure collision query to ignore our character.
    FCollisionQueryParams QueryParams(
        SCENE_QUERY_STAT(TutorialInteractionTrace),
        false,
        OwnerPawn
    );

    // Perform the actual collision check.
    bool bHit = World->LineTraceSingleByChannel(
        OutHit,
        TraceStart,
        TraceEnd,
        ECC_Visibility,
        QueryParams
    );

    // Visual debugging only.
    if (ShowInteractionDebug)
    {
        DrawDebugLine(
            World,
            TraceStart,
            TraceEnd,
            bHit ? FColor::Green : FColor::Red,
            false,
            0.0f,
            0,
            0.02f
        );
    }

    return bHit;
}


// Set the current interaction target.
void UTutorialInteractionComponent::SetCurrentInteractable(
    AActor* NewInteractable)
{
    CurrentInteractable = NewInteractable;
}


// Clear the current target.
void UTutorialInteractionComponent::ClearCurrentInteractable()
{
    CurrentInteractable = nullptr;
}