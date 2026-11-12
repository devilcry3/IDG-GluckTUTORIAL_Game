
#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "TutorialBTController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Sight;
class UBehaviorTree;

UCLASS(Blueprintable)
class MYPROJECT_API ATutorialBTController : public AAIController
{
    GENERATED_BODY()

public:
    ATutorialBTController();

    // Assign the Behavior Tree in the controller Blueprint.
    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,
        Category = "Tutorial|AI")
    TObjectPtr<UBehaviorTree> BehaviorTreeAsset = nullptr;

protected:
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* InPawn) override;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly,
        Category = "Tutorial|AI")
    TObjectPtr<UAIPerceptionComponent> EnemyPerception;

    UPROPERTY()
    TObjectPtr<UAISenseConfig_Sight> SightConfig;

    UFUNCTION()
    void HandlePerceptionUpdated(
        AActor* Actor,
        FAIStimulus Stimulus
    );
};
