// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/HitResult.h"
#include "TutorialInteractionComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYPROJECT_API UTutorialInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

private:
	/** Actor currently targeted by the interaction trace*/
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<AActor> CurrentInteractable = nullptr;
	

public:	
	// Sets default values for this component's properties
	UTutorialInteractionComponent();
	/** Maximum interaction distance in centimeters. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "0.0", Units = "cm"))
	float InteractionDistance = 350.0f;

	/** Draw the interaction trace during development. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	bool ShowInteractionDebug = true;

	// Perform the collision trace and return its results.
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool PerformInteractionTrace(FHitResult& OutHit);

	// Store an actore Blueprint validates interaction.
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void SetCurrentInteractable(AActor* NewInteractable);

	// Remove the current interactiojn target.
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	void ClearCurrentInteractable();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	

		
};
