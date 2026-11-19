// Fill out your copyright notice in the Description page of Project Settings.


#include "PawShadowFragment.h"
#include "Tutorials/Boss/PawBossEncounter.h"

APawShadowFragment::APawShadowFragment()
{
    Form = EPawShadowForm::Fragment;
    MaxHealth = 0.3f;
}

bool APawShadowFragment::InitializeFragment(APawShadowBoss* Target)
{
    if (bResolved
        || bDefeated
        || IsValid(ReturnTarget)
        || !IsValid(Target)
        || !IsValid(Target->Encounter)
        || !Target->Encounter->IsEncounterActive())
    {
        return false;
    }

    if (!Target->RegisterLooseFragment(this, MassValue))
    {
        return false;
    }

    ReturnTarget = Target;
    Encounter = Target->Encounter;

    if (Encounter->GetCurrentPhase() == EPawBossPhase::FinalPair)
    {
        Encounter->RegisterFinalCombatant(this);
    }

    SetBossActive(true);
    return true;
}

bool APawShadowFragment::TryReabsorb()
{
    if (bResolved
        || !CanAct()
        || !IsValid(ReturnTarget)
        || !ReturnTarget->CanAct())
    {
        return false;
    }

    if (GetDistanceTo(ReturnTarget) > FMath::Max(0.0f, AbsorbDistance))
    {
        return false;
    }

    // Prevent another resolution while the target runs its Blueprint event.
    bResolved = true;

    if (!ReturnTarget->ResolveLooseFragment(this, true))
    {
        bResolved = false;
        return false;
    }

    SetBossActive(false);

    if (IsValid(Encounter))
    {
        Encounter->ResolveFinalCombatant(this);
    }

    Destroy();
    return true;
}

void APawShadowFragment::FinishFragmentDefeat()
{
    if (bResolved || !bDefeated)
    {
        return;
    }

    bResolved = true;

    if (IsValid(ReturnTarget))
    {
        // False means its mass was destroyed, not returned.
        ReturnTarget->ResolveLooseFragment(this, false);
    }

    if (IsValid(Encounter))
    {
        Encounter->ResolveFinalCombatant(this);
    }

    Destroy();
}

