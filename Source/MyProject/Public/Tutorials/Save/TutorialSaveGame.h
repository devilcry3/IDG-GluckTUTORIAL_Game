// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Engine/World.h"
#include "TutorialSaveGame.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class MYPROJECT_API UTutorialSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

	// Version of our save file format.
	UPROPERTY(BlueprintReadOnly, Category = "Tutorial|Save")
	int32 SaveVersion = 1;

	// Map and checkpoint to restore.
	UPROPERTY(BlueprintReadOnly, Category = "Tutorial|Save")
	TSoftObjectPtr<UWorld> SavedMap;

	UPROPERTY(BlueprintReadOnly, Category = "Tutorial|Save")
	FName SavedCheckpoint;

	// Player's current objective
	UPROPERTY(BlueprintReadOnly, Category = "Tutorial|Save")
	FText SavedObjective;

	//Gameplay progression flags
	UPROPERTY(BlueprintReadOnly, Category = "Tutorial|Save")
	bool bSavedHasTriedEntrance = false;

	UPROPERTY(BlueprintReadOnly, Category = "Tutorial|Save")
	bool bSavedHasReadMaintenanceTerminal = false;
	
};
