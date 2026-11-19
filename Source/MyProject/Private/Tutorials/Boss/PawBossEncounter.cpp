// Fill out your copyright notice in the Description page of Project Settings.

#include "Tutorials/Boss/PawBossEncounter.h"
#include "Components/SceneComponent.h"


APawBossEncounter::APawBossEncounter()
{
    // Reports drive this controller; it does not need per-frame updates.
    PrimaryActorTick.bCanEverTick = false;

    RootComponent =
        CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
}

bool APawBossEncounter::IsEncounterActive() const
{
    return CurrentPhase == EPawBossPhase::Giant
        || CurrentPhase == EPawBossPhase::Fragmentation
        || CurrentPhase == EPawBossPhase::FinalPair;
}

void APawBossEncounter::StartEncounter()
{
    if (CurrentPhase != EPawBossPhase::Waiting)
    {
        return;
    }

    CurrentPhase = EPawBossPhase::Giant;
    OnPhaseOneStarted();
}

void APawBossEncounter::ReportInitialGiantCollapsed()
{
    if (CurrentPhase != EPawBossPhase::Giant)
    {
        return;
    }

    CurrentPhase = EPawBossPhase::Fragmentation;
    OnPhaseTwoStarted();
}

void APawBossEncounter::ReportCoreDefeated()
{
    if (CurrentPhase != EPawBossPhase::Fragmentation)
    {
        return;
    }

    FinalCombatants.Empty();
    bFinalPhaseSetupComplete = false;

    // Change phase before Blueprint starts spawning/registering enemies.
    CurrentPhase = EPawBossPhase::FinalPair;
    OnPhaseThreeStarted();
}

bool APawBossEncounter::RegisterFinalCombatant(AActor* Combatant)
{
    if (CurrentPhase != EPawBossPhase::FinalPair
        || !IsValid(Combatant))
    {
        return false;
    }

    FinalCombatants.Add(Combatant);
    return true;
}

void APawBossEncounter::FinishFinalPhaseSetup()
{
    if (CurrentPhase != EPawBossPhase::FinalPair
        || bFinalPhaseSetupComplete)
    {
        return;
    }

    // Both initial bosses must be registered before victory is possible.
    if (FinalCombatants.Num() < 2)
    {
        UE_LOG(
            LogTemp,
            Warning,
            TEXT("Register both final bosses before finishing setup.")
        );

        return;
    }

    bFinalPhaseSetupComplete = true;
    CheckFinalPhaseVictory();
}

void APawBossEncounter::ResolveFinalCombatant(AActor* Combatant)
{
    if (CurrentPhase != EPawBossPhase::FinalPair
        || Combatant == nullptr)
    {
        return;
    }

    const int32 RemovedCount = FinalCombatants.Remove(Combatant);

    if (RemovedCount > 0)
    {
        CheckFinalPhaseVictory();
    }
}

void APawBossEncounter::CheckFinalPhaseVictory()
{
    if (CurrentPhase != EPawBossPhase::FinalPair
        || !bFinalPhaseSetupComplete
        || FinalCombatants.Num() != 0)
    {
        return;
    }

    CurrentPhase = EPawBossPhase::Victory;
    OnEncounterWon();
}

void APawBossEncounter::ReportPlayerDefeated()
{
    if (!IsEncounterActive())
    {
        return;
    }

    CurrentPhase = EPawBossPhase::Defeat;
    OnEncounterLost();
}