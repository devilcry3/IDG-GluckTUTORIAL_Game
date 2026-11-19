// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PawBossEncounter.generated.h"


// An enum gives a variable a fixed list of named choices.
UENUM(BlueprintType)
enum class EPawBossPhase : uint8
{
    Waiting,
    Giant,
    Fragmentation,
    FinalPair,
    Victory,
    Defeat
};

UCLASS(Blueprintable)
class MYPROJECT_API APawBossEncounter : public AActor
{
    GENERATED_BODY()

public:
    APawBossEncounter();

    UFUNCTION(BlueprintCallable, Category = "Boss")
    void StartEncounter();

    UFUNCTION(BlueprintCallable, Category = "Boss")
    void ReportInitialGiantCollapsed();

    UFUNCTION(BlueprintCallable, Category = "Boss")
    void ReportCoreDefeated();

    UFUNCTION(BlueprintCallable, Category = "Boss")
    bool RegisterFinalCombatant(AActor* Combatant);

    UFUNCTION(BlueprintCallable, Category = "Boss")
    void FinishFinalPhaseSetup();

    UFUNCTION(BlueprintCallable, Category = "Boss")
    void ResolveFinalCombatant(AActor* Combatant);

    UFUNCTION(BlueprintCallable, Category = "Boss")
    void ReportPlayerDefeated();

    UFUNCTION(BlueprintPure, Category = "Boss")
    bool IsEncounterActive() const;

    UFUNCTION(BlueprintPure, Category = "Boss")
    EPawBossPhase GetCurrentPhase() const
    {
        return CurrentPhase;
    }

    UFUNCTION(BlueprintPure, Category = "Boss")
    int32 GetRemainingFinalCombatants() const
    {
        return FinalCombatants.Num();
    }

protected:
    // C++ triggers these; the Blueprint child supplies their behavior.
    UFUNCTION(BlueprintImplementableEvent, Category = "Boss")
    void OnPhaseOneStarted();

    UFUNCTION(BlueprintImplementableEvent, Category = "Boss")
    void OnPhaseTwoStarted();

    UFUNCTION(BlueprintImplementableEvent, Category = "Boss")
    void OnPhaseThreeStarted();

    UFUNCTION(BlueprintImplementableEvent, Category = "Boss")
    void OnEncounterWon();

    UFUNCTION(BlueprintImplementableEvent, Category = "Boss")
    void OnEncounterLost();

private:
    UPROPERTY(VisibleInstanceOnly, Category = "Boss")
    EPawBossPhase CurrentPhase = EPawBossPhase::Waiting;

    // A Set contains each actor only once.
    UPROPERTY()
    TSet<TObjectPtr<AActor>> FinalCombatants;

    bool bFinalPhaseSetupComplete = false;

    void CheckFinalPhaseVictory();
};
