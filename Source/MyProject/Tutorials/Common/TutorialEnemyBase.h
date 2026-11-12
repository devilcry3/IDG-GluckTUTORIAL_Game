// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "TutorialEnemyBase.generated.h"

UCLASS(Blueprintable)
class MYPROJECT_API ATutorialEnemyBase : public ACharacter
{
    GENERATED_BODY()

public:
    ATutorialEnemyBase();

    // Ordered patrol points assigned in the level.
    UPROPERTY(EditInstanceOnly, BlueprintReadWrite,
        Category = "Tutorial|AI")
    TArray<TObjectPtr<AActor>> PatrolPoints;

    // Movement speed during patrol.
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Tutorial|AI")
    float PatrolSpeed = 180.0f;

    // Movement speed while chasing a player.
    UPROPERTY(EditAnywhere, BlueprintReadWrite,
        Category = "Tutorial|AI")
    float ChaseSpeed = 400.0f;

    // Index of the current patrol destination.
    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly,
        Category = "Tutorial|AI")
    int32 CurrentPatrolIndex = 0;
};
