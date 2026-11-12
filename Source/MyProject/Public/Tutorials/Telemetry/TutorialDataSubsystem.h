// Fill out your copyright notice in the Description page of Project Settings.


#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "../../../Tutorials/Telemetry/TutorialTelemetryTypes.h"
#include "TutorialDataSubsystem.generated.h"

UCLASS()
class MYPROJECT_API UTutorialDataSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    // Called when Unreal initializes the Game Instance.
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // Called when the Game Instance shuts down.
    virtual void Deinitialize() override;

    // Starts a new tutorial playtesting session.
    // Can be called from our tutorial player controller Blueprint
    UFUNCTION(BlueprintCallable, Category = "Tutorial|Telemetry")
    void StartTutorialSession();

    // Saves current tutorial progress to disk
// Returns true if the save succeeeds
    UFUNCTION(BlueprintCallable, Category = "Tutorial|Save")
    bool SaveProgress();

    UFUNCTION(BlueprintCallable, Category = "Tutorial|Save")
    bool LoadProgress();

    // Records a gameplay event during an active tutorial session
    UFUNCTION(BlueprintCallable, Category = "Tutorial|Telemetry")
    void LogGameplayEvent(
        const FString& EventCategory,
        const FString& EventName,
        const FString& Details,
        double Value
    );


    // Exports the current playtesting session to a JSON file.
    // Returns true if the file was written successfully.
    UFUNCTION(BlueprintCallable, Category = "Tutorial|Telemetry")
    bool ExportTelemetryJSON();


    // ==========================================
    // TUTORIAL STATE
    // ==========================================

    // pREVENTS TUTORIAL SYSTEMS FROM AFFECTING pAWLADIN
    // wE will activate this when the tutorial starts
    UPROPERTY(BlueprintReadOnly, Category = "Tutorial|State")
    bool bTutorialActive = false;

    // ==========================================
    // GAMEPLAY PROGRESS
    // ==========================================

    //Current objective displayed to the player
    UPROPERTY(BlueprintReadWrite, Category = "Tutorial|Progress")
        FText CurrentObjective;

    //Tracks whether the player attempted the station entrance
    UPROPERTY(BlueprintReadWrite, Category = "Tutorial|Progress")
        bool bHasTriedEntrance = false;

    // Tracks whether the maintencance terminal was read
    UPROPERTY(BlueprintReadWrite, Category = "Tutorial|Progress")
        bool bHasReadMaintenanceTerminal = false;

    // ==========================================
    // MAPS AND CHECKPOINTS
    // ==========================================

    // Reference to the current tutorial map asset
    UPROPERTY(BlueprintReadWrite, Category = "Tutorial|Checkpoint")
        TSoftObjectPtr<UWorld> CurrentMap;

    // Identifier of the current checkpoint
    UPROPERTY(BlueprintReadWrite, Category = "Tutorial|Checkpoint")
        FName CurrentCheckpoint;

    // Whether to restore checkpoint position after loading a map
    UPROPERTY(BlueprintReadWrite, Category = "Tutorial|Checkpoint")
        bool bRestoreCheckpointOnNextMap = false;

    // ==========================================
    // SAVE SETTINGS
    // ==========================================

    // Name of the save-game slot
    UPROPERTY(BlueprintReadWrite, Category = "Tutorial|Save")
    FString SaveSlotName = TEXT("SilentRelaySave1");

    //Local user index for Unreal's save-game system
    UPROPERTY(BlueprintReadWrite, Category = "Tutorial|Save")
    int32 SaveUserIndex = 0;

    // ==========================================
    // TELEMETRY - SESSION DETAILS
    // ==========================================

    // Unique identifier for the current playtestr session
    UPROPERTY(BlueprintReadOnly, Category = "Tutorial|Telemetry")
    FString SessionID;

    //Anonymous identifier for the Participant
    UPROPERTY(BlueprintReadWrite, Category = "Tutorial|Telemetry")
    FString ParticipantCode = TEXT("anonymous");

    // Label identifying the build being tested
    UPROPERTY(BlueprintReadWrite, Category = "Tutorial|Telemetry")
    FString BuildLabel = TEXT("Week 2");

    // UTC start time in Unreal's DateTime format
    UPROPERTY(BlueprintReadOnly, Category = "Tutorial|Telemetry")
    FDateTime SessionStartTime;

    // Human-readable UTC start time
    UPROPERTY(BlueprintReadOnly, Category = "Tutorial|Telemetry")
    FString SessionStartUTC;


    // Unix timestamp of the session start
    UPROPERTY(BlueprintReadOnly, Category = "Tutorial|Telemetry")
    int64 SessionStartUnix = 0;

    // Tracks the next gameplay-event sequence number
    UPROPERTY(BlueprintReadOnly, Category = "Tutorial|Telemetry")
    int32 EventSequence = 0;

    // Recorded gameplay events
    UPROPERTY(BlueprintReadOnly, Category = "Tutorial|Telemetry")
    TArray<FTUTGameplayEvent> GameplayEvents;

    // Indicates whether a playtesting session has started
    UPROPERTY(BlueprintReadOnly, Category = "Tutorial|Telemetry")
    bool bSessionStarted = false;

    // Path where telemtry was most recently exported
    UPROPERTY(BlueprintReadOnly, Category = "Tutorial|Telemetry")
    FString LastTelemetryFilePath;
};

