// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PawShadowBoss.generated.h"

/// Forward declarations: these types exist, without including their headers here.
class APawBossEncounter;
class UStaticMeshComponent;

UENUM(BlueprintType)
enum class EPawShadowForm : uint8
{
    Giant,
    Core,
    Launcher,
    Brawler,
    Fragment
};

UCLASS(Blueprintable)
class MYPROJECT_API APawShadowBoss : public ACharacter
{
    GENERATED_BODY()

public:
    APawShadowBoss();

    virtual float TakeDamage(
        float DamageAmount,
        const FDamageEvent& DamageEvent,
        AController* EventInstigator,
        AActor* DamageCauser
    ) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Boss")
    TObjectPtr<UStaticMeshComponent> ShadowVisual;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadWrite,
        Category = "Boss",
        meta = (ExposeOnSpawn = "true")
    )
    TObjectPtr<APawBossEncounter> Encounter;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss")
    EPawShadowForm Form = EPawShadowForm::Giant;

    UPROPERTY(
        EditAnywhere,
        BlueprintReadOnly,
        Category = "Boss",
        meta = (ClampMin = "0.01")
    )
    float MaxHealth = 3.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss")
    float Health = 0.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss")
    bool bActive = false;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss")
    bool bDefeated = false;

    // Exposed now for the attack implementation that follows.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Attack")
    float AttackDamage = 10.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Attack")
    float AttackRange = 200.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Attack")
    float AttackCooldown = 2.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Attack")
    float WindupTime = 0.6f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Attack")
    float ChargeSpeed = 900.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Attack")
    float ChargeDuration = 0.8f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boss|Mass")
    float ReconstructionThreshold = 6.0f;

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Boss|Mass",
        meta = (ClampMin = "0.0"))
    float BrawlerPhaseHealthBonus = 24.0f;

    UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Boss|Mass")
    float ReturnedMass = 0.0f;

    UFUNCTION(BlueprintCallable, Category = "Boss")
    void SetBossActive(bool NewActive);

    UFUNCTION(BlueprintPure, Category = "Boss")
    bool CanAct() const;

    UFUNCTION(BlueprintCallable, Category = "Boss")
    void ApplyStun(float Duration);

    UFUNCTION(BlueprintPure, Category = "Boss")
    bool IsStunned() const;

    UFUNCTION(BlueprintCallable, Category = "Boss")
    void ChangeForm(EPawShadowForm NewForm);

    // Health currently carried by this boss's loose fragments.
    UFUNCTION(BlueprintPure, Category = "Boss|Mass")
    float GetLooseFragmentHealth() const;

    // All surviving health, wherever it is currently located.
    UFUNCTION(BlueprintPure, Category = "Boss|Mass")
    float GetTotalRemainingHealth() const;

    UFUNCTION(BlueprintCallable, Category = "Boss|Mass")
    bool BeginFragmentationCycle();

    UFUNCTION(BlueprintCallable, Category = "Boss|Mass")
    bool RegisterLooseFragment(AActor* Fragment, float Mass);

    UFUNCTION(BlueprintCallable, Category = "Boss|Mass")
    bool ResolveLooseFragment(AActor* Fragment, bool bReabsorbed);

    UFUNCTION(BlueprintPure, Category = "Boss|Mass")
    bool CanReconstruct() const;

    UFUNCTION(BlueprintPure, Category = "Boss|Mass")
    int32 GetLooseFragmentCount() const
    {
        return LooseFragments.Num();
    }

protected:
    virtual void BeginPlay() override;

    UFUNCTION(BlueprintImplementableEvent, Category = "Boss")
    void OnBossDamaged(float AppliedDamage, AActor* DamageCauser);

    UFUNCTION(BlueprintImplementableEvent, Category = "Boss")
    void OnBossDefeated();

    UFUNCTION(BlueprintImplementableEvent, Category = "Boss")
    void OnFormChanged();

    UFUNCTION(BlueprintImplementableEvent, Category = "Boss")
    void OnStunStarted(float Duration);

    UFUNCTION(BlueprintImplementableEvent, Category = "Boss")
    void OnMassChanged();

private:
    double StunEndTime = 0.0;

    // Each fragment is paired with the amount of mass it represents.
    UPROPERTY()
    TMap<TObjectPtr<AActor>, float> LooseFragments;
};