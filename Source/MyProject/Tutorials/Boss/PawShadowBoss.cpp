// Fill out your copyright notice in the Description page of Project Settings.


#include "PawShadowBoss.h"
#include "Tutorials/Boss/PawBossEncounter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"

APawShadowBoss::APawShadowBoss()
{
    // Leaves Tick available for the later movement/attack implementation.
    PrimaryActorTick.bCanEverTick = true;

    // Matches the tag your existing punches already recognize.
    Tags.AddUnique(TEXT("ShadowBaddy"));

    ShadowVisual =
        CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShadowVisual"));

    ShadowVisual->SetupAttachment(GetCapsuleComponent());
    ShadowVisual->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    GetCapsuleComponent()->SetGenerateOverlapEvents(true);
    GetCharacterMovement()->MaxWalkSpeed = 220.0f;
}

void APawShadowBoss::BeginPlay()
{
    Super::BeginPlay();

    Health = FMath::Max(MaxHealth, 0.01f);
}

bool APawShadowBoss::CanAct() const
{
    return bActive
        && !bDefeated
        && !IsStunned()
        && IsValid(Encounter)
        && Encounter->IsEncounterActive();
}

void APawShadowBoss::SetBossActive(bool NewActive)
{
    bActive = NewActive && !bDefeated;

    if (!bActive)
    {
        GetCharacterMovement()->StopMovementImmediately();
    }
}

bool APawShadowBoss::IsStunned() const
{
    return GetWorld()
        && GetWorld()->GetTimeSeconds() < StunEndTime;
}

void APawShadowBoss::ApplyStun(float Duration)
{
    if (bDefeated || Duration <= 0.0f || !GetWorld())
    {
        return;
    }

    const double NewEndTime =
        double(GetWorld()->GetTimeSeconds()) + Duration;

    // A shorter new stun cannot shorten an existing longer stun.
    StunEndTime = FMath::Max(StunEndTime, NewEndTime);

    GetCharacterMovement()->StopMovementImmediately();
    OnStunStarted(Duration);
}

float APawShadowBoss::TakeDamage(
    float DamageAmount,
    const FDamageEvent& DamageEvent,
    AController* EventInstigator,
    AActor* DamageCauser
)
{
    if (DamageAmount <= 0.0f
        || !FMath::IsFinite(DamageAmount)
        || !CanBeDamaged()
        || !bActive
        || bDefeated
        || !IsValid(Encounter)
        || !Encounter->IsEncounterActive())
    {
        return 0.0f;
    }

    // Stunned enemies can still receive damage.
    const float AppliedDamage = FMath::Min(Health, DamageAmount);

    Health = FMath::Max(0.0f, Health - AppliedDamage);

    const bool bLethal = Health <= 0.0f;

    if (bLethal)
    {
        // Set this before notifying Blueprint to prevent repeated defeat.
        bDefeated = true;
        SetBossActive(false);
    }

    // Preserve Unreal's standard damage notifications.
    Super::TakeDamage(
        AppliedDamage,
        DamageEvent,
        EventInstigator,
        DamageCauser
    );

    OnBossDamaged(AppliedDamage, DamageCauser);

    if (bLethal)
    {
        OnBossDefeated();
    }

    return AppliedDamage;
}

void APawShadowBoss::ChangeForm(
    EPawShadowForm NewForm,
    float NewMaxHealth
)
{
    if (NewMaxHealth <= 0.0f || !FMath::IsFinite(NewMaxHealth))
    {
        return;
    }

    Form = NewForm;
    MaxHealth = NewMaxHealth;
    Health = MaxHealth;

    bDefeated = false;
    StunEndTime = 0.0;

    // Blueprint explicitly reactivates after the transformation is ready.
    SetBossActive(false);
    OnFormChanged();
}

bool APawShadowBoss::BeginFragmentationCycle()
{
    // Never discard fragments that are still unresolved.
    if (!LooseFragments.IsEmpty())
    {
        return false;
    }

    ReturnedMass = 0.0f;
    return true;
}

bool APawShadowBoss::RegisterLooseFragment(
    AActor* Fragment,
    float Mass
)
{
    if (!IsValid(Fragment)
        || Fragment == this
        || Mass <= 0.0f
        || !FMath::IsFinite(Mass)
        || LooseFragments.Contains(Fragment))
    {
        return false;
    }

    LooseFragments.Add(Fragment, Mass);
    return true;
}

bool APawShadowBoss::ResolveLooseFragment(
    AActor* Fragment,
    bool bReabsorbed
)
{
    const float* StoredMass = LooseFragments.Find(Fragment);

    if (StoredMass == nullptr)
    {
        return false;
    }

    if (bReabsorbed && !CanAct())
    {
        return false;
    }

    if (bReabsorbed)
    {
        ReturnedMass += *StoredMass;
    }

    // Both destruction and absorption resolve the loose fragment.
    LooseFragments.Remove(Fragment);

    OnMassChanged();
    return true;
}

bool APawShadowBoss::CanReconstruct() const
{
    return CanAct()
        && Form == EPawShadowForm::Core
        && LooseFragments.IsEmpty()
        && ReturnedMass >= FMath::Max(0.01f, ReconstructionThreshold);
}