// Fill out your copyright notice in the Description page of Project Settings.


#include "TutorialEnemyBase.h"
#include "GameFramework/CharacterMovementComponent.h"

ATutorialEnemyBase::ATutorialEnemyBase()
{
    // Let the navigation system control movement.
    AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

    // The enemy must not receive player input.
    AutoPossessPlayer = EAutoReceiveInput::Disabled;

    // Rotate toward the direction of movement.
    bUseControllerRotationYaw = false;
    bUseControllerRotationPitch = false;
    bUseControllerRotationRoll = false;

    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->bUseControllerDesiredRotation = false;

    GetCharacterMovement()->RotationRate =
        FRotator(0.0f, 360.0f, 0.0f);

    GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
}
