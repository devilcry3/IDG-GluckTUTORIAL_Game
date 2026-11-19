// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Tutorials/Boss/PawShadowBoss.h"
#include "PawShadowFragment.generated.h"

UCLASS(Blueprintable)
class MYPROJECT_API APawShadowFragment : public APawShadowBoss
{
    GENERATED_BODY()

public:
    APawShadowFragment();

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Fragment",
        meta = (ClampMin = "0.01")
    )
    float MassValue = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fragment")
    float ReturnSpeed = 180.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Fragment")
    float AbsorbDistance = 100.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Fragment")
    TObjectPtr<APawShadowBoss> ReturnTarget;

    UFUNCTION(BlueprintCallable, Category = "Fragment")
    bool InitializeFragment(APawShadowBoss* Target);

    UFUNCTION(BlueprintCallable, Category = "Fragment")
    bool TryReabsorb();

    UFUNCTION(BlueprintCallable, Category = "Fragment")
    void FinishFragmentDefeat();

private:
    bool bResolved = false;
};